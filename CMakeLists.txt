cmake_minimum_required(VERSION 3.20)
project(cpp_tryout)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES main.cpp)
add_executable(cpp_tryout ${SOURCE_FILES})

add_subdirectory(boost_interprocess)

add_subdirectory(boost_asio_mq)
