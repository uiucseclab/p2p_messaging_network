/* server_test.cpp
 *	Test code for server socket.
 *	This test is paired with the client test.
 */

#include "../main/global_data.h"
#include "../main/socket.h"
#include "../main/po_node.h"
#include "../main/connection.h"

#include <stdio.h>
#include <unistd.h> // read()
#include <string.h>
#include <syslog.h>
#include <iostream>

int main(int argc, const char *argv[])
{
	global_data data;
	int new_socket, valread;

	char buffer[1024] = {0};
	char *hello = "Hello from server";

	openlog("server_test", LOG_PID, LOG_INFO);

	Socket s;
	s.setup_server_socket(SERVER_PORT);

	// Continually listen on the port and accept connections.
	// while(1)
	// {
		if(0 < (new_socket = s.accept_conn()))
		{
			std::cout << "Accepted connection" << std::endl;
		}
		std::string ip;
		start_neighbor_to_server_conn(ip, new_socket, &data);

		// valread = read(new_socket, buffer, 1024);
		// printf("%s\n", buffer);
		// send(new_socket, hello, strlen(hello), 0);
		// printf("Hello message sent\n");
	// }

	closelog();
	return 0;
}


