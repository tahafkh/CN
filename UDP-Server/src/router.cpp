#include "../include/router.hpp"

using namespace std;

int socket_fd;
struct sockaddr_in router_addr, station_addr;
socklen_t station_addr_size;

// RED variables
std::time_stamp q_time = current_time(); // Time since the queue was last idle
int count = 0;                           // number of packets enqueued since last drop


std::queue<std::string> buffer;
bool read_done = false;
bool send_done = false;
mutex buffer_mutex;

map<int, pair<sockaddr_in, socklen_t> > forward_table;
// sender_id : sock_addr_in
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

void connect_to_station(char *frame) {
    if (frame[0]) // frame sent from receiver
        forward_table[RECV_ID] = make_pair(station_addr, station_addr_size);
    else { // frame sent from sender
        uint32_t net_sender_id;
        memcpy(&net_sender_id, frame + 2, 4);
        // todo: send file name to receiver
        forward_table[net_sender_id] = make_pair(station_addr, station_addr_size);
    }
}

bool setup_connection(int port) {
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

    return true;
}

void send_packet() {

    char frame[MAX_DATA_SIZE];

    while (!read_done) {
        send_done = false;
        while (buffer.size()) {
            buffer_mutex.lock();
            /* Forward the front frame to socket */
            string front_data = buffer.front();
            buffer.pop();
            buffer_mutex.unlock();
            int data_size = strlen(front_data.c_str());
            memcpy(frame, front_data.c_str(), data_size);
            if (frame[IS_ACK_INDEX] == 0x0){  // Is not ack

                uint32_t net_sender_id;
                memcpy(&net_sender_id, frame + 2, 4);

                // ????
                sendto(socket_fd, frame, data_size, 0, 
                    (const struct sockaddr *) &(forward_table[net_sender_id].first),
                    forward_table[net_sender_id].second);
            }
            else {
                sendto(socket_fd, frame, data_size, 0, 
                    (const struct sockaddr *) &(forward_table[RECV_ID].first),
                    forward_table[RECV_ID].second);
            }
        }
        send_done = true;
    }
}

void recv_packet() {
    srand (time(NULL));
    read_done = false;
    int router_addr_len = sizeof(router_addr); 

	while(!read_done) {
        char frame[MAX_DATA_SIZE] = {0};
        int frame_size;

        // todo: match the frame to the sender
        memset(&station_addr, 0, sizeof(station_addr)); 
        frame_size = recvfrom(socket_fd, (char *) frame, MAX_FRAME_SIZE, 
                    MSG_WAITALL, (struct sockaddr *) &station_addr, 
                    &station_addr_size);

        // check if it's a frame zero
        if (frame[frame_size-1] == 0x1)
        {
            connect_to_station(frame);
            continue;
        }

        buffer_mutex.lock();
        bool eot = frame[EOT_INDEX] == 0x0 ? true : false;
        if (rand()%100 < LOSS_RATE) {
            cerr << "packet lost" << endl;
            continue;
        } else if (red_enabled && !red_check_queue(max_buffer_size, buffer.size())) {
            cerr << "packet dropped" << endl;
            continue;
        }
        
        buffer.push(frame);
        read_done = eot ? true : false;
        buffer_mutex.unlock();
	}
}

int main(int argc, char * argv[]) {

    if (argc == 4) {
        port = atoi(argv[1]);
        max_buffer_size = atoi(argv[2]);
        red_enabled = atoi(argv[3]);
    } else {
        cerr << "usage: router <port> <buffer_size> <red_enable>" << endl;
        return 1;
    }

    if (setup_connection(port) == false) {
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
