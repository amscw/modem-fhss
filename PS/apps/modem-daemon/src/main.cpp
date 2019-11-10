#include "daemon.h"
#include "daemon_tool_exception.h"
#include "error_log.h"
#include "server_config.h"
#include "server_log_exception.h"
#include "system_exception.h"

#include <iostream>
#include <memory>

int main(int argc, char* argv[])
{
	try
	{
		daemon_tool::init_config(); 					// Инициализация системы конфигурации
		daemon_tool::init_log(); 						// Инициализация системы логирования

		auto daemon = std::make_unique<daemon_tool>();
		daemon->start_daemon();
	}
	catch (daemon_tool_exception& ex)
	{
		error_log::get_instance()->save_log(ex.what()); // Обработка ошибок при переводе в режим демона
	}
	catch(server_log_exception& ex)
	{
		error_log::get_instance()->save_log(ex.what()); // Обработка ошибок при содании лог файлов
	}
	catch(system_exception& ex)
	{
		error_log::get_instance()->save_log(ex.what()); // Перехват всех типов системных исключений
														//	(нехватка памяти и т.д.)
	}
	catch(YAML::ParserException &ex)
	{
		std::cout << ex.what() << "\n"; 				// Обработка ошибок при загрузке файла конфигурации
	}

	return 0;
}
