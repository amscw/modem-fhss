/**
 *  Файл: config_struct.h
 *
 *  Описание: Структура, содержащий инфломации из конфиг файла
 *
 */

#ifndef _CONFIG_STRUCT_H_
#define _CONFIG_STRUCT_H_

#include <string>

/* Структра для харнения основной конфигурацию акпа сп-фк */
struct config_struct
{
	std::string name_;						// имя сервера

	std::string type_;						// тип сервера

	/* Структра для хранения инцормации о сетевом соединении */
	struct server_connection
	{
		std::size_t ip_version_;			// версия ip адреса
		std::string ip_address_;			// ip адрес
		std::uint16_t port_;				// порт
	};

	struct server_directories
	{
		/* Структура для хранения информации о лог файлах */
		struct server_logs
		{
			std::string error_log_;			// директория до файла логирования ошибок
			std::string access_log_;		// директория до файла логирования выполненных действий
		};

		/* Струтра для хранения информации о pid файле */
		struct server_pid
		{
			std::string pid_path_;			// директория до pid файла
		};

		server_logs logs_;
		server_pid pid_;
	};

	server_connection connection_;
	server_directories directories_;
};

#endif //_CONFIG_STRUCT_H_
