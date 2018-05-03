
# Makefile for P2P Messaging Network Project Unit Test
# All executables will be stored in ./../../../bin/test/


### Flags used for compilation ###
CPPC=g++
CPPFLAGS=-Wall -fno-builtin -fno-stack-protector -std=c++17
BIN_DIR=./../../../bin/test/$@ $<


### client ###
CLIENT_TEST_SRC=./client_test.cpp
CLIENT_TEST_INC=./../main/socket.cpp ./../main/po_node.cpp ./../main/connection.cpp ./../main/client_connection.cpp

client_test: $(CLIENT_TEST_SRC)
	$(CPPC) $(CPPFLAGS) $(CLIENT_TEST_INC) -o $(BIN_DIR)


### server test ###
SERVER_TEST_SRC=./server_test.cpp
# $(wildcard ./src/main/*.cpp) < optional
SERVER_TEST_INC=./../main/socket.cpp ./../main/po_node.cpp ./../main/connection.cpp ./../main/client_connection.cpp

server_test: $(SERVER_TEST_SRC)
	$(CPPC) $(CPPFLAGS) $(SERVER_TEST_INC) -o $(BIN_DIR)


### client_server package ###
client_server_test: client_test server_test


### daemon test ###
DAEMON_TEST_SRC=./daemon_test.cpp
DAEMON_TEST_INC=./../main/daemonize.cpp ./../main/socket.cpp
daemon_test: $(DAEMON_TEST_SRC)
	$(CPPC) $(CPPFLAGS) $(DAEMON_TEST_INC) -o $(BIN_DIR)


### po_node_test ###
PO_NODE_TEST_SRC=./po_node_test.cpp
PO_NODE_TEST_INC=./../main/po_node.cpp ./../main/socket.cpp
po_node_test: $(PO_NODE_TEST_SRC)
	$(CPPC) $(CPPFLAGS) $(PO_NODE_TEST_INC) -o $(BIN_DIR)

CLIENT_CONN_TEST_SRC=./client_connection_test.cpp
CLIENT_CONN_TEST_INC=./../main/po_node.cpp ./../main/socket.cpp ./../main/client_connection.cpp ./../main/connection.cpp
client_conn_test: $(CLIENT_CONN_TEST_SRC)
	$(CPPC) $(CPPFLAGS) $(CLIENT_CONN_TEST_INC) -o $(BIN_DIR) -lpthread

### cleanup ###
.PHONY: clean
clean:
	rm -rf ./../../../bin/test/*


