#include "../include/router.hpp"

using namespace std;

int socket_fd;
struct sockaddr_in router_addr;

// RED variables
std::time_stamp q_time = current_time(); // Time since the queue was last idle
int count = 0;                           // number of packets enqueued since last drop


std::queue<std::string> buffer;
bool read_done = false;
bool send_done = false;

map<int, int> socket_id_fd_map;
int curr_id = 1;

// argvs
int port;
int max_buffer_size = 20;
bool red_enabled = false;

double get_avg_queue_len(std::time_stamp &q_time, int q_len) {
    double avg;
    if(q_len == 0) { // queue empty
        double m = elapsed_time(current_time(), q_time);
        avg = pow((1 - wq), m) * avg;

        // Update q_time, since the queue is now empty
        q_time = current_time();
    } else {
        avg = ((1 - wq) * avg) + (wq * q_len);
    }

    return avg;
}

void calc_pd_pa(double &pd, double &pa, int avg) {
    count++;
    pd = avg - minth_coeff*max_buffer_size;
    pd *= maxp;
    pd /= (max_buffer_size*maxth_coeff - max_buffer_size*minth_coeff);
    pa = pd/(1 - (count * pd));
    if(count == 1/maxp) {
        // count has reached 1/maxp, 
        // Need to drop packets now
        cout << "Count has reached 1/maxp. Dropping packet" << endl;
        pa = 1.0;
    }
}

bool red_check_queue(int max_buffer_size, int q_len) {
    double new_avg = get_avg_queue_len(q_time, q_len);
    if(new_avg < minth_coeff * max_buffer_size) {
        return true; // push packet
    } else if (new_avg > maxth_coeff * max_buffer_size) {
        return false; // drop packet
    } else {
        double pd, pa;
        calc_pd_pa(pd, pa, new_avg);

        // decision making

        double rand_prob = (rand()%100)/100.00;
        if(rand_prob <= pa) {
            if(count != 1/maxp)
                cout << "Dropping packet" << endl;
            count = 0;
            return false;
        } else {
            count = -1;
            return true;
        }
    }
}

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
    srand (time(NULL));
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
                        //todo: get_id
						FD_CLR(i, &master_set);
					} else {
                        bool eot = data[EOT_INDEX] == 0x0 ? true : false;
                        if (rand()%100 < LOSS_RATE) {
                            cerr << "packet lost" << endl;
                            continue;
                        } else if (red_enabled && !red_check_queue(max_buffer_size, buffer.size())) {
                            cerr << "packet dropped" << endl;
                            continue;
                        }
                        
                        buffer.push(data);
                        read_done = eot ? true : false;
					}
                }
            }
        }
	}
}

int main(int argc, char * argv[]) {
    // int send_port;

    if (argc == 4) {
        port = atoi(argv[1]);
        max_buffer_size = atoi(argv[2]);
        red_enabled = atoi(argv[3]);
    } else {
        cerr << "usage: router <port> <buffer_size> <red_enable>" << endl;
        return 1;
    }

    if (connect_to_station(port) == false) {
        cerr << "router setup failed" << endl;
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
