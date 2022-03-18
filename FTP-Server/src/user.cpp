#include "../include/user.hpp"

User::User(std::string _username, std::string _password, bool _admin_status, int _size) {
	this->username = _username;
	size_t hashed = std::hash<std::string>{}(_password);
	this->password = hashed;
    this->admin_status = _admin_status;
    this->authenticated = false;
    this->curr_dir = getenv("PWD");
    this->size_limit = _size * 1024; //KB -> B
}

User::~User() {}

std::string User::get_username() {
    return this->username;
}

bool User::is_admin() {
    return admin_status;
}

std::string User::get_cwd() {
    return curr_dir;
}

void User::set_cwd(std::string dir) {
    char tmp[300];
    realpath(dir.c_str(), tmp);
    curr_dir = std::string(tmp);
}

void User::login(std::string pw) {
    size_t hashed_pw = std::hash<std::string>{}(pw);
    if (hashed_pw == password)
        authenticated = true;
    else
        throw UserNotFound();
}

void User::logout() {
    if (!authenticated)
        throw BadSequence();
    authenticated = false;
}

bool User::is_logged_in() {
    return authenticated;
}

bool User::can_download(uint size) {
    return size_limit >= size;
}

void User::subtract_size(uint size) {
    size_limit -= size;
}