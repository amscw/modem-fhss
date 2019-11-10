/**
 * Флайл: selector.h
 *
 * Описание: мультиплексор соединений
 */

#ifndef _SELECTOR_H_
#define _SELECTOR_H_

#include "connection.h"
#include "event_impl.h"

#include <map>
#include <memory>
#include <sys/epoll.h>

class selector
{
public:
	// Событие: пришли данные в сокете
	std::unique_ptr<event<std::unique_ptr<connection>&>> epoll_data_ready_;

public:
	selector() : epoll_data_ready_(std::make_unique<event<std::unique_ptr<connection>&>>()), epoll_(epoll_create1(0)) {};
	~selector() = default;

	/**
	 * @brief регистрация соединения
	 */
	int register_connect(std::unique_ptr<connection> tcp) noexcept;


	/**
	 * @brief получить ссылку на соединение по дескриптору сокета
	 */
	std::unique_ptr<connection> &tcp(int socket_id);

	/**
	 * @brief ожидание событий
	 */
	void wait_event() noexcept;

	/**
	 * @brief удаление объекта connection
	 * отключившегося клиента
	 */
	void connection_delete(int slave_socket) noexcept;

	selector(const selector& copy) = delete;
	selector& operator=(const selector& copy) = delete;

private:
	/**
	 * @brief Перевод сокета в неблокирующий режим
	 */
	int set_nonblock(const int &fd) noexcept;

private:
	// Дескриптор, указывающий на экземпляр epoll
	int epoll_;

	// Максимальное количество обработанных событий за один раз
	static const int max_events_;

	// Таймаут на epoll
	static const int epoll_timeout_;

	// Контейнер для хранения пары: номер дескриптора и объект connection, соответсвующий ему
	std::map<int, std::unique_ptr<connection>> conn_;
};

#endif //_SELECTOR_H_
