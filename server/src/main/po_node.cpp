#include "po_node.h"

// standard libraries
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
// system logging
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <iostream>

// project libraries
#include "global_data.h"
#include "connection.h"
#include "socket.h"
#include "client_connection.h"

/* 
 * @purpose: server <- client
 * 		Main function for a neighbor to server connection thread.
 *		This is used when this node is acting a server to a neighbor client.
 * @param:
 *
 * @return:	
 *		
 */
void start_neighbor_to_server_conn(std::string ip, int sock_fd, global_data* data)
{
	std::cout<<"Starting connection thread with " << ip << std::endl;
	// With this implementation, its possible for a connection to be accepted followed by being immediately terminated
	{
		std::lock_guard<std::mutex> lock(data->conn_lock);
		if (data->num_connections >= 3) {
			close(sock_fd);
			return;
		}
		data->num_connections++;
	}
	Connection *stream = new Connection(sock_fd, ip, data);

	std::cout<<"Initializing all threads with " << ip << std::endl;
	std::thread handle_thread = std::thread(&Connection::handle_message, stream);
	std::thread recv_thread = std::thread(&Connection::receive_message, stream);
	std::thread send_thread = std::thread(&Connection::send_message, stream);

	handle_thread.join();
	recv_thread.join();
	send_thread.join();

	close(sock_fd);
	{
		std::lock_guard<std::mutex> lock(data->conn_lock);
		data->num_connections--;
	}
	return;
}

/* 
 * @purpose: client -> server
 * 		Main function for a server to neighbor connection thread.
 *		This is used when this node is acting as a client to a neighbor server
 * @param:
 *
 * @return:	
 *		
 */
 bool start_server_to_neighbor_conn(std::string ip, global_data* data)
 {
	std::cout<<"Starting connection thread with " << ip << std::endl;
	{
		std::lock_guard<std::mutex> lock(data->conn_lock);
		if (data->num_connections >= 3) {
			return false;
		}
		data->num_connections++;
	}

	std::cout<<"Attempting to connect with " << ip << std::endl;
 	int sock_fd;
 	Socket client_sock;
 	if((sock_fd = client_sock.setup_client_socket(SERVER_PORT, ip.c_str())) < 0)
 	{	// unable to create client
 		syslog(LOG_NOTICE, "[po_node] Unable to connect to the neighbor ip (%s)", ip.c_str());
 		return false;
 	}

 	Connection* stream = new Connection(sock_fd, ip, data);

	std::cout<<"Greeting " << ip << std::endl;
 	// send initialization message
 	stream->greet_neighbor();

	std::thread handle_thread = std::thread(&Connection::handle_message, stream);
	std::thread recv_thread = std::thread(&Connection::receive_message, stream);
	std::thread send_thread = std::thread(&Connection::send_message, stream);

	handle_thread.join();
	recv_thread.join();
	send_thread.join();

 	close(sock_fd);
	{
		std::lock_guard<std::mutex> lock(data->conn_lock);
		data->num_connections--;
	}
 	return true;
}

void start_client_conn(global_data * data)
{
	int sock_fd;
	char buf[2048];

	client_connection connection = client_connection(data);
	Socket client_listener;
	client_listener.setup_server_socket(CLIENT_PORT);
	while (1) {
		if ((sock_fd = client_listener.accept_conn()) < 0) {
			continue;
		}

		int request_length = read(sock_fd, buf, 2048);
		if (request_length == 0) {
			printf("Nothing has been read\n");
			write(sock_fd, "FAIL", 4);
		} else {
			std::string request = std::string(buf).substr(0, request_length);
			connection.process_request(sock_fd, request);
		}

		close(sock_fd);
	}
}
