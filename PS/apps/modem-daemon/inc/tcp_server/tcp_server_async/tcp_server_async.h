/**
 * Флайл: tcp_server_async.h
 *
 * Описание:
 */

#ifndef _TCP_SERVER_ASYNC_H_
#define _TCP_SERVER_ASYNC_H_

#include "tcp_server.h"

template <typename... Args>
class event;

class tcp_server_async : public tcp_server
{
public:
	// Событие: остановка master_thread
	std::unique_ptr<event<void>> stop_master_threads_;

	// Событие: остановка worker_thread
	std::unique_ptr<event<void>> stop_worker_threads_;

public:
	tcp_server_async();
	~tcp_server_async();

	/**
	 * @brief запуск сервера
	 */
	void run(std::unique_ptr<endpoint_ipv4>& ep) noexcept override;

	/**
	 * @brief остановка сервера
	 */
	void stop() noexcept override;

	tcp_server_async(const tcp_server_async& copy) = delete;
	tcp_server_async& operator=(const tcp_server_async& copy) = delete;

private:
	class impl;
	std::unique_ptr<impl> pimpl_;
};

#endif //_TCP_SERVER_ASYNC_H_
