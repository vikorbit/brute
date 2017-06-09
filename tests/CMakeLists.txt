cmake_minimum_required(VERSION 2.6)
project(tests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

find_package(Threads REQUIRED)

include_directories(
    ../src
    thirdparty/catch/
)

file(GLOB src
	tests.cpp
    ../src/parse_command_line.cpp
    ../src/worker_pool.cpp
    ../src/words_bucket.cpp
    ../src/md5.cpp
)

add_executable(${PROJECT_NAME} ${src})

target_link_libraries(${PROJECT_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
)
