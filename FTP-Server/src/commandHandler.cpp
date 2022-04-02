#include "../include/commandHandler.hpp"

CommandHandler::CommandHandler(DataBase* _database, Logger* _logger):
	database(_database), logger(_logger) {
	
	int opt = 1;

    data_addr.sin_family = AF_INET;
    data_addr.sin_addr.s_addr = INADDR_ANY;
    data_addr.sin_port = htons(database->get_data_port());

	if ((data_socket = socket(AF_INET , SOCK_STREAM , IP_PROTOCOL)) == 0) {
		throw SocketCreationFailed();
	}

    if (setsockopt(data_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&opt, sizeof(opt)) < 0) 
		throw SetSockOptFailed(); 
       
	if (bind(data_socket, (struct sockaddr *)&data_addr, sizeof(data_addr)) < 0) 
		throw BindFailed();

	if (listen(data_socket, 4) < 0) 
		throw ListenFailed();
}

CommandHandler::~CommandHandler() {
	close(data_socket);
}

std::string CommandHandler::run_command_handler(std::string input, int client_fd) {
	try {
		tokenize_input(input);
		return handle_command(client_fd);
	}
	catch (std::exception &ex) {
		return ex.what();
	}
}

void CommandHandler::tokenize_input(std::string input) {
    std::stringstream input_string_stream(input);
    input_words = std::vector<std::string>(
        std::istream_iterator<std::string>(input_string_stream),
        std::istream_iterator<std::string>()
    );
}

int CommandHandler::create_data_connection(int fd) {
	char buff[100] = {0};
	strcpy(buff, "connect");
	send(fd, buff, strlen(buff), 0);

	struct sockaddr_in client_addr;
	int addr_len = sizeof(client_addr);
	memset(&client_addr, 0, sizeof(client_addr));
	int new_socket;
	if ((new_socket = accept(data_socket, (struct sockaddr *)&client_addr, (socklen_t*)&addr_len)) < 0) {
		throw AcceptFailed();
	}
	return new_socket;
}

std::string CommandHandler::build_path(std::string input_path) {
    std::string curr_cwd = user->get_cwd();
    if (input_path[0] == '/') {
        return input_path;
    }
    else if (input_path[0] == '.') {
        if (input_path[1] == '.')
            return curr_cwd.substr(0, curr_cwd.find_last_of('/')) + input_path.substr(2);
        else if (input_path[1] == '/')
            return curr_cwd + input_path.substr(1);
    }
	return curr_cwd + '/' + input_path;
}

std::string CommandHandler::mkd_command(int client_fd) {
	if (user == nullptr || !user->is_logged_in())
		throw UserNotLoggedin();
	if (input_words.size() < 2)
		throw WritingError();

	std::string path = build_path(input_words[1]);
	mkdir(path.c_str(), 0777);

	return path;
}

std::string CommandHandler::dele_command(User* user) {
	if (user == nullptr || !user->is_logged_in())
		throw UserNotLoggedin();
	if (input_words.size() < 3)
		throw WritingError();
	
	struct stat sb;
    std::string path = build_path(input_words[2]);
	if (input_words[1] == "-f") {
		if (database->is_restricted(input_words[2]) && !user->is_admin())
		    throw IllegalAccess();
		
		if (stat(path.c_str(), &sb) == 0 && S_ISREG(sb.st_mode))
			remove(path.c_str());
		else
			throw WritingError();
		logger->save_log("User with username: '" + user->get_username() + "' deleted file with path: '" + 
									path +  "'.");
	} else if (input_words[1] == "-d") {
		if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
			system(("rm -rf " + path).c_str());
		else 
			throw WritingError();
		logger->save_log("User with username: '" + user->get_username() + "' deleted directory with path: '" + 
									path +  "'.");
	} else {
		throw WritingError();
	}

	return path;
}

void CommandHandler::ls_command(int client_fd) {
	if (user == nullptr || !user->is_logged_in())
		throw UserNotLoggedin();
	
	int data_fd = database->get_data_fd(client_fd);
	char tmp[100] = {0};
	strcpy(tmp, "ls");
	if (send(client_fd, tmp, strlen(tmp), 0) < 0)
		throw SendDataFailed();
	std::vector<std::string> dir_list;
	std::string result;

    DIR *d;
    struct dirent *dir;
    d = opendir(user->get_cwd().c_str());
    if (d) {
        while ((dir = readdir(d)) != NULL)
            dir_list.push_back(dir->d_name);
        closedir(d);
    }
    sort(dir_list.begin(), dir_list.end());

    for(std::string dir_name: dir_list) {
    	result.append(dir_name + "\n");
	}

	if (fork() == 0) {
		if (send(data_fd, result.data(), result.size(), 0) < 0)
			throw SendDataFailed();
		exit(0);
	}
}

void CommandHandler::cwd_command(User* user) {
	if (user == nullptr || !user->is_logged_in())
		throw UserNotLoggedin();
	if (input_words.size() == 1) {
		user->set_cwd(getenv("PWD"));
	} else {
		struct stat sb;
		std::string path = build_path(input_words[1]);
		std::cout << path << std::endl;
		if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
			user->set_cwd(path);
		else
			throw WritingError();
	}
}

void CommandHandler::rename_command(User* user) {
	if (user == nullptr || !user->is_logged_in())
		throw UserNotLoggedin();
	if (input_words.size() < 3)
		throw WritingError();
	if (database->is_restricted(input_words[1]) && !user->is_admin())
		throw IllegalAccess();
	if (input_words[1].find('/') != std::string::npos ||
		input_words[2].find('/') != std::string::npos)
		throw WritingError();

	std::string file_name = user->get_cwd() + "/" + input_words[1];
	std::string new_name = user->get_cwd() + "/" + input_words[2];
	if (std::rename(file_name.c_str(), new_name.c_str()) < 0)
		throw WritingError();
	logger->save_log("User with username: '" + user->get_username() + "' renamed file with name: '" + 
								input_words[1] +  "' to new name: '" + input_words[2] + "'.");
}

void CommandHandler::retr_command(int client_fd) {
	if (user == nullptr || !user->is_logged_in())
		throw UserNotLoggedin();
	if (input_words.size() < 2)
		throw WritingError();
	if (database->is_restricted(input_words[1]) && !user->is_admin())
		throw IllegalAccess();
	
	struct stat sb;
	std::string path = build_path(input_words[1]);
	if (stat(path.c_str(), &sb) != 0 || !S_ISREG(sb.st_mode))
		throw WritingError();
	if (!user->can_download(sb.st_size))
		throw FullCapacity();
	user->subtract_size(sb.st_size);
	
	int data_fd = database->get_data_fd(client_fd);
	struct stat stat_buf; 
	int file_fd = open(path.c_str() , O_RDONLY);
    fstat (file_fd, &stat_buf);
	char tmp[100] = {0};
	strcpy(tmp, "dl ");
	strcat(tmp, (path.substr(path.find_last_of("/\\") + 1)).c_str());
	strcat(tmp, "#");
	strcat(tmp, std::to_string(stat_buf.st_size).c_str());
	strcat(tmp, "$");

	if (send(client_fd, tmp, strlen(tmp), 0) < 0)
		throw SendDataFailed();

	if (fork() == 0) {
    	sendfile(data_fd, file_fd, NULL, stat_buf.st_size);
    	close(file_fd);
		close(data_fd);
		exit(0);
	}
	logger->save_log("User with username: '" + user->get_username() + "' downloaded file with name: '" + 
								input_words[1] +  "'.");
	close(file_fd);
}

void CommandHandler::user_command(int client_fd) {
	if (input_words.size() == 1)
		throw WritingError();
	if (user != nullptr && user->is_logged_in())
    	throw BadSequence();
	user = database->find_user(input_words[1]);
	if (user->is_logged_in())
		throw BadSequence();
	database->set_user_fd(client_fd, user);
}

void CommandHandler::pass_command(int client_fd) {
	if (input_words.size() == 1)
		throw WritingError();
	if (user == nullptr || user->is_logged_in())
		throw BadSequence();
	user->login(input_words[1]);
	int sock = create_data_connection(client_fd);
	database->set_data_fd(client_fd, sock);
}

std::string CommandHandler::help_command(int client_fd) {
	std::stringstream message;

	message << "214\n"
			<< "1) USER [name], Its argument is used to specify the user's string. It is used for user authentication.\n"
			<< "2) Pass [password], Its argument is used to specify the user's password. It is used for user login.\n"
			<< "3) PWD, It is used for get the path of current working directory.\n"
			<< "4) MKD [directory path], Its argument is used to specify the new directory's path. It is used for creat new directory in specified path\n"
			<< "5) DELE -f [file name], Its argument is used to specify the file's name. It is used for delete file with specified name\n"
			<< "6) DELE -d [directory path], Its argument is used to specify the directory's path. It is used for delete directory with specified path\n"
			<< "7) LS, It is used for view files in the current working directory.\n"
			<< "8) CWD [path], Its argument is used to specify the new directory's path. It is used for change the current working directory to specified path."
			<< "If the argument equals to '..' it goes to previous directory and if no argument is entered, it goes to the first directory.\n"
			<< "9) RENAME [from] [to], First argument is used to specify the file's name and second argument is used to specify new name for that file."
			<< "It is used for reaname file, if available.\n"
			<< "10) RETR [name], Its argument is used to specify the file's name. It is used for download file with the given name, if available.\n"
			<< "11) HELP, It is used to display commands on the server along with instructions for using them.\n"
			<< "12) QUIT, It is used for logout and remove current user from the system.\n";

	char tmp[100] = {0};
	strcpy(tmp, "hp ");
	strcat(tmp, std::to_string(message.str().size()).c_str());
	strcat(tmp, "$");

	if (send(client_fd, tmp, strlen(tmp), 0) < 0)
		throw SendDataFailed();

	return message.str();
}

void CommandHandler::quit_command(int client_fd) {
	if (user == nullptr || !user->is_logged_in())
		throw BadSequence();
	user->logout();
	database->remove_user_fd(client_fd);
	database->remove_data_fd(client_fd);
}

std::string CommandHandler::handle_command(int client_fd) {
	this->user = database->find_user(client_fd);

	if (input_words[0] == "user") {
		user_command(client_fd);
		return SUCCESS_USERNAME_FINDING;
	} 
	else if (input_words[0] == "pass") {
		pass_command(client_fd);
		logger->save_log("User with username: '" + user->get_username() + "' logged in.");
		return LOGIN_SUCCESS;
	} 
	
	else if (input_words[0] == "pwd") {
		if (user == nullptr || !user->is_logged_in())
			throw UserNotLoggedin();
		return "257: " + user->get_cwd();
	} 
	
	else if (input_words[0] == "mkd") {
		std::string path = mkd_command(client_fd);
		logger->save_log("User with username: '" + user->get_username() + "' created directory or file with path: '" + 
									path +  "'.");
		return "257: " + path + " created.";
	} 
	
	else if (input_words[0] == "dele") {
		std::string path = dele_command(user);
		return "250: " + path + " deleted";
	} 
	
	else if (input_words[0] == "ls") {
		ls_command(client_fd);
		return LIST_TRANSFER_SUCCESS;
	} 
	
	else if (input_words[0] == "cwd") {
		cwd_command(user);
		return CHANGE_SUCCESS;
	} 
	
	else if (input_words[0] == "rename") {
		rename_command(user);
		return CHANGE_SUCCESS;
	} 
	
	else if (input_words[0] == "retr") {
		retr_command(client_fd);
		return DOWNLOAD_SUCCESS;	
	} 
	
	else if (input_words[0] == "help") { 
		return help_command(client_fd);
	} 
	
	else if (input_words[0] == "quit") {
		quit_command(client_fd);
		logger->save_log("User with username: '" + user->get_username() + "' logged out.");
		return LOGOUT_SUCCESS;
	}

	throw DefaultError();
}
