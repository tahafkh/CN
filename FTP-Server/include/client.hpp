#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include "defs.hpp"
#include "headers.hpp"

class Client {
    
    private:
        int command_socket;
        int data_socket;
        struct sockaddr_in command_addr; 
        struct sockaddr_in data_addr; 
        int command_port;
        int data_port;

        void handle_response(std::string responsee);
        void connect_to_server();
        void send_data_to_server(std::string data);
        std::string receive_response_from_server(int sock);
        void receive_file_from_server(std::string response);
        void read_ports();
    
    public:
        Client();
        ~Client();
        void run();
};

#endif