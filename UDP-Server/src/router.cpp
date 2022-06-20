#include "../include/router.hpp"

#define MAX_BUFFER_SIZE 1024

using namespace std;

int socket_fd;
struct sockaddr_in router_addr;

std::queue<std::string> buffer;
bool read_done = false;
bool send_done = false;

map<int, int> socket_id_fd_map;
int curr_id = 1;

bool connect_to_station(int port) {
    memset(&router_addr, 0, sizeof(router_addr)); 
      
    /* Fill router address data structure */
    router_addr.sin_family = AF_INET;
    router_addr.sin_addr.s_addr = INADDR_ANY; 
    router_addr.sin_port = htons(port);

    /* Create socket file descriptor */ 
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL)) < 0) {
        cerr << "socket creation failed" << endl;
        return false;
    }

    /* Bind socket to server address */
    if (bind(socket_fd, (const struct sockaddr *)&(router_addr), sizeof(router_addr)) < 0) { 
        cerr << "socket binding failed" << endl;
        return false;
    }

    if (listen(socket_fd, TOTAL_STATIONS) < 0) {
        cerr << "listen failed" << endl;
        return false;
    }

    return true;
}

void send_packet() {

    char data[MAX_DATA_SIZE];

    while (!read_done) {
        send_done = false;
        while (buffer.size()) {
            /* Forward the front frame to socket */
            string front_data = buffer.front();
            buffer.pop();
            int data_size = strlen(front_data.c_str());
            memcpy(data, front_data.c_str(), data_size);
            if (data[IS_ACK_INDEX] == 0x0){  // Is not ack

                uint32_t net_sender_id;
                memcpy(&net_sender_id, data + 2, 4);

                uint32_t net_seq_num;
                memcpy(&net_seq_num, data + 6, 4);

                if (send(socket_id_fd_map[net_sender_id], data, data_size, 0) < 0) {
                    cerr << "send of " << net_seq_num << " failed" << endl;
                }
            }
            else {
                uint32_t net_seq_num;
                memcpy(&net_seq_num, data + 6, 4);
                
                if (send(socket_id_fd_map[0], data, data_size, 0) < 0) {
                    cerr << "send ACK of " << net_seq_num << " failed" << endl;
                }
            }
        }
        send_done = true;
    }
}

void recv_packet() {
    read_done = false;
    int router_addr_len = sizeof(router_addr); 
	fd_set master_set, working_set;
	int new_socket;

	FD_ZERO(&master_set);
	int max_sd = socket_fd;
	FD_SET(socket_fd, &master_set);

	while(!read_done) {
		working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL); 

        for (int i = 0; i <= max_sd; i++) {
            if (FD_ISSET(i, &working_set)) {
                if (i == socket_fd) { // New station connected
					if ((new_socket =  accept(socket_fd, (struct sockaddr *)&router_addr, 
                        (socklen_t*)&router_addr_len)) < 0) {
                        cerr << "accept failed" << endl;
					}
                    socket_id_fd_map[curr_id++] = new_socket;
                    FD_SET(new_socket, &master_set);
                    if (new_socket > max_sd)
                        max_sd = new_socket;
                }
                else { // New message from a station
					char data[MAX_DATA_SIZE] = {0};
					int temp;
					if ((temp = recv(i , data, MAX_DATA_SIZE, 0)) < 0) {
                        cerr << "recv failed" << endl;
					}
					else if (temp == 0) { // Station disconnected
						close(i);
                        socket_id_fd_map[curr_id++] = -1;
						FD_CLR(i, &master_set);
					} else {
                        bool eot = data[EOT_INDEX] == 0x0 ? true : false;
                        buffer.push(data);

                        if (eot)
                            read_done = true;
					}
                }
            }
        }
	}
}

int main(int argc, char * argv[]) {
    int port;
    // int send_port;

    if (argc == 2) {
        port = atoi(argv[1]);
        // send_port = atoi(argv[2]);
    } else {
        cerr << "usage: router <port>" << endl;
        return 1;
    }

    if (connect_to_station(port) == false) {
        cerr << "router setup failed" << endl;
        return 1;
    }
    // else if (connect_to_station(send_port) == false) {
    //     cerr << "connect to sender failed" << endl;
    //     return 1;
    // }

    /* Start thread to keep sending packets to receiver */
    thread recv_thread(send_packet);

    /* Start thread to keep receiving packets from sender */
    thread send_thread(recv_packet);

    if (read_done && send_done) {
        cout << "Finishing up send and receive" << endl;
        recv_thread.join();
        send_thread.join();
    }
    cout << "Done" << endl;

    return 0;
}
