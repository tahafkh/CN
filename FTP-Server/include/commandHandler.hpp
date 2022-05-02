#ifndef __COMMAND_HANDLER_HPP__
#define __COMMAND_HANDLER_HPP__

#include "headers.hpp"
#include "database.hpp"
#include "logger.hpp"
#include "defs.hpp"

class CommandHandler {
private:
    DataBase* database;
    Logger* logger;
    int data_socket;
    struct sockaddr_in data_addr;
    std::vector <std::string> input_words;
    User* user;

    void tokenize_input(std::string input);
    std::string build_path(std::string input_path);

    std::string mkd_command(int client_fd);
    std::string dele_command(User* user);
    void ls_command(int client_fd);
    void cwd_command(User* user);
    void rename_command(User* user);
    void retr_command(int client_fd);
    void user_command(int client_fd);
    void pass_command(int client_fd);
    std::string help_command(int client_fd);
    void quit_command(int client_fd);

public:
    CommandHandler(DataBase* _data_base, Logger* _logger);
    ~CommandHandler();
    std::string run_command_handler(std::string input, int client_fd);
    std::string handle_command(int client_fd);
    int create_data_connection(int fd);
};


#endif