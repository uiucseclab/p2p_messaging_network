/* daemon_test.cpp
 *	This is an example from stackoverflow to daemonize a process,
 *	writes a few log messages, sleeps 20 seconds, and terminates
 *	afterwards.
 */

#include "../main/daemonize.h"
// special daemonized server includes
#include "../main/socket.h"

#include <string.h>
// end special daemonized server includes

#include <syslog.h>
#include <stdlib.h>
#include <unistd.h>

void daemon_server()
{
	int new_socket, valread;

	char buffer[1024] = {0};
	char *hello = "Hello from server";

	Socket s;
	s.setup_server_socket(31337);

	new_socket = s.accept_conn();
	valread = read(new_socket,buffer, 1024);
	syslog(LOG_INFO, "[daemon_test] received message: %s", buffer);
	send(new_socket, hello, strlen(hello), 0);
	syslog(LOG_INFO, "[daemon_test] Hello message sent.");

	return;
}

int main()
{
	setup_daemon();
	while(1)
	{
		syslog(LOG_NOTICE, "[daemon_test] First daemon started.");

		//TODO: insert daemon code here
		//daemon_server();

		sleep(20);
		break;
	}

	syslog(LOG_NOTICE, "[daemon_test] First daemon terminated.");
	teardown_daemon();

	return EXIT_SUCCESS;
}
