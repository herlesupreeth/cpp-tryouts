#include <ctime>
#include <iostream>
#include <string>
#include <future>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

std::string make_daytime_string() {
  using namespace std; // For time_t, time and ctime;
  time_t now = time(nullptr);
  return ctime(&now);
}

class udp_server {
 public:
  udp_server(boost::asio::io_context &io_context)
	  : socket_(io_context, udp::endpoint(udp::v4(), 13000)) {
	recv_buffer_.resize(1024);
	start(io_context);
  }

 private:
  void start(boost::asio::io_context &io_context) {
	socket_.async_receive_from(
		boost::asio::buffer(recv_buffer_.data(), 1024), remote_endpoint_,
		[this](boost::system::error_code error, std::size_t bytes_transferred) {
		  handle_receive(error, bytes_transferred);
		});

	io_context.post([this, &io_context]() {
	  another_task(io_context);
	});
  }

  void another_task(boost::asio::io_context &io_context) {
	auto start = std::chrono::high_resolution_clock::now();
	auto end = start + std::chrono::seconds{3};
	do {
	  std::this_thread::yield();
	} while (std::chrono::high_resolution_clock::now() < end);

	std::cout << "Another Task thread id: " << std::this_thread::get_id() << std::endl;

	io_context.post([this, &io_context]() {
	  another_task(io_context);
	});
  }

  void handle_receive(const boost::system::error_code &error, std::size_t bytes_transferred) {
	if (!error) {
	  if (bytes_transferred > 0) {
		recv_buffer_.resize(bytes_transferred);

		std::cout << "Handle receive thread id: " << std::this_thread::get_id() << "\n"
				  << "Received (S): " << recv_buffer_ << std::endl;

		sleep(3);

		boost::shared_ptr<std::string> message(
			new std::string("Server Timestamp - " + make_daytime_string()));

		socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
							  boost::bind(&udp_server::handle_send, this, message,
										  boost::asio::placeholders::error,
										  boost::asio::placeholders::bytes_transferred));

	  }
	  // Receive again
	  recv_buffer_.resize(1024);
	  socket_.async_receive_from(
		  boost::asio::buffer(recv_buffer_.data(), 1024), remote_endpoint_,
		  [this](boost::system::error_code error, std::size_t bytes_transferred) {
			handle_receive(error, bytes_transferred);
		  });
	}
  }

  void handle_send(const boost::shared_ptr<std::string> & /*message*/,
				   const boost::system::error_code & /*error*/,
				   std::size_t /*bytes_transferred*/) {
  }

  udp::socket socket_;
  udp::endpoint remote_endpoint_{};
  std::string recv_buffer_{};
};

int main() {
  try {
	std::cout << "Starting server ..." << std::endl;

	boost::asio::io_context io_context;
	udp_server server(io_context);
	io_context.run();

	std::cout << "Closing server ..." << std::endl;
  } catch (std::exception &e) {
	std::cerr << e.what() << std::endl;
  }

  return 0;
}
