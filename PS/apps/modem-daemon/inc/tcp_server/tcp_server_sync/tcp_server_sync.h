/**
 * Флайл: tcp_server_sync.h
 *
 * Описание:
 */

#ifndef _TCP_SERVER_SYNC_H_
#define _TCP_SERVER_SYNC_H_

#include "tcp_server.h"

class tcp_server_sync : public tcp_server
{
public:
	tcp_server_sync() = default;
	~tcp_server_sync() = default;

	/**
	 * @brief запуск сервера
	 */
	void run(std::unique_ptr<endpoint_ipv4>& ep) noexcept override;

	/**
	 * @brief остановка сервера
	 */
	void stop() noexcept override;
	
	tcp_server_sync(const tcp_server_sync& copy) = delete;
	tcp_server_sync& operator=(const tcp_server_sync& copy) = delete;
};

#endif //_TCP_SERVER_SYNC_H_
