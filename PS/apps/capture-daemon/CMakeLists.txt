# Сборочный файл для программы

# Минимальная версия CMake
cmake_minimum_required( VERSION 3.3.2 )

# Используемый в исходном коде стандарт языка C++
set ( CMAKE_CXX_STANDARD 14 )

set ( CMAKE_CXX_STANDARD_REQUIRED on )

# Устанавливаем флаги компилятора
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14" )

# Устанавливаем флаги компилятора
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++" )

# Устанавливаем флаги компилятора
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread" )

# Устанавливаем флаги компилятора
#set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lyaml-cpp" )

# Устанавливаем значение переменной PROJECT
set ( PROJECT capture-daemon )

# Имя проекта
project ( ${PROJECT} )

# Указываем пути, где искать заголовочники, дополнительно к стандартным путям.
include_directories ( "inc"
					"inc/common"
					"inc/common/user_msg"
					"inc/daemon"
					"inc/logs"
					"inc/config"
					"inc/network_api/"
					"inc/network_api/endpoint"
					"inc/network_api/tcp_connection"
					"inc/process/"
					"inc/process/daemon_process"
					"inc/process/master_process"
					"inc/process/worker_process")

# Собираем исходные файлы программы
aux_source_directory( src SOURCES )
aux_source_directory( src/common SOURCES )
aux_source_directory( src/common/user_msg SOURCES )
aux_source_directory( src/daemon SOURCES )
aux_source_directory( src/logs SOURCES )
aux_source_directory( src/config SOURCES )
aux_source_directory( src/network_api SOURCES )
aux_source_directory( src/network_api/endpoint SOURCES )
aux_source_directory( src/network_api/tcp_connection SOURCES )
aux_source_directory( src/process SOURCES )
aux_source_directory( src/process/daemon_process SOURCES )
aux_source_directory( src/process/master_process SOURCES )
aux_source_directory( src/process/worker_process SOURCES )

# Указываем путь к исполнеямому файлу
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Собираем исполняемый файл с именем
add_executable ( ${PROJECT} ${SOURCES} )

# Линковка исполняемого файла
target_link_libraries( ${PROJECT} -pthread -lyaml-cpp )
