#include "client_connection.h"

client_connection::client_connection(global_data * data) {
	this->data = data;
}

bool client_connection::process_push(std::string message) {
	if (message.substr(0,3) != "IP:") {
		return false;
	}

	int msg_tag_start = message.find_first_of(" ", 3) + 1;
	if (msg_tag_start == 0 || message.substr(msg_tag_start, 4) != "MSG:") {
		return false;
	}

	std::string dest_ip = message.substr(3, msg_tag_start - 4);
	std::string message_contents = message.substr(msg_tag_start + 4);
	
	std::ostringstream stream;
	stream << (message_contents.length() + 5) << CODE_MSG_DIVIDER << STD_CODE << CODE_MSG_DIVIDER << message_contents;

	{
		std::lock_guard<std::mutex> lock(data->out_lock);
		try {
			data->outgoing_messages->at(dest_ip)->push(stream.str());
		} catch (std::out_of_range &e) {
			std::queue<std::string> *msg_queue = new std::queue<std::string>();
			msg_queue->push(message_contents);
			data->outgoing_messages->insert(std::pair<std::string, std::queue<std::string>*>(dest_ip, msg_queue));
		}
	}

	return true;
}

void client_connection::process_pull(int sock_fd) {
	int msg_start;
	std::queue<std::string> local_message_queue = std::queue<std::string>();

	{
		std::lock_guard<std::mutex> lock(data->in_lock);
		for(std::map<std::string,std::queue<std::string>*>::iterator it = data->incoming_messages->begin(); it != data->incoming_messages->end(); it++) {
			std::string msg;
			while (!it->second->empty()) {
				msg = it->second->front();
				msg_start = -1;
				for (int i = 0; i < 2; i++) {
					// Ignore message code for now
					msg_start = msg.find_first_of(CODE_MSG_DIVIDER, msg_start + 1);
				}

				it->second->pop();
				std::string resp_i = "IP: " + it->first + " MSG:" + msg.substr(msg_start + 1);
				local_message_queue.push(resp_i);
			}
		}
	}

	int count = local_message_queue.size();

	std::ostringstream string_stream;
	string_stream << "SUCCESS " << count;
	std::string success_str = string_stream.str();

	write(sock_fd, success_str.c_str(), success_str.length());
	while (!local_message_queue.empty()) {
		usleep(500000); // Half a second
		std::string response = local_message_queue.front();
		write(sock_fd, response.c_str(), response.length());
		local_message_queue.pop();
	}
}

void client_connection::process_request(int sock_fd, std::string request) {
	std::string operation = request.substr(0,4);
	if (operation == CLIENT_PUSH) {
		if (process_push(request.substr(5))) {
			write(sock_fd, "SUCCESS", 7);
		} else {
			write(sock_fd, "FAIL", 4);
		}
	} else if (operation == CLIENT_PULL) {
		process_pull(sock_fd);
	} else {
		write(sock_fd, "FAIL", 4);
	}
}