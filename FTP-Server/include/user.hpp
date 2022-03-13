#ifndef __USER_HPP__
#define __USER_HPP__ 

#include "headers.hpp"
#include "defs.hpp"

class User {
private:
    std::string username;
    size_t password;
    bool admin_status;
    bool authenticated;
    uint size_limit;
    std::string curr_dir;

public:
    User(std::string _username, std::string _password, bool _admin_status, int _size);
    ~User();
    std::string get_username();
    bool is_admin();
    void login(std::string password);
    void logout();
    bool is_logged_in();
    std::string get_cwd();
    void set_cwd(std::string dir);
    bool can_download(uint size);
    void subtract_size(uint size);
};

#endif 
