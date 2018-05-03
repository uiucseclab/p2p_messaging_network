#include "../main/global_data.h"
#include "../main/client_connection.h"
#include "../main/po_node.h"

#include <queue>
#include <map>
#include <mutex>
#include <thread>
#include <cstring>
#include <string>

#include <iostream>

// Print out all messages pushed to server
void test_push(global_data &globals) {
	for (int i = 0; i < 100; i++) {
		sleep(5);
		std::lock_guard<std::mutex> lock(globals.out_lock);
		if (globals.outgoing_messages->size() == 0) {
			continue;
		}

		std::cout<< "Printing all messages in queue" << std::endl;

		for (auto it = globals.outgoing_messages->begin(); it != globals.outgoing_messages->end(); it++) {
			while (!it->second->empty()) {
				std::cout << it->first << " " << it->second->front() << std::endl;
				it->second->pop();
			}	
		}
	}
}

// Fill message queue, see if client pulls them
void test_pull(global_data &globals) {
	std::queue<std::string> *queue0 = new std::queue<std::string>();
	queue0->push("This is the first message");
	queue0->push("This is the second message");

	std::queue<std::string> *queue1 = new std::queue<std::string>();
	queue1->push("Wazzzzup");
	queue1->push("Hey jerry");

	std::queue<std::string> *queue2 = new std::queue<std::string>();

	std::cout<< "Attempting to acquire lock" << std::endl;
	{
		std::lock_guard<std::mutex> lock(globals.in_lock);
		globals.incoming_messages->insert(std::pair<std::string, std::queue<std::string>*>("1.1.1.1", queue0));
		globals.incoming_messages->insert(std::pair<std::string, std::queue<std::string>*>("2.2.2.2", queue1));
		globals.incoming_messages->insert(std::pair<std::string, std::queue<std::string>*>("3.3.3.3", queue2));
	}
	std::cout<< "All messages have been loaded in" << std::endl;

	bool read1 = false;
	bool read2 = false;

	while (1) {
		sleep(5);
		std::lock_guard<std::mutex> lock(globals.in_lock);
		if (globals.incoming_messages->at("1.1.1.1")->empty()) {
			read1 = true;
			std::cout << "All messages read from 1.1.1.1" << std::endl;
		}
		if (globals.incoming_messages->at("2.2.2.2")->empty()) {
			read2 = true;
			std::cout << "All messages read from 2.2.2.2" << std::endl;
		}
		if (read1 && read2) {
			break;
		}
	}

}

int main(int argc, char * argv[])
{
	global_data globals;
	globals.incoming_messages = new std::map<std::string,std::queue<std::string>*>();
	globals.outgoing_messages = new std::map<std::string,std::queue<std::string>*>();

	std::cout<<"Creating client thread"<<std::endl;
	std::thread client_conn(start_client_conn, &globals);
	client_conn.detach();

	if (std::strcmp(argv[1], "push") == 0) {
		std::cout<<"Testing push..."<<std::endl;
		test_push(globals);
	} else if (std::strcmp(argv[1], "pull") == 0) {
		std::cout<<"Testing pull..."<<std::endl;
		test_pull(globals);
	}

	return 0;
}