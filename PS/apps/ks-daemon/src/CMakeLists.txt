include_directories(${PROJECT_SOURCE_DIR}/inc)

add_library(main_o OBJECT daemon.cpp daemon-tool.cpp hw.cpp keysender-daemon.cpp logger.cpp keygen.cpp)
