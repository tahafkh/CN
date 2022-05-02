#ifndef __DATABASE_HPP__
#define __DATABASE_HPP__ 

#include "headers.hpp"
#include "defs.hpp"
#include "user.hpp"

class User; 

class DataBase {

private:
    std::ifstream json_file;
    std::vector <User*> users;
    std::map <int, User*> fd_users;
    std::map <int, int> data_fds;
    std::vector <std::string> restricted_files;
    int command_port;
    int data_port;

    void read_file(std::string);
    void parse_json();

public:
    DataBase();
    ~DataBase();
    std::vector <User*> get_users();
    User* find_user(std::string username);
    User* find_user(int fd);
    void set_user_fd(int fd, User* user);
    void remove_user_fd(int fd);
    int get_data_fd(int command_fd);
    void set_data_fd(int command_fd, int data_fd);
    void remove_data_fd(int command_fd);
    bool is_restricted(std::string fname);
    int get_command_port();
    int get_data_port();
};

#endif 