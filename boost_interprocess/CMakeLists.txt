set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer -O0")

add_executable(inter_thread_messaging_app inter_thread_messaging_app.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(inter_thread_messaging_app rt Threads::Threads)
