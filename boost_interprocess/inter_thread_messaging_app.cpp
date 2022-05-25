//
// Created by supreeth on 08.05.22.
//
#include <iostream>
#include <string>
#include <vector>

#include <boost/interprocess/ipc/message_queue.hpp>
#include <thread>

#define MAX_MSG_SIZE 1280
#define MAX_MSG_NUMBER 100

using namespace boost::interprocess;

auto Sender() -> int {
  try {
	message_queue s_to_r(open_or_create, "sender_to_receiver_queue",
						 MAX_MSG_NUMBER, MAX_MSG_SIZE);
	message_queue r_to_s(open_or_create, "receiver_to_sender_queue",
						 MAX_MSG_NUMBER, MAX_MSG_SIZE);

	std::string final_msg{};

	for (int i = 0; i < 20; i++) {
	  std::string msg = "Hello World from Sender " + std::to_string(i) + "\n";
	  s_to_r.send(msg.c_str(), msg.size(), 0);
	  //mq.try_send(msg.c_str(), msg.size() + 1, 0);
	}

	for (int i = 0; i < 20; i++) {
	  std::string received_msg{};
	  received_msg.resize(MAX_MSG_SIZE);
	  size_t recvd_size{};
	  unsigned int priority{};
	  r_to_s.receive(received_msg.data(), received_msg.capacity(), recvd_size, priority);
	  //r_to_s.try_receive(received_msg.data(), received_msg.capacity(),
	  //                   recvd_size, priority);
	  received_msg.resize(recvd_size);

	  final_msg.append(received_msg);
	}
	std::cout << "Total received message at Sender:\n" << final_msg << std::endl;
  } catch (interprocess_exception &ex) {
	std::cout << ex.what() << std::endl;
	message_queue::remove("sender_to_receiver_queue");
	message_queue::remove("receiver_to_sender_queue");
	return 1;
  }
  return 0;
}

auto Receiver() -> int {
  try {
	message_queue s_to_r(open_or_create, "sender_to_receiver_queue",
						 MAX_MSG_NUMBER, MAX_MSG_SIZE);
	message_queue r_to_s(open_or_create, "receiver_to_sender_queue",
						 MAX_MSG_NUMBER, MAX_MSG_SIZE);

	std::string final_msg{};

	for (int i = 0; i < 20; i++) {
	  std::string msg = "Hello World from Receiver " + std::to_string(i) + "\n";
	  r_to_s.send(msg.c_str(), msg.size(), 0);
	}

	for (int i = 0; i < 20; i++) {
	  std::string received_msg{};
	  received_msg.resize(MAX_MSG_SIZE);
	  size_t recvd_size{};
	  unsigned int priority{};
	  s_to_r.receive(received_msg.data(), received_msg.capacity(), recvd_size, priority);
	  //s_to_r.try_receive(received_msg.data(), received_msg.capacity(),
	  //                   recvd_size, priority);
	  received_msg.resize(recvd_size);

	  final_msg.append(received_msg);
	}
	std::cout << "Total received message at Receiver:\n" << final_msg << std::endl;
  } catch (interprocess_exception &ex) {
	std::cout << ex.what() << std::endl;
	message_queue::remove("sender_to_receiver_queue");
	message_queue::remove("receiver_to_sender_queue");
	return 1;
  }
  return 0;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  message_queue::remove("sender_to_receiver_queue");
  message_queue::remove("receiver_to_sender_queue");

  // Launch two threads, one for each message queue
  std::thread t1(Sender);
  std::thread t2(Receiver);

  t1.join();
  t2.join();

  message_queue::remove("sender_to_receiver_queue");
  message_queue::remove("receiver_to_sender_queue");

  return 0;
}
