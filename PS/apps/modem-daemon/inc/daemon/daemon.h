/**
 * Флайл: daemon.h
 *
 * Описание: перевод программы в режим демона
 */

#ifndef _DAEMON_H_
#define _DAEMON_H_

#include <memory>
#include <string>

class daemon_tool
{
public:
	daemon_tool();
	~daemon_tool();

	/**
	 * @brief инициализция файла конфигурации
	 *
	 * @throw user_tool_exception
	 */
	static void init_config();

	/**
	 * @brief инициализция лог файлов
	 *
	 * @throw user_tool_exception
	 */
	static void init_log();

	/**
	 *@brief перевод программы в режим демона
	 *
	 *@throw user_tool_exception
	 */
	void start_daemon();

	daemon_tool(const daemon_tool& copy) = delete;
	daemon_tool& operator=(const daemon_tool& copy) = delete;

private:
	class impl;
	std::unique_ptr<impl> pimpl_;
};

#endif //_DAEMON_H_
