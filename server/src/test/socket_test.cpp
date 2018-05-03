/* socket_test.cpp
 *	Test code for server socket.
 *	This test is paired with the client test.
 */

#include "../main/socket.h"

#include <stdio.h>
#include <unistd.h> // read()
#include <string.h>
#include <syslog.h>

int main()
{
	int new_socket;

	char buffer[2048] = {0};

	openlog("server_test", LOG_PID, LOG_INFO);

	Socket s;
	s.setup_server_socket(31337);

	// Continually listen on the port and accept connections.
	while(1)
	{
		new_socket = s.accept_conn();
		read(new_socket, buffer, 1024);
		printf("%s\n", buffer);
		send(new_socket, "message received", 17, 0);
		printf("Hello message sent\n");
	}

	closelog();
	return 0;
}