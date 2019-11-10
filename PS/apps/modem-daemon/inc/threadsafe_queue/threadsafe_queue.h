/**
 * Флайл: threaddafe_queue.h
 *
 * Описание: потокобезопасная очередь
*/

#ifndef _THREADSAFE_QUEUE_H_
#define _THREADSAFE_QUEUE_H_

#include <mutex>
#include <queue>
#include <condition_variable>

template <class T>
class threadsafe_queue
{
public:
	threadsafe_queue() = default;
	~threadsafe_queue() = default;

	/**
	 * @brief помещение lvalue данных в очередь
	 *
	 * @param buffer буффер для записи данных
	 */
	 void push(const T& buffer);

	/**
	 * @brief помещение rvalue данных в очередь
	 *
	 * @param buffer буффер для записи данных
	 */
	 void push(T&& buffer);

	 /**
	 * @brief получение данных из очереди
	 *
	 * @param buffer буффер для чтения данных
	 */
	T pop();

	/**
	* @brief получение данных из очереди
	*
	* @param buffer буффер для чтения данных
	*/
	bool is_empty();

	threadsafe_queue(const threadsafe_queue &copy) = default;
	threadsafe_queue &operator=(const threadsafe_queue &copy) = default;

private:
	//мьютекс
	mutable std::mutex mut;

	//очередь
	std::queue<T> data_queue;

	//условная переменная, сигнализирующая о том, что можно считывать данные из очереди
	std::condition_variable data_cond;
};

template <class T>
void threadsafe_queue<T>::push(const T& buffer)
{
	std::lock_guard<std::mutex> lk(mut);

	data_queue.push(buffer);

	data_cond.notify_one();
}

template <class T>
void threadsafe_queue<T>::push(T&& buffer)
{
	std::lock_guard<std::mutex> lk(mut);

	data_queue.push(std::move(buffer));

	data_cond.notify_one();
}

template <class T>
T threadsafe_queue<T>::pop()
{
	std::unique_lock<std::mutex> lk(mut);
	data_cond.wait(lk, [this] { return !data_queue.empty(); });

	T buffer = std::move(data_queue.front());
	data_queue.pop();

	return std::move(buffer);
}

template <class T>
bool threadsafe_queue<T>::is_empty()
{
	return data_queue.empty();
}

#endif //_THREADSAFE_QUEUE_H_
