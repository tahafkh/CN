#include "../include/client.hpp"

using json = nlohmann::json;

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
	std::ifstream json_file = std::ifstream("config.json");
	json json_data;
    json_file >> json_data;
	command_port = json_data["commandChannelPort"];
    data_port = json_data["dataChannelPort"];
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

	if ((data_socket = socket(AF_INET, SOCK_STREAM, IP_PROTOCOL)) < 0) {
        throw SocketCreationFailed();
	}
}

void Client::send_data_to_server(std::string data)
{
	if (send(command_socket, data.data(), data.size(), 0) < 0)
		throw SendDataFailed();
}

std::string Client::receive_response_from_server(int sock) {
	char buffer[1024] = {0};
	if (recv(sock, buffer, 1024, 0) < 0)
		throw ReceiveDataFailed();
	return buffer;
}

void Client::receive_file_from_server(std::string res) {
	char buffer[1024] = {0};
    inline const std::string directory = "Downloads"; 
	mkdir(directory.c_str(), 0777);
	int len = res.find("$") - res.find("#") - 1;
	off_t file_len = std::stol(res.substr(res.find('#') +1, len));
	std::string file_path = directory + "/" + res.substr(3,  res.find('#')-3);
	int file_fd = open(file_path.c_str(), O_RDWR | O_CREAT, 0777);
	int total_read = 0;
	int last_read = 0;
	while (total_read < file_len) {
		if ((last_read = recv(data_socket, buffer, 1024, 0)) < 0) {
			throw ReceiveDataFailed();
		}
		write(file_fd, buffer, last_read);
		memset(buffer, 0, 1024);
		total_read += last_read;
	}
    close(file_fd);
}