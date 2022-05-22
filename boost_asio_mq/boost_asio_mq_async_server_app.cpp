#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(nullptr);
    return ctime(&now);
}

class udp_client
{
public:
    explicit udp_client(boost::asio::io_context& io_context)
            : socket_(io_context, udp::endpoint(udp::v4(), 13000))
    {
        start_receive();
    }

private:
    void start_receive()
    {
        socket_.async_receive_from(
                boost::asio::buffer(recv_buffer_), remote_endpoint_,
                [this] (const boost::system::error_code& error,
                        std::size_t bytes_received) { handle_receive(error, bytes_received); });
    }

    void handle_receive(const boost::system::error_code& error,
            std::size_t /*bytes_transferred*/)
    {
        if (!error)
        {
            boost::shared_ptr<std::string> message(
                    new std::string(make_daytime_string()));

            socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
                    boost::bind(&udp_client::handle_send, this, message,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));

            start_receive();
        }
    }

    void handle_send(const boost::shared_ptr<std::string>& /*message*/,
            const boost::system::error_code& /*error*/,
            std::size_t /*bytes_transferred*/)
    {
    }

    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    boost::array<char, 1> recv_buffer_{};
};

int main()
{
    try
    {
        boost::asio::io_context io_context;
        udp_client server(io_context);
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
