#include <stdlib.h>
#include <stdio.h>
#include "global_data.h"
#include "socket.h"
#include "po_node.h"


int main(int argc, char * argv[]) {
	if (argc > 4)
		exit(EXIT_FAILURE);

	int num_init_connections = argc - 1;

	global_data globals;
	globals.incoming_messages = new std::map<std::string,std::queue<std::string>*>();
	globals.outgoing_messages = new std::map<std::string,std::queue<std::string>*>();
	globals.num_connections = 0;

	// Create client connection
	std::thread client_conn(start_client_conn, &globals);
	client_conn.detach();
	
	// Create node connections
	for (int i = 1; i <= num_init_connections; i++) {
		std::string ip(argv[i]);
		std::thread new_conn(start_server_to_neighbor_conn, ip, &globals);
		new_conn.detach();
	}

	// create server socket
	printf("Listening for more connections...\n");
	Socket server_listener;
	server_listener.setup_server_socket(SERVER_PORT);
	while (1) {
		int sock_fd = -1;
		sock_fd = server_listener.accept_conn();
		std::string ip(server_listener.get_most_recent_ip());
		std::thread new_conn(start_neighbor_to_server_conn, ip, sock_fd, &globals);
		new_conn.detach();
	}
}
