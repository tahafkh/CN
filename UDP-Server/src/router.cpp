#include "../include/router.hpp"

using namespace std;

int socket_fd;
struct sockaddr_in router_addr, station_addr;
socklen_t station_addr_len;

// RED variables
std::time_stamp q_time = current_time(); // Time since the queue was last idle
int count = 0;                           // number of packets enqueued since last drop


std::queue<char*> buffer;
int buffer_size = 0;
bool read_done = false;
bool send_done = false;
mutex buffer_mutex;

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

bool setup_connection() {
    memset(&router_addr, 0, sizeof(router_addr));
    /* Fill router address data structure */
    router_addr.sin_family = AF_INET;
    router_addr.sin_addr.s_addr = INADDR_ANY; 
    router_addr.sin_port = htons(port);

    /* Create socket file descriptor */ 
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL)) < 0) {
        perror("socket");
        return false;
    }

    /* Bind socket to server address */
    if (bind(socket_fd, (const struct sockaddr *)&(router_addr), sizeof(router_addr)) < 0) { 
        perror("bind");
        return false;
    }

    return true;
}

void send_packet() {
    cout << "Router is sending..." << endl;
    char frame[MAX_DATA_SIZE];

    while (true) {
        if (buffer_size==0)
            continue;
        buffer_mutex.lock();
        /* Forward the front frame to socket */
        string front_data = buffer.front();
        buffer.pop();
        buffer_size--;
        buffer_mutex.unlock();
        int data_size = strlen(front_data.c_str());
        if (!data_size)
            continue;
        
        memcpy(frame, front_data.c_str(), data_size);

        memset(&station_addr, 0, sizeof(station_addr));

        station_addr.sin_family = AF_INET;
        station_addr.sin_addr.s_addr = inet_addr("localhost"); // localhost
        if (frame[IS_ACK_INDEX] == 0x0){  // Is not ack
            uint32_t net_sender_port;
            memcpy(&net_sender_port, frame + 2, 4);

            station_addr.sin_port = htons(ROUTER_PORT);
        }
        else {
            uint32_t net_sender_port;
            memcpy(&net_sender_port, frame + 2, 4);

            station_addr.sin_port = htons(net_sender_port);
        }
        cout << "Sending packet to " << ntohs(station_addr.sin_port) << endl;
        sendto(socket_fd, frame, data_size, 0,
                (const struct sockaddr *) &station_addr,
                station_addr_len);
    }
}

void recv_packet() {
    cout << "Router is listening..." << endl;
    srand (time(nullptr));
    read_done = false;
    char frame[MAX_FRAME_SIZE];

	while(!read_done) {
        int frame_size = recvfrom(socket_fd, (char *) frame, MAX_FRAME_SIZE,
                    MSG_WAITALL, (struct sockaddr *) &station_addr, 
                    &station_addr_len);
        if (frame_size < 0) {
            perror("recvfrom");
            sleep(3000);
            exit(1);
        }

        buffer_mutex.lock();
        // if (rand()%100 < LOSS_RATE) {
        //     cerr << "packet lost" << endl;
        //   buffer_mutex.unlock();
        //     continue;
        // } else if (red_enabled && !red_check_queue(max_buffer_size, buffer_size)) {
        //     cerr << "packet dropped" << endl;
        //   buffer_mutex.unlock();
        // continue;
        // }

        cout << "Received frame from " << ntohs(station_addr.sin_port) << endl;

        buffer.push(frame);
        cout << buffer.size() << endl;
        buffer_size++;
        read_done = frame[EOT_INDEX] != 0x0;
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

    if (!setup_connection()) {
        cerr << "router setup failed" << endl;
        return 1;
    }

    thread send_thread(send_packet);
    recv_packet();
    send_thread.join();

    cout << "Finishing up send and receive" << endl;
    cout << "Done" << endl;

    return 0;
}
