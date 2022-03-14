#ifndef __SERVER_HPP__
#define __SERVER_HPP__ 

#include "headers.hpp"
#include "database.hpp"
#include "commandHandler.hpp"
#include "logger.hpp"
#include "defs.hpp"

class DataBase;
class Logger;
class CommandHandler;

class Server {
private:
    int command_socket;
    struct sockaddr_in command_addr; 
    
    DataBase* database;
    Logger* logger;
    CommandHandler* command_handler;

public:
    Server();
    ~Server();
    void initialize_socket();
    void run();
};

#endif