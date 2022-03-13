#include "../include/logger.hpp"

Logger::Logger() {
    log_file.open("log.txt", std::ios::out | std::ios::app);
}

Logger::~Logger() {
    log_file.close();
}

std::time_t Logger::get_time() {
    auto curr_time = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(curr_time);
}

void Logger::save_log(std::string message) {
    std::time_t time = get_time();
    log_file << message << " At: " << std::ctime(&time) << std::endl;
}
