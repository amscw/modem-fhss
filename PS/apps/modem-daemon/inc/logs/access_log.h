/**
 *  Файл: access_log.h
 *
 *  Описание: Реализация логгирования успешно выполненных действий
 */

#ifndef _ACCESS_LOG_H_
#define _ACCESS_LOG_H_

#include "server_log.h"
#include "destroyer_singleton.h"

class access_log : public server_log
{
public:
	/**
	 * @brief инстансирование единосвенного экземпляра класса singleton
	 *
	 * @return указатель на единственный экземпляр класса access_log
	 */
	static access_log* get_instance() noexcept;

	/**
	 * @brief удаление единосвенного экземпляра класса singleton
	 */
	void delete_instance() noexcept;
	
	/**
	 * @brief инициализация лог файла
	 *
	 * @param file_name директрия до лог файла
	 *
	 *@throw user_tool_exception
	 */
	void init_log_file(const std::string& file_name) override;

	access_log(const access_log& copy) = delete;
	access_log& operator=(const access_log& copy) = delete;

private:
	access_log() = default;
	~access_log()= default;

private:
	static access_log* log_;
	static destroyer_singleton<access_log> destroyer_;
};

#endif //_ACCESS_LOG_H_
