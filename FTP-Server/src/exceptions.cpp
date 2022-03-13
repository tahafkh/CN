#include "../include/exceptions.hpp"

UserNotLoggedin::UserNotLoggedin() {
	message = USER_NOT_LOGGIN;
}

const char *UserNotLoggedin::what() const throw() {
	return message.c_str();
}

WritingError::WritingError() {
	message = WRITING_ERROR;
}

const char *WritingError::what() const throw() {
	return message.c_str();
}

BadSequence::BadSequence() {
	message = BAD_SEQUENCE;
}

const char *BadSequence::what() const throw() {
	return message.c_str();
}


DefaultError::DefaultError() {
	message = DEFAULT_ERROR;
}

const char *DefaultError::what() const throw() {
	return message.c_str();
}

FullCapacity::FullCapacity() {
	message = FULL_CAPACITY;
}

const char *FullCapacity::what() const throw() {
	return message.c_str();
}


UserNotFound::UserNotFound() {
	message = USER_NOT_FOUND;
}

const char *UserNotFound::what() const throw() {
	return message.c_str();
}

IllegalAccess::IllegalAccess() {
	message = ILLEGAL_ACCESS;
}

const char *IllegalAccess::what() const throw() {
	return message.c_str();
}

SocketCreationFailed::SocketCreationFailed() {
	message = SOCKET_CREATION_FAILED;
}

const char *SocketCreationFailed::what() const throw() {
	return message.c_str();
}

AddressFailed::AddressFailed() {
    message = ADDRESS_FAILED;
}

const char *AddressFailed::what() const throw() {
	return message.c_str();
}

ConnectionFailed::ConnectionFailed() {
	message = CONNECTION_FAILED;
}

const char *ConnectionFailed::what() const throw() {
	return message.c_str();
}

SetSockOptFailed::SetSockOptFailed() {
	message = SET_SOCK_OPT__FAILED;
}

const char *SetSockOptFailed::what() const throw()
{
	return message.c_str();
}

BindFailed::BindFailed() {
	message = BIND_FAILED;
}

const char *BindFailed::what() const throw() {
	return message.c_str();
}

ListenFailed::ListenFailed() {
	message = LISTEN_FAILED;
}

const char *ListenFailed::what() const throw() {
	return message.c_str();
}

AcceptFailed::AcceptFailed() {
	message = ACCEPT_FAILED;
}

const char *AcceptFailed::what() const throw() {
	return message.c_str();
}

SendDataFailed::SendDataFailed() {
	message = SEND_DATA_FAILED;
}

const char *SendDataFailed::what() const throw() {
	return message.c_str();
}

ReceiveDataFailed::ReceiveDataFailed() {
	message = RECIVE_DATA_FAILED;
}

const char *ReceiveDataFailed::what() const throw() {
	return message.c_str();
}
