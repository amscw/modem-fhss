#include "raii_thread.h"
#include "selector.h"
#include "worker_thread.h"


#include <stdexcept>

// data_ready_ - событие "поток готов принять и обработать данные"

// WARNING: для модема - только непрерывная поточная передача! Так как границы блоков могут быть не достоверны
// следовательно, передавать начинаем только после полной загрузки файла в RAM 

void worker_thread::run(cancellation_token& token, std::unique_ptr<threadsafe_queue<std::unique_ptr<connection>>> &queue) noexcept
{
	std::ostringstream oss;
	int fd;
	long timeElapsed;

	data_ready_->connect(std::bind(&selector::wait_event, std::ref(select_connection_)));												// подписывает selector_connection_ на свое событие data_ready_
	data_ready_->connect(std::bind(&worker_thread::listen_modem, this));
	
	select_connection_->epoll_data_ready_->connect(std::bind(&worker_thread::data_receive, this, std::placeholders::_1)); 				// подписывается на событие селектора epoll_data_ready
	connection_delete_ready_->connect(std::bind(&selector::connection_delete, std::ref(select_connection_),	std::placeholders::_1));	// подписывает select_connection_ на свое событие connection_delete_ready_

	while(token)	// объект с перегруженным оператором приведения типа к bool. 
	{
		while (!queue->is_empty())
		{
			std::unique_ptr<connection> tcp = std::move(queue->pop());
			if (tcp->get_slave_port() == 4097)
			{
				fd_port1_ = tcp->get_slave_socket();
				oss << "port1 connected over TCP";
				LOG_STREAM(oss);
			}
			if (tcp->get_slave_port() == 4098)
			{
				fd_port2_ = tcp->get_slave_socket();
				oss << "port2 connected over TCP";
				LOG_STREAM(oss);
			}
			select_connection_->register_connect(std::move(tcp));
		}
		data_ready_->emit();	// select_connection_->wait_event(), далее будет вызван data_receive(), если событие имело место быть
	}
}

void worker_thread::listen_modem() noexcept
{
	std::ostringstream oss;
	int fd;
	ssize_t bytes;
	std::unique_ptr<byte[]> pBuffer(new byte[MSG_LENGTH]);

	if ((fd = open("/dev/mfhss0", O_RDONLY)) != -1)
	{
		block_from_modem_.clear();
		stopwatchStart(m_nTimeoutOp);
		if ((bytes = read(fd, pBuffer.get(), MSG_LENGTH)) > 0)
		{
			// забираем что прочитали
			block_from_modem_.insert(block_from_modem_.end(), pBuffer.get(), pBuffer.get() + bytes);
			
			// отладка
			#ifdef DEBUG_MODEM_SERVER
			oss << "received from modem: " << bytes << " bytes, elapsed: " << stopwatchStop(m_nTimeoutOp) << "ms";
			dump(block_from_modem_);
			LOG_STREAM(oss); 
			#endif // DEBUG_MODEM_SERVER
			
			// отправка в TCP
			try
			{
				select_connection_->tcp(fd_port1_)->send_data(block_from_modem_);
				select_connection_->tcp(fd_port2_)->send_data(block_from_modem_);
			} catch (std::out_of_range) {
				oss << "cannot transmit data from modem: uart not connected";
				LOG_STREAM(oss);
			}
		}	
		close(fd);		
	}  
}

// сквозная передача в модем
void worker_thread::data_receive(std::unique_ptr<connection> &tcp) noexcept
{
	std::ostringstream oss;
	std::int64_t recv_result;
	int slave_socket = tcp->get_slave_socket(), fd;
	size_t bytes;
	long timeElapsed;

	// выгребаем все, что есть
	// специальные возвращенные значения:
	// 0 - закрыть сокет 
	// -1 - данные кончились
	while ((recv_result = tcp->receive_data(buffer_)) > 0 )
	{
		if (block_from_client_.size() == 0)
		{
			// Принимается следующая посылка
			stopwatchStart(m_nTimeoutRx);
		}
		// block_from_client_.reserve(block_from_client_.size() + recv_result);
		block_from_client_.insert(block_from_client_.end(), buffer_.begin(), buffer_.begin() + recv_result);
	}

	if (recv_result == 0)
	{
		shutdown(slave_socket, SHUT_RDWR);
		close(slave_socket);
		connection_delete_ready_->emit(slave_socket);	// select_connection_->connection_delete(slave_socket)
		block_from_client_.clear();
		client_block_size_ = 0;
		return;
	}

	//!!! FOR DEBUG ONLY !!!
	//dump(block_from_client_);
	// block_from_client_.clear();
	// return;

	oss << "received: " << block_from_client_.size() << " bytes";
	LOG_STREAM(oss);

	// если модем является приемником - передавать ничего не можем!
	// if (!m_bModEnabled)
	// {
	// 	block_from_client_.clear();
	// 	return;
	// } // теперь можем =)


	if (block_from_client_.size() > packet_size_)
	{
		if ((fd = open ("/dev/mfhss0", O_WRONLY)) != -1)
		{
			while(block_from_client_.size() >= packet_size_)
			{
				write(fd, block_from_client_.data() + bytes, packet_size_);
				block_from_client_.erase(block_from_client_.begin(), block_from_client_.begin() + packet_size_);
				oss << "send next packet (" << packet_size_ << " bytes), remain: " << block_from_client_.size() << bytes;
				LOG_STREAM(oss);
				stopwatchStart(m_nTimeoutRx);
			}
			close(fd);
		} else {
			block_from_client_.clear();
		}
	}
}

void worker_thread::data_process(std::unique_ptr<connection> &tcp, std::vector<byte> nextBlock) noexcept
{
	std::ostringstream oss;

	switch (static_cast <enum data_type>(nextBlock[2]))
	{
		case data_type::BLOCK_TYPE_CONFIG:
			// блок содержит legacy-строку конфигурации
			nextBlock.erase(nextBlock.begin(), nextBlock.begin() + 3);
			modem_conf_->run_conf(nextBlock, nextBlock.size());
			oss << modem_conf_->get_msg_result();
			break;

		case data_type::BLOCK_TYPE_SEND_IMAGE:
		case data_type::BLOCK_TYPE_SEND_TEXT:
			// блок содержит данные для передачи через модем
			if (block_to_modem_.size() == 0)
			{
				block_to_modem_ = std::move(nextBlock);
				oss << "start write data to modem";
				// выключить приемник
				// ...
			} else oss << "error: cannot write - modem busy";
			break;

		case data_type::BLOCK_TYPE_DATA_REQ:
			// блок содержит запрос на выдачу данных клиенту
			if (block_to_client_.size() > 0)
			{
				// блок уже отформатирован, можно слать
				tcp->send_data(block_to_client_);
				block_to_client_.clear();
				return;
			} else oss << "no data received or busy";
			break;

		default:
			oss << "unknown block type: 0x" << std::hex << (int)nextBlock[2];
	}

	// сообщить клиенту результат обработки
	data_answer(tcp, oss.str());
	LOG_STREAM(oss);
}

void worker_thread::data_answer(std::unique_ptr<connection> &tcp, const std::string &message) noexcept
{
	std::ostringstream oss;
	uint16_t block_size = message.length() + sizeof(uint16_t) + sizeof(uint8_t);
	std::vector<byte> v;

	v.emplace_back((byte)(block_size >> 8));
	v.emplace_back((byte)(block_size));
	v.emplace_back((byte)(data_type::BLOCK_TYPE_SERVER_MESSAGE));
	v.reserve(v.size() + message.length());
	v.insert(v.end(), message.begin(), message.end());
	tcp->send_data(v);
}


long worker_thread::getCurrentTimeMs (void) noexcept
{
   	struct timespec spec;
   	clock_gettime(CLOCK_REALTIME, &spec);
   	return round(spec.tv_sec*1.0e3 + spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds;
}
	