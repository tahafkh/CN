#include "../include/client.hpp"

using json = nlohmann::json;

inline const std::string directory = "Downloads"; 

Client::Client() {
	read_ports();

	try {			
		connect_to_server();		
	}
	catch (std::exception &ex) {
		std::cout << ex.what() << std::endl;
	}
}

Client::~Client() {
	close(data_socket);
	close(command_socket);
}

void Client::read_ports() {
	std::ifstream json_file = std::ifstream(CONFIG_FILE);
	json json_data;
    json_file >> json_data;
	command_port = json_data[COMMAND_PORT];
    data_port = json_data[DATA_PORT];
}

void Client::connect_to_server() {
    command_addr.sin_family = AF_INET; 
	command_addr.sin_port = htons(command_port);

    if ((command_socket = socket(AF_INET, SOCK_STREAM, IP_PROTOCOL)) < 0) {
        throw SocketCreationFailed();
	}
	
	if (inet_pton(AF_INET, IP_ADDRESS, &command_addr.sin_addr) <= 0)
        throw AddressFailed();

	if (connect(command_socket, (struct sockaddr *)&command_addr, sizeof(command_addr)) < 0)
        throw ConnectionFailed();

	data_addr.sin_family = AF_INET; 
    data_addr.sin_port = htons(data_port); 
    data_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

	if ((data_socket = socket(AF_INET, SOCK_STREAM, IP_PROTOCOL)) < 0)
        throw SocketCreationFailed();
}

void Client::send_data_to_server(std::string data)
{
	if (send(command_socket, data.data(), data.size(), 0) < 0)
		throw SendDataFailed();
}

std::string Client::receive_response_from_server(int sock) {
	char buffer[MAX_BUFFER_SIZE] = {0};
	if (recv(sock, buffer, MAX_BUFFER_SIZE, 0) < 0)
		throw ReceiveDataFailed();
	return buffer;
}

void Client::receive_file_from_server(std::string response) {
	char buffer[MAX_BUFFER_SIZE] = {0};

	mkdir(directory.c_str(), 0777);
	
	// Response format: dl <name>#<size>$
	int size_len = response.find("$") - response.find("#") - 1;
	off_t file_size = std::stol(response.substr(response.find('#') +1, size_len));
	std::string file_path = directory + "/" + response.substr(3,  response.find('#')-3);
	int file_fd = open(file_path.c_str(), O_RDWR | O_CREAT, 0777);
	
	int total_read = 0;
	int last_read = 0;
	
	while (total_read < file_size) {
		if ((last_read = recv(data_socket, buffer, MAX_BUFFER_SIZE, 0)) < 0) {
			throw ReceiveDataFailed();
		}
		write(file_fd, buffer, last_read);
		memset(buffer, 0, MAX_BUFFER_SIZE);
		total_read += last_read;
	}
    close(file_fd);
}

void Client::handle_response(std::string response) {
	if (response == "connect") { // Create Data Route Connection
		if (connect(data_socket, (struct sockaddr *)&data_addr, sizeof(data_addr)) < 0)
        	throw ConnectionFailed();
		handle_response(receive_response_from_server(command_socket));
		return;
	} 
	else if (response.length() > 1 && response.substr(0,2) == "ls") {
		std::string data_resp = receive_response_from_server(data_socket);
		std::cout << data_resp << std::endl;
		handle_response(receive_response_from_server(command_socket));
		return;
	} 
	else if (response.length() > 1 && response.substr(0,2) == "dl") {
		receive_file_from_server(response);
		handle_response(receive_response_from_server(command_socket));
		return;
	} 
	else if (response.substr(0,3) == "221") {
		close(data_socket);
		if ((data_socket = socket(AF_INET, SOCK_STREAM, IP_PROTOCOL)) < 0) {
        	throw SocketCreationFailed();
		}
	} 
	else if (response.length() > 1 && response.substr(0,2) == "hp") {
		off_t msg_len = std::stol(response.substr(3, response.find('$') - 2));

		int total_read = 0;
		int last_read = 0;
		
		char buffer[MAX_BUFFER_SIZE] = {0};
		while (total_read < msg_len) {
			if ((last_read = recv(command_socket, buffer, MAX_BUFFER_SIZE, 0)) < 0) {
				throw ReceiveDataFailed();
			}
			printf("%.*s", last_read, buffer);
			memset(buffer, 0, MAX_BUFFER_SIZE);
			total_read += last_read;
		}
		return;
	}
	
	std::cout << response << std::endl;
}

void Client::run() {
	while (true) {
		std::string command;
		try {			
			getline(std::cin, command);
			send_data_to_server(command);	
			std::string response = receive_response_from_server(command_socket);
			handle_response(response);
		}
		catch (std::exception &ex) {
			std::cout << ex.what() << std::endl;
		}
    }
}

int main(int argc, char const *argv[]) {
	Client client = Client();

	client.run();

	return 0;
}
