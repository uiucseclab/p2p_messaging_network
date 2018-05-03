# Peer-to-Peer Messaging Network
CS460
Jerry Chen (jchen238)
Tim Hostetler (thostet2)
Rohit Thotakura (thotaku2)

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

## Introduction
We will be using the word Server and Node interchangeably.
From the project proposal, the peer-to-peer messaging network is designed to be a medium for communication between any two users. We decided that this network isn’t intended to be a large scale network. Instead this network is better served as a messaging network internal to a organization. The organization will spawn some Bootstrap Nodes to prepare the network for Servers. The organization members can then join the network with their Servers at will.

![Relation Diagram](https://github.com/jerr-chen/p2p_messaging_network/blob/master/images/relation_diagram.PNG "Relation Diagram")

The users will host their own servers and connect them to the network. The users will also run a client interface that interacts with their local server. The local server will handle all server to server communications for the user. The diagram below will help illustrate the relations. When a user wants to send a message, the data will travel through volunteer nodes to reach the destination user.

There are several features that we wanted to incorporate into the the network, but due the number of times the design has been rewritten and the time constraints, we were only able to implement the basic idea of the project using simple protocols. To supplement our code, we would like to provide the fleshed out design of the peer-to-peer network and describe what we were able to implement.

## Design
The design goals of the peer-to-peer network can be broken down in different components:
    Local Client
    Local Server
    Bootstrap Server
    Network Structure
Message Structure
Communication Protocol
Joining/Leaving Network Protocol
Security Concerns

### Local Client
The Local Client is the interface that the user uses to interact with the network and the other users. The Client is implemented using Python and the interface uses the command line interface (CLI) to send messages through sockets to the Local Server. The user has the option to push (sending) messages to the server or pop (receiving) messages from the server. When pushing messages to the server, the user will also specify the ip of the user, which is the destination of the message they are pushing.

### Local Server
The Local Server serves two main purposes: sends messages pushed by the Local Client and receive messages sent by Neighbor Servers (the relation between Local Server and Neighbor Server will be detailed in the Network Structure component). When sending messages, it will be sent to all Neighbor servers hoping it reaches the intended recipient. When receiving messages, The Local Server will decide whether it is the intended recipient. If it is the intended recipient, it will read the message’s metadata in the header (explained in the Message Structure component) and decide whether it’s a standard message that needs to be stored for the client to pull or an instruction intended for the server to respond to. If Local Server isn’t the intended recipient, it will reroute the message to all its neighbors except for the sending neighbor.

### Bootstrap Server
The Bootstrap Server maintains a Distributed Hash Table, possibly implemented using Chord. The table holds the information regarding the network’s Servers. This information includes which nodes are listening for new connections, and how many connections each nodes have. More details of how this affects the network in the Joining/Leaving Network Protocol. This Server doesn’t transmit messages and strictly maintains the Network.

### Network Structure
The Network Structure consists of two types of Servers: Servers and Bootstrap Servers. The Servers are the Local Servers described above. The Bootstrap Server bootstraps new Servers joining the network, providing them with enough information to interact with other Servers in the network. Each Server can only maintain a maximum number of connections.. 

### Joining/Leaving Network Protocol
When joining the network, the Server upon startup, sends a message to the Bootstrap Server to request for neighbors to connect to. The Bootstrap Server will provide ips of neighbors that are listening for connections and update its table. The server will initiate a connection with the provided ips by greeting the neighbors and the two Servers will form a connection for message transmission. If the Server hasn’t reached maximum connections, it will start listening on port 58008 waiting for a connection. The Server shouldn’t need to inform the Bootstrap Server because it keeps track of the Server’s current connection count.
When leaving the network, the Server will signal the neighbor that it’s disconnecting, and it will then notify the Bootstrap Server to update. If the Bootstrap Server realizes that the disconnecting Server’s neighbor is left with no connections, it will signal it to connect with a new Server. The purpose of this is to ensure that the network doesn’t split into two components. This signal should be blocking. If the disconnecting Server’s neighbor has messages to transmit, it must reconnect with a new neighbor before it can continue transmission.

### Message Structure
All messages have a certain structure associated with them. The structure is composed of three things: the length of the remaining message, message code, and the actual message. The message code denotes what type of message is being sent/received. For example, a message code of 0001 denotes a greeting message to established a connection between two Servers, to signify that there is now an active connection. There are multiple message codes that will be used, from broadcast message codes that represent a message that is sent to the whole network, to normal messages from a Server to another Server. Lastly, the actual message is appended to the back of the message, which is what the sender wants to communicate to the receiver. 
An example message: 17|0002|Hello World!
The ‘|’ characters are dividers between the components. The “17” is a count of how many characters are in the message after the first divider, “0002” denotes a standard message, and “Hello World!” is the message being sent. We are not concerned with the need to sanitize what is in the message for fake headers/dividers because the header of the message is generated within the application and it always precedes the message.

### Communication Protocol
For a Node to send a message to another Node, there is a certain protocol. The sending Node will first select the IP of its receiver, and then proceed to send the message to all of its neighbors or connected Nodes. If none of these nodes are the receiver IP, they will continue to pass on the message to all of its neighbors as well. This method is known as a flood query as the message “floods” the entire network pool. Once the message is received by the IP it is destined for, there is another flood query response from the receiver that it has received the message and that any Node that owns or is about to pass on the message can drop it. 

The problem of a Node having to deal with multiple messages at a time is another problem in this communication protocol. Therefore, we have implemented a local queue for these Nodes that can hold multiple messages and process each one at a time. This way, there is no way a message can be lost or dropped by a server because it is handling a different message.

There are several problems with using flood queries. First, they are slow and bring along quite a bit of overhead. For these reasons, they are not scalable. You cannot use this method for large networks, but should still be fine for smaller networks. 

### Security Concerns
The biggest security concern in our implementation is confidentiality of the data passed from the sending Node to the receiving Node. As it stands, there is no encryption used to protect the messages being passed along the network. That means that any intermediary node that passes on the message can read the exact message that the original sender has sent. The obvious fix to this is to have the original sending node to encrypt the message before it is passed on the the rest of the network in search for its receiver. However, there is overhead to this as there would have to be some key-sharing process that takes place between the sending and receiving Nodes before the messages are sent. This would introduce a more complex handshake. Another way to solve this is to agree on a encryption key outside of the network, like how a lot of similar networks do it.

Another security concern is the integrity of the messages. As of right now, any intermediary Node can modify a message that it needs to pass along. This can be harmful because the receiver of the message can get something completely different than what was originally sent from the sender. To fix this, the same thing can be done as mentioned before. We would need to have the sender and receiver establish some sort of hash of the message for example on an off network process. 

## What We Learned - Conclusion
Writing decentralized systems are hard. The latest motivation for so many decentralized and distributed systems comes from a general lack of trust in larger corporations. Open source clients don’t mean anything if we don’t know what corporations do with the data we send them. But there was a reason the first type of architecture was around centralized systems. They are generally more efficient and easier to write. What we wrote works for 3 computers talking to each other. Much greater leaps would need to be taken in order to make it work for millions of people in a network.

What we actually implemented is a way to communicate with up to 3 machines at once. If you know their IPs, you can establish a connection with them and send messages through the python client. We were unable to implement message forwarding or dynamically adding or removing nodes into the network. A message will go from local client to local server to neighbor server to neighbor client. All messages are held in a global queue in the server until the client asks for them. There, the messages are popped off the queue and the client prints out where the message came from and the contents of the message.
