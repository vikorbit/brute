cmake_minimum_required(VERSION 2.6)
project(brute)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

add_subdirectory(tests)

find_package(Threads REQUIRED)

include_directories(
    src
)

file(GLOB src
    src/brute.cpp
    src/parse_command_line.cpp
    src/worker_pool.cpp
    src/words_bucket.cpp
    src/md5.cpp
)

add_executable(${PROJECT_NAME} ${src})

target_link_libraries(${PROJECT_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
)
