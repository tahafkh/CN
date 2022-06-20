#include "../include/router.hpp"

#define MAX_BUFFER_SIZE 1024

using namespace std;

int socket_fd;
struct sockaddr_in send_server_addr, send_client_addr;
struct sockaddr_in recv_server_addr, recv_client_addr;

std::queue<std::string> buffer;
bool read_done = false;
bool send_done = false;

std::queue<std::string> ack_buffer;
bool read_ack_done = false;
bool send_ack_done = false;

bool connect_to_station(int port, struct sockaddr_in *server_addr, sockaddr_in *client_addr, int &fd) {
    memset(&(*server_addr), 0, sizeof(*server_addr)); 
    memset(&(*client_addr), 0, sizeof(*client_addr)); 
      
    /* Fill server address data structure */
    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY; 
    server_addr->sin_port = htons(port);

    /* Create socket file descriptor */ 
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "socket creation failed" << endl;
        return false;
    }

    /* Bind socket to server address */
    if (bind(fd, (const struct sockaddr *)&(*server_addr), sizeof(*server_addr)) < 0) { 
        cerr << "socket binding failed" << endl;
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
            
            sendto(socket_fd, data, data_size, 0, 
                    (const struct sockaddr *) &send_server_addr, sizeof(send_server_addr));
        }
        send_done = true;
    }
}

void recv_packet() {
    read_done = false;
    char data[MAX_DATA_SIZE];
    while (!read_done) {
        /* Receive current buffer from sender */
        // Router simply forwards the packet, no error detection
        
        int data_size;
        socklen_t client_addr_size;
        data_size = recvfrom(socket_fd, (char *) data, MAX_FRAME_SIZE, 
                MSG_WAITALL, (struct sockaddr *) &send_client_addr, 
                &client_addr_size);

        bool eot = data[0] == 0x0 ? true : false;
        buffer.push(data);

        if (eot)
            read_done = true;
    }
}

int main(int argc, char * argv[]) {
    int recv_port;
    int send_port;

    if (argc == 3) {
        recv_port = atoi(argv[1]);
        send_port = atoi(argv[2]);
    } else {
        cerr << "usage: router <recv_port> <send_port>" << endl;
        return 1;
    }

    if (connect_to_station(recv_port, &recv_server_addr, &recv_client_addr, s) == false) {
        cerr << "connect to receiver failed" << endl;
        return 1;
    }
    else if (connect_to_station(send_port, &send_server_addr, &send_client_addr) == false) {
        cerr << "connect to sender failed" << endl;
        return 1;
    }

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
