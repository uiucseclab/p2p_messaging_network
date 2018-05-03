#ifndef P2P_MAIN_CONNECTION_H_
#define P2P_MAIN_CONNECTION_H_

#include <string.h>

#include "global_data.h"

class Connection {

private:

    // Max buffer size indirectly determines max message length
    // This influence is based on how we receive messages.
    static const int MAX_BUF_LEN = 512;
    int fd; // file descriptor
    std::string ip; // ip address of other side of connection
    global_data * data; // pointer to global map of messages
	std::queue<std::string> * local_incoming_msg;
	std::queue<std::string> * local_outgoing_msg;

	std::mutex local_in;
	std::mutex local_out;

public:

    // constructor and destructor
    Connection(int fd, std::string ip, global_data * data);
    ~Connection();
    

    // methods to interface with messages
    bool greet_neighbor();
    void receive_message();
    void send_message();
    void handle_message();

};

#endif
