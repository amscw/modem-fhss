/**
 * Флайл: master_thread.h
 *
 * Описание: класс реализющий master поток
 */

#ifndef _MASTER_THREAD_H_
#define _MASTER_THREAD_H_

#include <memory>

class connection;

class endpoint_ipv4;

class cancellation_token;

template <class T>
class threadsafe_queue;

class master_thread
{
public:
	master_thread() = default;
	~master_thread() = default;

	/**
	 * @brief точка входа в master поток
	 */
	void run(cancellation_token& token, std::unique_ptr<threadsafe_queue<std::unique_ptr<connection>>> &queue,
	  std::unique_ptr<endpoint_ipv4> &ep) noexcept;

	master_thread(const master_thread& copy) = delete;
	master_thread& operator=(const master_thread& copy) = delete;
};

#endif //_MASTER_THREAD_H_
