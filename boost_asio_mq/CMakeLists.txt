set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer -O0")

add_executable(boost_asio_mq_async_server_app boost_asio_mq_async_server_app.cpp)
add_executable(boost_asio_sync_client_app boost_asio_async_client_app.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(boost_asio_mq_async_server_app Threads::Threads)
target_link_libraries(boost_asio_sync_client_app Threads::Threads)
