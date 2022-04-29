#ifndef __EXCEPTION_HPP__
#define __EXCEPTION_HPP__

#include "defs.hpp"

#include <exception>
#include <string>


class UserNotLoggedin : public std::exception {
    private:
    	std::string message;

    public:
    	UserNotLoggedin();
    	const char *what() const throw();
};

class WritingError : public std::exception {
    private:
    	std::string message;

    public:
    	WritingError();
    	const char *what() const throw();
};

class DefaultError : public std::exception {
    private:
    	std::string message;

    public:
    	DefaultError();
    	const char *what() const throw();
};

class FullCapacity : public std::exception {
    private:
    	std::string message;

    public:
    	FullCapacity();
    	const char *what() const throw();
};

class UserNotFound : public std::exception {
    private:
    	std::string message;

    public:
    	UserNotFound();
    	const char *what() const throw();
};

class BadSequence : public std::exception {
    private:
    	std::string message;

    public:
    	BadSequence();
    	const char *what() const throw();
};

class IllegalAccess : public std::exception {
    private:
    	std::string message;

    public:
    	IllegalAccess();
    	const char *what() const throw();
};

class SocketCreationFailed : public std::exception {
    private:
    	std::string message;

    public:
    	SocketCreationFailed();
    	const char *what() const throw();
};

class AddressFailed : public std::exception {
    private:
    	std::string message;

    public:
    	AddressFailed();
    	const char *what() const throw();
};

class ConnectionFailed : public std::exception {
    private:
    	std::string message;

    public:
    	ConnectionFailed();
    	const char *what() const throw();
};

class SetSockOptFailed : public std::exception {
    private:
    	std::string message;

    public:
    	SetSockOptFailed();
    	const char *what() const throw();
};

class BindFailed : public std::exception {
    private:
    	std::string message;

    public:
    	BindFailed();
    	const char *what() const throw();
};

class ListenFailed : public std::exception {
    private:
    	std::string message;

    public:
    	ListenFailed();
    	const char *what() const throw();
};

class AcceptFailed : public std::exception {
    private:
    	std::string message;

    public:
    	AcceptFailed();
    	const char *what() const throw();
};

class SendDataFailed : public std::exception {
    private:
    	std::string message;

    public:
    	SendDataFailed();
    	const char *what() const throw();
};

class ReceiveDataFailed : public std::exception {
    private:
    	std::string message;

    public:
    	ReceiveDataFailed();
    	const char *what() const throw();
};

#endif