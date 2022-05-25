#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

std::string make_daytime_string() {
  using namespace std; // For time_t, time and ctime;
  time_t now = time(nullptr);
  return ctime(&now);
}

class udp_client {
 public:
  explicit udp_client(boost::asio::io_context &io_context)
	  : socket_(io_context) {
	udp::resolver resolver(io_context);
	udp::resolver::query query(udp::v4(), std::to_string(13000));
	udp::resolver::iterator itr = resolver.resolve(query);
	server_endpoint_ = *itr;

	socket_.connect(server_endpoint_);

	std::cout << "Client: IP: " << socket_.local_endpoint().address()
			  << " Port: " << socket_.local_endpoint().port() << std::endl;

	boost::shared_ptr<std::string> message(
		new std::string("From client:" + make_daytime_string()));

	socket_.send_to(boost::asio::buffer(*message), server_endpoint_);

	start_receive();
  }

 private:
  void start_receive() {
	recv_buffer_.reserve(1024);
	socket_.async_receive_from(
		boost::asio::buffer(recv_buffer_), remote_endpoint_,
		boost::bind(&udp_client::handle_receive, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
  }

  void handle_receive(const boost::system::error_code &error, std::size_t bytes_transferred) {
	if (!error) {
	  if (bytes_transferred > 0) {
		recv_buffer_.resize(bytes_transferred);
		std::cout << "Received (S): " << recv_buffer_ << std::endl;

		boost::shared_ptr<std::string> message(
			new std::string("From server:" + make_daytime_string()));

		socket_.async_send_to(boost::asio::buffer(*message), server_endpoint_,
							  boost::bind(&udp_client::handle_send, this, message,
										  boost::asio::placeholders::error,
										  boost::asio::placeholders::bytes_transferred));
	  }
	  start_receive();
	}
  }

  void handle_send(const boost::shared_ptr<std::string> & /*message*/,
				   const boost::system::error_code & /*error*/,
				   std::size_t /*bytes_transferred*/) {
  }

  udp::socket socket_;
  udp::endpoint remote_endpoint_{};
  udp::endpoint server_endpoint_{};
  std::string recv_buffer_{};
};

int main() {
  try {
	std::cout << "Starting client ..." << std::endl;

	boost::asio::io_context io_context;
	udp_client client(io_context);
	io_context.run();

	std::cout << "Closing client ..." << std::endl;
  } catch (std::exception &e) {
	std::cerr << e.what() << std::endl;
  }

  return 0;
}
