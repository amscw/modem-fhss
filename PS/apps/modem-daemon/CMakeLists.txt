cmake_minimum_required( VERSION 3.3.2 )
project(modem-daemon)

# Ключи компиляции
add_compile_options(-std=c++14 -Wall)

# Сборка библиотеки python-api
add_subdirectory(python_lib)
add_library(python-api STATIC $<TARGET_OBJECTS:lib_o>)
target_link_libraries(python-api libpython3.6m.so)

# Сборка исполняемого файла приложения modem-daemon
add_subdirectory(src)
add_executable(${PROJECT_NAME}
	$<TARGET_OBJECTS:user_msg_o>
	$<TARGET_OBJECTS:common_o>
	$<TARGET_OBJECTS:config_o>
	$<TARGET_OBJECTS:daemon_o>
	$<TARGET_OBJECTS:event_o>
	$<TARGET_OBJECTS:logs_o>
	$<TARGET_OBJECTS:modem_conf_o>
	$<TARGET_OBJECTS:network_api_o>
	$<TARGET_OBJECTS:endpoint_o>
	$<TARGET_OBJECTS:tcp_connection_o>
	$<TARGET_OBJECTS:daemon_process_o>
	$<TARGET_OBJECTS:master_process_o>
	$<TARGET_OBJECTS:worker_process_o>
	$<TARGET_OBJECTS:raii_thread_o>
	$<TARGET_OBJECTS:selector_o>
	$<TARGET_OBJECTS:tcp_server_async_o>
	$<TARGET_OBJECTS:tcp_server_sync_o>
	$<TARGET_OBJECTS:master_thread_o>
	$<TARGET_OBJECTS:worker_thread_o>
	$<TARGET_OBJECTS:transport_o>
	$<TARGET_OBJECTS:main_o>
)
target_link_libraries(${PROJECT_NAME} python-api -lpthread -lyaml-cpp -lstdc++)

# Установка приложения
install(TARGETS ${PROJECT_NAME} DESTINATION /bin/)

# Установка скриптов
install(PROGRAMS  scripts/modem_fhss_api.py DESTINATION /root/)
install(PROGRAMS  scripts/modem-daemon DESTINATION /root/)

# Установка файла конфигурации
install(FILES settings/config.yaml DESTINATION /etc/modem-daemon/)
