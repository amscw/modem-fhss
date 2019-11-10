/**
 * Флайл: raii_thread.h
 *
 * Описание:  обертка над потоком std::thread
 */

#ifndef _RAII_THREAD_H_
#define _RAII_THREAD_H_

#include <atomic>
#include <thread>

class cancellation_token
{
public:
	cancellation_token() : cancelled_(false) {};
	~cancellation_token() = default;

	explicit operator bool() const { return !cancelled_; };

	void cancel() { cancelled_ = true; };

	cancellation_token(const cancellation_token& copy) = delete;
	cancellation_token& operator=(const cancellation_token& copy) = delete;

private:
	// Булевый флаг
	std::atomic<bool> cancelled_;
};

class raii_thread
{
public:
	/**
	 * @brief запуск потока в конструкторе
	 */
	template <typename Function, typename... Args>
	raii_thread(Function&& f, Args&&... args) noexcept;

	/**
	 * @brief присоединение потока в деструкторе
	 */
	~raii_thread() noexcept;

	/**
	 * @brief корректное завершение потока
	 */
	void cancel() noexcept;

	raii_thread(const raii_thread& copy) = delete;
	raii_thread& operator=(const raii_thread& copy) = delete;

private:
	// Объект std::thread
	std::thread thread_;

	// Булевый флаг
	cancellation_token token_;
};

template <typename Function, typename... Args>
raii_thread::raii_thread(Function&& f, Args&&... args) noexcept
{
	thread_ = std::thread(std::forward<Function>(f), std::ref(token_), std::forward<Args>(args)...);
}

#endif //_RAII_THREAD_H_
