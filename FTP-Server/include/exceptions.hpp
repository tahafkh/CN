#ifndef __EXCEPTION_HPP__
#define __EXCEPTION_HPP__

#include "defs.hpp"

#include <exception>
#include <string>


class UserNotLoggin : public std::exception {
    private:
    	std::string message;

    public:
    	UserNotLoggin();
        ~UserNotLoggin();
    	const char *what() const throw();
};

class WritingError : public std::exception {
    private:
    	std::string message;

    public:
    	WritingError();
        ~WritingError();
    	const char *what() const throw();
};

class DefaultError : public std::exception {
    private:
    	std::string message;

    public:
    	DefaultError();
        ~DefaultError();
    	const char *what() const throw();
};

class FullCapacity : public std::exception {
    private:
    	std::string message;

    public:
    	FullCapacity();
        ~FullCapacity();
    	const char *what() const throw();
};

class UserNotFound : public std::exception {
    private:
    	std::string message;

    public:
    	UserNotFound();
        ~UserNotFound();
    	const char *what() const throw();
};

class BadSequence : public std::exception {
    private:
    	std::string message;

    public:
    	BadSequence();
        ~BadSequence();
    	const char *what() const throw();
};

class IllegalAccess : public std::exception {
    private:
    	std::string message;

    public:
    	IllegalAccess();
        ~IllegalAccess();
    	const char *what() const throw();
};

class SocketCreationFailed : public std::exception {
    private:
    	std::string message;

    public:
    	SocketCreationFailed();
        ~SocketCreationFailed();
    	const char *what() const throw();
};

class AddressFailed : public std::exception {
    private:
    	std::string message;

    public:
    	AddressFailed();
        ~AddressFailed();
    	const char *what() const throw();
};

class ConnectionFailed : public std::exception {
    private:
    	std::string message;

    public:
    	ConnectionFailed();
        ~ConnectionFailed();
    	const char *what() const throw();
};

class SetSockOptFailed : public std::exception {
    private:
    	std::string message;

    public:
    	SetSockOptFailed();
        ~SetSockOptFailed();
    	const char *what() const throw();
};

class BindFailed : public std::exception {
    private:
    	std::string message;

    public:
    	BindFailed();
        ~BindFailed();
    	const char *what() const throw();
};

class ListenFailed : public std::exception {
    private:
    	std::string message;

    public:
    	ListenFailed();
        ~ListenFailed();
    	const char *what() const throw();
};

class AcceptFailed : public std::exception {
    private:
    	std::string message;

    public:
    	AcceptFailed();
        ~AcceptFailed();
    	const char *what() const throw();
};

class SendDataFailed : public std::exception {
    private:
    	std::string message;

    public:
    	SendDataFailed();
        ~SendDataFailed();
    	const char *what() const throw();
};

class ReceiveDataFailed : public std::exception {
    private:
    	std::string message;

    public:
    	ReceiveDataFailed();
        ~ReceiveDataFailed();
    	const char *what() const throw();
};

#endif