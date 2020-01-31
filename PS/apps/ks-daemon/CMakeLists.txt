cmake_minimum_required( VERSION 3.3.2 )
project(ks-daemon)

# Ключи компиляции
add_compile_options(-std=c++14 -Wall)

# Сборка исполняемого файла приложения
add_subdirectory(src)
add_executable(${PROJECT_NAME}	$<TARGET_OBJECTS:main_o>)
target_link_libraries(${PROJECT_NAME} -lstdc++ -lyaml-cpp)

# Установка приложения
install(TARGETS ${PROJECT_NAME} DESTINATION /bin/)

# Установка файла конфигурации
install(FILES ks-config.yaml DESTINATION /etc/)
