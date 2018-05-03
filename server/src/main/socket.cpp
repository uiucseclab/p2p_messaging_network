/* socket.cpp
 *
 *
 *
 */

#include "socket.h"

// standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
// system logging
#include <string.h>
#include <syslog.h>
#include <errno.h>

// project libraries


/* public
 * @purpose:
 * 
 * @param:
 *
 * @return:	
 *
 */
Socket::Socket()
{
	memset(&(this->address), '0', sizeof(this->address));
	this->port = -1;
	this->socket_fd = -1;
	// TODO: Consider other max backlog values
	this->max_queued_conn = 5;
}

int Socket::get_socket_fd()
{
	return socket_fd;
}

char* Socket::get_most_recent_ip()
{
	return inet_ntoa(address.sin_addr);
}

/* public
 * @purpose:
 * 		This method is used by server sockets to setup the socket
 *		with proper configurations and a specific port to listen on.
 *		This is a wrapper that handles all the overhead prior to
 *		accepting a connection from a client.
 * @param:
 *		port | specified port to bind server socket
 * @return:	status of operation
 *		true: properly executed operation
 *		false: unable to setup the socket
 */
/* Server TCP socket methods */
bool Socket::setup_server_socket(int port)
{
	bool success = false;
	this->port = port;
	if(create())
	{
		if(attach(port))
		{
			if(listen_port())
			{
				success = true;
			}
		}
	}
	return success;
}

/* public
 * @purpose:
 * 		This method is used by client sockets to setup the socket
 *		with proper configurations and a specified port and ip to
 *		connect to a server that is listening on the port.
 * @param:
 *		port | specified port server is listening on
 *		ip | specified ip of the server
 * @return:	socket file descriptor
 *		< 0: Unable to setup client socket and connect
 *		> 0: Valid socket fd for use.
 */
int Socket::setup_client_socket(int port, const char* ip)
{
	this->port = port;
	if(create())
	{
		if(configure_address(port, ip))
		{
			if(connect_server())
			{
				return this->socket_fd;				
			}
		}
	}
	return -1;
}

/* public
 * @purpose:
 * 		This method is used by client sockets to attempt to connect
 *		to the configured address.
 * @param:
 *		None
 * @return: status of operation
 *		true: properly completed operation
 *		false: unable to connect to target address
 */
bool Socket::connect_server()
{
	if(connect(this->socket_fd, (struct sockaddr *)&(this->address), sizeof(this->address)) < 0)
	{
		syslog(LOG_WARNING, "[socket] Failed to connect to server.");
		return false;
	}
	return true;
}

/* public
 * @purpose:
 * 		This method is used by client sockets to configure
 *		the socket to connect to a specific ip that is listening
 *		on the specified port.
 * @param:
 *		port | specified port to connect to
 *		ip | specified ip to connect to
 * @return: status of operation	
 *		true: properly completed operation
 *		false: unable to configure target address
 */
bool Socket::configure_address(int port, const char* ip)
{
	(this->address).sin_family = AF_INET;
	(this->address).sin_port = htons(port);
	if(inet_pton(AF_INET, ip, &(this->address).sin_addr) <= 0)
	{
		syslog(LOG_ERR, "[socket] Can't configure. Invalid address or address not supported: %s", strerror(errno));
		return false;
	}
	return true;
}

/* public
 * @purpose:
 *		This method is used by server and client sockets. Generates
 *		a file descriptor stored in the object. 
 * @param:
 *		None
 * @return:	status of operation
 *		true: properly completed operation
 *		system exit: unable to create socket file descriptor
 */
bool Socket::create()
{
	// create socket file descriptor
	this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(this->socket_fd == 0)
	{
		syslog(LOG_ERR, "[socket] Failed to create a socket: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	return true;
}

/* public
 * @purpose:
 * 		This method is used by server sockets that are binded
 *		to a port. Sets the socket option/flags and attaches the
 *		object to a specified port. Socket is currently set to
 *		accept connections on the specified port from any address.
 * @param:
 *		port | port to attach to.
 * @return:	status of operation
 *		true: properly completed operation
 *		false: failed to attach to port
 *		system exit: unable to set socket operations
 */
bool Socket::attach(int port)
{
	int opt = 1;
	int ret_check;

	// setting socket options
	ret_check = setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	if(ret_check)
	{
		syslog(LOG_ERR, "[socket] Failed to set socket options: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// attach socket to port
	(this->address).sin_family = AF_INET;
	(this->address).sin_addr.s_addr = INADDR_ANY;
	(this->address).sin_port = htons(port);
	ret_check = bind(this->socket_fd, (struct sockaddr*) &(this->address), sizeof(address));
	if(ret_check < 0)
	{
		// Failure to bind to port should return with 0 instead of exit.
		syslog(LOG_ERR, "[socket] Failed to bind to port %d: %s", port, strerror(errno));
		return false;
	}

	return true;
}

/* public
 * @purpose:
 * 		This method is used by server sockets that are binded
 *		to a port. Listens on a port readying itself to accept
 *		connections.
 * @param:
 *		None
 * @return:	status of operation
 *		true: properly completed operation
 *		system exit: unable to listen
 */
bool Socket::listen_port()
{
	int ret_check;

	ret_check = listen(this->socket_fd, this->max_queued_conn);
	if(ret_check < 0)
	{
		syslog(LOG_ERR, "[socket] Failed to listen on port %d: %s", this->port, strerror(errno));
		exit(EXIT_FAILURE);
	}

	return true;
}

/* public
 * @purpose:
 * 		This method is used by server sockets that are binded
 *		to a port. Accepts a client's request to connect and 
 *		produces a socket file descriptor that will is used to
 *		continue communications and return messages.
 * @param:
 *		None
 * @return: socket file descriptor
 *		< 0: Unable to accept connection
 *		> 0: Valid socket fd for use.
 */
int Socket::accept_conn()
{
	int sock, addrlen;

	// accept incoming connection
	addrlen = sizeof(this->address);
	sock = accept(this->socket_fd, (struct sockaddr*) &(this->address), (socklen_t*) &addrlen);
	if(sock < 0)
	{
		syslog(LOG_WARNING, "[socket] Failed to accept connection: %s", strerror(errno));
	}

	return sock;
}
