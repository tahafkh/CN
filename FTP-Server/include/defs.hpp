#ifndef __DEFS_HPP__
#define __DEFS_HPP__

#define MAX_BUFFER_SIZE 1024
#define CONFIG_FILE "config.json"

// Port definitions
#define COMMAND_PORT "commandChannelPort"
#define DATA_PORT "dataChannelPort"

// IP constants
#define IP_PROTOCOL 0 
#define IP_ADDRESS "127.0.0.1"

// Response codes
#define SUCCESS_USERNAME_FINDING "331: User name okay, need password."
#define LOGIN_SUCCESS "230: User logged in, proceed. Logged out if appropriate"
#define LOGOUT_SUCCESS "221: Successful Quit."
#define CHANGE_SUCCESS "250: Successful Change."
#define LIST_TRANSFER_SUCCESS "226: List transfer done."
#define DOWNLOAD_SUCCESS "226: Successful Download."

// Exception codes
#define USER_NOT_LOGGIN "332: Need account for login."
#define WRITING_ERROR "501: Syntax error in parameters or arguments."
#define DEFAULT_ERROR "500: Error"
#define BAD_SEQUENCE "503: Bad sequence of commands"
#define FULL_CAPACITY "425: Can't open data connection."
#define USER_NOT_FOUND "430: Invalid username or password"
#define ILLEGAL_ACCESS "550: File unavailable."

// Failure errors
#define SOCKET_CREATION_FAILED "Socket creation failed."
#define ADDRESS_FAILED "Invalid address."
#define CONNECTION_FAILED "Connection failed."
#define SET_SOCK_OPT__FAILED "Set sock opt failed."
#define BIND_FAILED "Bind failed."
#define LISTEN_FAILED "Listen failed."
#define ACCEPT_FAILED "Accept failed."
#define SEND_DATA_FAILED "Send data failed."
#define RECIVE_DATA_FAILED "Receive data failed."

#endif