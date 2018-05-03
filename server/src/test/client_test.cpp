// Client side C/C++ program to demonstrate Socket programming

#include "../main/global_data.h"
#include "../main/socket.h"
#include "../main/po_node.h"
#include "../main/connection.h"

#include <queue>
#include <map>
#include <string>
#include <thread>
#include <mutex>

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <iostream>
 
int main(int argc, const char *argv[])
{
    global_data thing; //= new global_data();
    std::string ip = "127.0.0.1";
    start_server_to_neighbor_conn(ip, &thing);


    // int sock, valread;
    // char *hello = "Hello from client";
    // char buffer[1024] = {0};

    // Socket s;
    // sock = s.setup_client_socket(31337, "127.0.0.1");
    // if(sock < 0)
    // {
    //     std::cout << "Failed" << std::endl;
    // }

    // send(sock , hello , strlen(hello), 0);
    // printf("Hello message sent\n");
    // valread = read( sock , buffer, 1024);
    // printf("%s\n",buffer );
    return 0;
}