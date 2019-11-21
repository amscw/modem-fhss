/**
 * Флайл: worker_thread.h
 *
 * Описание: класс реализющий worker поток
 */

#ifndef _WORKER_THREAD_H_
#define _WORKER_THREAD_H_

#include "event_impl.h"
#include "modem_conf.h"
#include "selector.h"
#include "threadsafe_queue.h"

#include <thread>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>

#include "tracers.h"
#include "modem-sender.h"
#include "modem-receiver.h"

#define DEBUG_MODEM_SERVER

class cancellation_token;

class worker_thread
{
	// Размер принимаемых/передаваемых данных
	enum { MSG_LENGTH = 1024 };

	enum class data_type : std::uint32_t {
		BLOCK_TYPE_CONFIG = 0,			// set|get|send <register>|<msg> <param> (legacy) 
		BLOCK_TYPE_SERVER_MESSAGE,		// текстовый ответ сервера
		BLOCK_TYPE_SEND_TEXT,			// отправка текста через модем
		BLOCK_TYPE_DATA_REQ,			// запрос принятых через мрдем данных
		BLOCK_TYPE_SEND_IMAGE,			// отправка изображения через модем
		 
	};

	const long MAX_WAIT_TIME = 5000;
	const long MAX_WAIT_TIME_RX = 1000;
	const int MAX_PACKET_SIZE = 107;

public:
	// Событие: данные можно принимать
	// и обрабатывать
	std::unique_ptr<event<void>> data_ready_;

	// Событие: соединение на стороне клиента
	// закрыто - уничтожить объект connection
	// для этого соединения
	std::unique_ptr<event<int>> connection_delete_ready_;

public:
	worker_thread() : data_ready_(std::make_unique<event<void>>()),
		connection_delete_ready_(std::make_unique<event<int>>()),
		buffer_(std::vector<byte>(MSG_LENGTH)),
		client_block_size_(0),
		modem_block_size_(0),
		select_connection_(std::make_unique<selector>()),
		modem_conf_(std::make_unique<modem_conf>()),
		m_nTimeoutOp(0), m_nTimeoutRx(0),
		fd_port1_(-1), fd_port2_(-1),
		m_bModEnabled(false)
		{
			int fd, bytes; 
			std::ostringstream oss;

			if ((fd = open("/sys/mfhssdrv-registers/m_en_mod", O_RDONLY)) != -1)
			{
				std::unique_ptr<char[]> ubuff(new char[32]);
				memset (ubuff.get(), 0, 32);
				bytes = read(fd, ubuff.get(), 32);
				close (fd);
				std::string str(ubuff.get());
				std::istringstream(str) >> m_bModEnabled;
				oss << "modem is " << (m_bModEnabled ? "transmitter" : "receiver"); 
				LOG_STREAM(oss);
			}
		};

	~worker_thread() = default;

	/**
	 * @brief точка входа в master поток
	 */
	void run(cancellation_token& token, std::unique_ptr<threadsafe_queue<std::unique_ptr<connection>>> &queue) noexcept;

	worker_thread(const worker_thread& copy) = delete;
	worker_thread& operator=(const worker_thread& copy) = delete;

private:
	/**
	 * @brief прием данных
	 */
	void data_receive(std::unique_ptr<connection> &tcp) noexcept;
	
	/**
	 * @brief обработка данных
	 */
	void data_process(std::unique_ptr<connection> &tcp, std::vector<byte> v) noexcept;

	/**
	 * @brief отсылка данных клиенту
	 */	
	void data_answer(std::unique_ptr<connection> &tcp, const std::string &message) noexcept;						// текстовый ответ
	void data_answer(std::unique_ptr<connection> &tcp, enum data_type type, const std::vector<byte> &v) noexcept;	// для первой посылки
	void data_answer(std::unique_ptr<connection> &tcp, const std::vector<byte> &v) noexcept;						// для остальной части блока

	void listen_modem() noexcept;
	void write_modem() noexcept;

	inline void dump(std::vector<uint8_t> &buf) __attribute__((always_inline))
	{
		std::ostringstream oss;
		oss << "total: " << buf.size() << " bytes";
		LOG_STREAM(oss);
		oss << std::hex;
		for (uint8_t n : buf)
			oss << "0x" << (int)n << " ";
		LOG_STREAM(oss);
	}

	long getCurrentTimeMs (void) noexcept;
	/* inline */ void stopwatchStart(long &timer) noexcept { timer = getCurrentTimeMs(); }
    /* inline */ long stopwatchStop(const long &timer) noexcept { return (getCurrentTimeMs() - timer); }

private:
	// Объект запускаемого потока
	std::thread thread_;

	// Буффер данных
	std::vector<byte> buffer_;
	
	// Блок данных от клиента
	std::vector<byte> block_from_client_;
	uint16_t client_block_size_;

	// Блок данных от модема
	std::vector<byte> block_from_modem_;
	uint16_t modem_block_size_;

	// Блок данных для модема 
	std::vector<byte> block_to_modem_;

	// Блок данных для клиента
	std::vector<byte> block_to_client_;

	
	// Объект selector
	std::unique_ptr<selector> select_connection_;

	// Объект modem_conf
	std::unique_ptr<modem_conf> modem_conf_;

	long m_nTimeoutOp, m_nTimeoutRx;

	int fd_port1_, fd_port2_;
	bool m_bModEnabled;
};

#endif //_WORKER_THREAD_H_
