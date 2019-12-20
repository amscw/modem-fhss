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

	data_ready_->connect(std::bind(&selector::wait_event, std::ref(select_connection_)));
	data_ready_->connect(std::bind(&worker_thread::write_modem, this));
	data_ready_->connect(std::bind(&worker_thread::listen_modem, this));
	
	select_connection_->epoll_data_ready_->connect(std::bind(&worker_thread::data_receive, this, std::placeholders::_1)); 				// подписывается на событие селектора epoll_data_ready
	connection_delete_ready_->connect(std::bind(&selector::connection_delete, std::ref(select_connection_),	std::placeholders::_1));	// подписывает select_connection_ на свое событие connection_delete_ready_

	while(token)	// объект с перегруженным оператором приведения типа к bool. 
	{
		while (!queue->is_empty())
		{
			std::unique_ptr<connection> tcp = std::move(queue->pop());
			select_connection_->register_connect(std::move(tcp));
		}
		data_ready_->emit();	// select_connection_->wait_event(), далее будет вызван data_receive(), если событие имело место быть
	}
}


// факт окончания приема индицируется по таймауту
void worker_thread::listen_modem() noexcept
{
	std::ostringstream oss;
	int fd;
	ssize_t bytes;
	std::vector<byte> buf;
	buf.reserve(1024);
	long timeElapsed; 

	if ((fd = open("/dev/mfhss0", O_RDONLY)) != -1)
	{
		bytes = read(fd, buf.data(), buf.capacity());
		if (bytes > 0)
		{
			// перемещение данных в блок
			block_from_modem_.reserve(block_from_modem_.size() + bytes);
			block_from_modem_.insert(block_from_modem_.end(), buf.begin(), buf.begin() + bytes);
			oss << "--read: " << block_from_modem_.size() << " bytes";
			LOG_STREAM(oss);
			stopwatchStart(m_nTimeoutOp);
		} else if (bytes == 0 && block_from_modem_.size() > 0 &&  ( (timeElapsed = stopwatchStop(m_nTimeoutOp)) >= 3000 ) ) {
			oss << "end of block, elapsed: " << timeElapsed << " ms";
			LOG_STREAM(oss);
			// больше не ждем, завершаем приём
			if (block_from_modem_.size() < 3)
			{
				// блок невалиден - маленький размер
				oss << "error: block too small-" << block_from_modem_.size() << " bytes";
			} else {
				modem_block_size_ = block_from_modem_[0] << 8 | block_from_modem_[1];
				if (modem_block_size_ == block_from_modem_.size())
				{
					// блок валиден, можно отдать клиенту
					oss << "modem read complete (total: " <<  modem_block_size_ << " bytes)";
					dump(block_from_modem_);
					block_to_client_ = std::move(block_from_modem_);
				} else {
					// блок невалиден - несовпадение по размеру
					oss << "error: received-" << block_from_modem_.size() << ", need-" << modem_block_size_ << " bytes";
				}
				block_from_modem_.clear();
				modem_block_size_ = 0;		
			}
			LOG_STREAM(oss);		
		}
		close(fd);		
	}  
}

void worker_thread::write_modem() noexcept
{
	std::ostringstream oss;
	int fd;
	static ssize_t bytes = 0;
	
	if ((block_to_modem_.size() > 0) && ((fd = open ("/dev/mfhss0", O_WRONLY)) != -1))
	{
		bytes += write(fd, block_to_modem_.data() + bytes, block_to_modem_.size() - bytes);
		if (bytes == block_to_modem_.size())
		{
			oss << "modem write complete (total: " << bytes << " bytes)";
			dump(block_to_modem_);
			// уничтожить блок
			bytes = 0;
			block_to_modem_.clear();
			
		} else oss << "--write: " << bytes << "/" << block_to_modem_.size() << " bytes";
		LOG_STREAM(oss);
		close (fd);
	} 
}

// возможны ситуации:
// была принята только часть блока
// было принято несколько блоков
void worker_thread::data_receive(std::unique_ptr<connection> &tcp) noexcept
{
	std::ostringstream oss;
	std::int64_t recv_result;
	int slave_socket = tcp->get_slave_socket();
	std::vector<byte> nextBlock;

	// выгребаем все, что есть
	// специальные возвращенные значения:
	// 0 - закрыть сокет 
	// -1 - данные кончились
	while ((recv_result = tcp->receive_data(buffer_)) > 0 )
	{
		block_from_client_.reserve(block_from_client_.size() + recv_result);
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

	if (client_block_size_ == 0)
	{
		if (block_from_client_.size() >= sizeof(uint16_t))
			client_block_size_ = block_from_client_[0] << 8 | block_from_client_[1]; 
		else return;
	}

	// обработать все принятые блоки
	while (block_from_client_.size() >= client_block_size_)
	{
		std::copy(block_from_client_.begin(), block_from_client_.begin() + client_block_size_, std::back_inserter(nextBlock));
		data_process(tcp, std::move(nextBlock));			
		block_from_client_.erase(block_from_client_.begin(), block_from_client_.begin() + client_block_size_);
		if (block_from_client_.size() >= sizeof(uint16_t))
		{
			client_block_size_ = block_from_client_[0] << 8 | block_from_client_[1];
		} else {
			// все блоки закончились
			client_block_size_ = 0;
			break;
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
	
