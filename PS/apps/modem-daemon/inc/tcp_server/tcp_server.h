/**
 * Флайл: tcp_server.h
 *
 * Описание:
 */

#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <memory>

class endpoint_ipv4;

class tcp_server
{
public:
	tcp_server() = default;
	virtual ~tcp_server() = default;

	/**
	 * @brief запуск сервера
	 */
	virtual void run(std::unique_ptr<endpoint_ipv4>& ep) = 0;

	/**
	 * @brief остановка сервера
	 */
	virtual void stop() = 0;

	tcp_server(const tcp_server& copy) = delete;
	tcp_server& operator=(const tcp_server& copy) = delete;
};

#endif //_TCP_SERVER_H_
