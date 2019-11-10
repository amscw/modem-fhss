/**
 * Файл: server_config.h
 *
 * Описание: класс server_config реализует функционал
 *           для работы с конфигурационным файлом сервера,
 *           класс использует паттерн singleton
 */

#ifndef _SERVER_CONFIG_H_
#define _SERVER_CONFIG_H_

#include <yaml-cpp/yaml.h>

#include "destroyer_singleton.h"
#include "config_struct.h"

class server_config
{
public:
	/**
	 * @brief инстанцирование класса singleton
	 *
	 * @return ссылка на единственный экземпля класса
	 */
	static server_config* get_instance() noexcept;

	/**
	 * @brief удаление единосвенного экземпляра класса singleton
	 */
	void delete_instance() noexcept;

	/**
	 * @brief загрузка и чтение конфиг файла, для разбора используется библиотека yaml-cpp
	 *
	 * @param путь до конфиг файла
	 *
	 * @throw YAML::ParserException
	 */
	void load_config_file(const std::string& file_name);

	/**
	 * @brief полчение указателя на структуру c конфигом
	 *
	 * @return указатель на структуру
	 */
	const config_struct* get_config() noexcept;

	server_config(const server_config& copy) = delete;
	server_config& operator=(const server_config& copy) = delete;

private:
	/**
	 * @brief имя приложения
	 *
	 * @throw YAML::ParserException
	 */
	void get_server_name();

	/**
	 * @brief тип сервера
	 *
	 * @throw YAML::ParserException
	 */
	void get_server_type();

	/**
	 * @brief версия ip адрреса (ip_v4 или ip_v6) из файла конфигурации сервера
	 *
	 * @throw YAML::ParserException
	 */
	void get_ip_version();

	/**
	 * @brief ip адррес из файла конфигурации сервера
	 *
	 * @throw YAML::ParserException
	 */
	void get_ip_address();

	/**
	 * @brief порт сетевого соединиения
	 *
	 * @throw YAML::ParserException
	 */
	void get_port();

	/**
	 * @brief директория файла лога выполненых действий из файла конфигурации сервера
	 *
	 * @throw YAML::ParserException
	 */
	void get_access_log();

	/**
	 * @brief директория файла лога ошибок из файла конфигурации сервера
	 *
	 * @throw YAML::ParserException
	 */
	void get_error_log();

	/**
	 * @brief директория pid файла из файла конфигурации сервера
	 *
	 * @throw YAML::ParserException
	 */
	void get_pidfile();

	void get_packet_size();

private:
	server_config() noexcept;
	~server_config() noexcept;

	static server_config* instance_;
	static destroyer_singleton<server_config> destroyer_;

private:
	YAML::Node config_;
	static config_struct* cfg_;
};

#endif //_SERVER_CONFIG_H_
