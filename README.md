# p2p_messaging_network

If you are looking for the final project report, read REPORT.md instead

## How-To-Use
Run ```make``` in the root directory of the project to build the standard application. 
The node can be started by running ```bin/main/p2p```
If this node is the first node to be started on the network, the above command will set it up fine.
If there are existing nodes on the network already listening, as command line arguments pass up to 3 IP addresses of the nodes you want to connect to

Example
The following will connect to nodes together:
On machine 1 with ip address 192.168.1.10 at time t=0:
```bin/main/p2p```

On machine 2 at time t > 0:
```bin/main/p2p 192.168.1.10```

To actually send messages run:
```python client/main/client.py PUSH --to IP_ADDRESS --msg MESSAGE```

To read your new messages run:
```python client/main/client.py PULL```

## Project Proposal
We intend to work on a peer to peer messaging network. Every user will run their own
instance of an identical node, where they can send and receive messages. The messages will
be encrypted and will take a different path from the source node through the nodes to the target
node every iteration. The intermediary nodes, the message will travel along, will consist of
users, who volunteer to act as intermediaries. The goal of this is to create a secure
decentralised system for sending and transmitting messages via nodes.

We do not intend to create a GUI for this project because this application will be
interfaced with the command line. We intend to use C++ to build this, but we will revisit this
decision as needed when we begin implementing the decentralized messaging system. If we
have time, we may explore the feature to break up the messages into shards and reconstruct
the shards at the target node for added security. Other features and necessities will be created
throughout the process.

This project will take at least 40 hours for each person due to the complexity of creating
a peer to peer environment. There are multiple complicated components in making this type of
messaging system where each part has their own caveats. At the same time, testing will
become more complicated as we will require several active intermediary nodes during testing.

## Architecture Concerns

2 separate client and thread processes instead of a client and server thread on a single process. Mainly because we want to the server to run as a daemon.
If we were to do one process with client/server thread, then we will need an active terminal, and that would allow the user to spawn a new client through a keystroke, but I don't we want to do that.

Group the conversations based on who they are with to prevent duplicate conversations with the same client.

Server will communicate with other server and the server will save whatever data it needs to into memory. When client makes a request to a server it will provide the data to the client after it retrieves it from its memory.

What happens when a node shuts down? We can't expect the node's computer to always be active.
	Path saving? Maintaining a set amount of nearby nodes. If a node shuts down, what happens if that's a node being used as intermediary node between two clients?
	If a node drops, it should tell it's adjacent nodes to connect with each other to preserve the path. (IDEA)

Node joining network overhead.
	Have to know at least one ip in the network before being able to join the network?

Don't want every node connecting to every other node.

Thread pool of server connections (max number of servers and minimum to join the network)
	If every node in the network is connected to maximum servers, how will a new node join?

Message codes - Logistical/meta messages, normal messages (How do we keep track of this? This is more of a network information)

Drop List when receiving a broadcast of messages received. (figure out when to flush droplist) This is to drop messages that are still propagating after being received by the client. Maintain a healthy network

### Client to Node Protocol
Node sets up a server socket that accepts connections from the local client. All interactions are handled with a simple client request and server response. The socket connection is immediately terminated afterwards.

The client prefaces its request with either PUSH or PULL. 
PUSH writes a single message to the specified IP. The client request is formatted as such:
	PUSH IP:<DESTINATION-IP> MSG:<message contents>
The server response is formatted as one of the two following:
	SUCCESS

	FAIL

PULL retrieves all messages from the server back to the client The client request is formatted as such:
	PULL
The server response is formatted as such:
	FAIL # Connection is immediately terminated

	SUCCESS <number of messages> # can be 0
	IP: <SRC-IP> MSG:<message 0>
	IP: <SRC-IP> MSG:<message 1>
	IP: <SRC-IP> MSG:<message n> # connection is terminated after this send
The client must make successive reads on the socket to get all of its messages


#### Future client node considerations
* Have client be able to submit multiple messages
* Have client be able to pull messages from a given timeframe
* Have client be able to write a message of unlimited length
* PRIORITY: Verify the request came from localhost

### Node Design


### Communication Protocol
