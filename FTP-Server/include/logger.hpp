#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include "defs.hpp"
#include "headers.hpp"

class Logger {
private:
    std::fstream log_file;

public:
    Logger();
    ~Logger();
    std::time_t get_time();
    void save_log(std::string message);
};

#endif