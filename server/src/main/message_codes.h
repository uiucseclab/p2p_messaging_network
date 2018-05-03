#ifndef MESSAGE_CODES_H_
#define MESSAGE_CODES_H_

// Each code contains a fixed number of alphanumeric characters
#define CODE_LEN 4
#define CODE_MSG_DIVIDER '|'

// Operation Status
#define SUCCESS "FFFE"
#define FAIL "FFFF"


// Server Connection Operations
#define INIT_CODE "0001" // initiate new connections, followed by ip
#define STD_CODE "0002" // standard message


// Client Connection Operations
#define CLIENT_PUSH "PUSH" // Initiated by client to write new messages to the message queue
#define CLIENT_PULL "PULL" // Initiated by client to retrieve the queue of messages for the client





#endif
