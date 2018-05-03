#ifndef CLIENT_CONNECTION_H_
#define CLIENT_CONNECTION_H_

#include "message_codes.h"
#include "global_data.h"
#include <iostream>
#include <unistd.h>
#include <sstream>

class client_connection {
	private:
		global_data * data;

		bool process_push(std::string message);
		void process_pull(int sock_fd);
	public:
		client_connection(global_data * data);

		void process_request(int sock_fd, std::string request);	
};

#endif