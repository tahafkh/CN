#include "../include/router.hpp"

using namespace std;

struct Frame {
    char data[MAX_FRAME_SIZE];
};

int socket_fd;
struct sockaddr_in router_addr;

// RED variables
std::time_stamp q_time = current_time(); // Time since the queue was last idle
int count = 0;                           // number of packets enqueued since last drop


std::queue<struct Frame> buffer;
int buffer_size = 0;
mutex buffer_mutex;

map<int, struct sockaddr_in> forwarding_table;

// argvs
int port;
int max_buffer_size = 20;
bool red_enabled = false;

double get_avg_queue_len(std::time_stamp &q_time, int q_len) {
    double avg;
    if (q_len == 0) { // queue empty
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
    pd = avg - minth_coeff * max_buffer_size;
    pd *= maxp;
    pd /= (max_buffer_size * maxth_coeff - max_buffer_size * minth_coeff);
    pa = pd / (1 - (count * pd));
    if (count == 1 / maxp) {
        // count has reached 1/maxp, 
        // Need to drop packets now
        cout << "Count has reached 1/maxp. Dropping packet" << endl;
        pa = 1.0;
    }
}

bool red_check_queue(int max_buffer_size, int q_len) {
    double new_avg = get_avg_queue_len(q_time, q_len);
    if (new_avg < minth_coeff * max_buffer_size) {
        return true; // push packet
    } else if (new_avg > maxth_coeff * max_buffer_size) {
        return false; // drop packet
    } else {
        double pd, pa;
        calc_pd_pa(pd, pa, new_avg);

        // decision making

        double rand_prob = (rand() % 100) / 100.00;
        if (rand_prob <= pa) {
            if (count != 1 / maxp)
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
    router_addr.sin_port = htons(ROUTER_PORT);

    /* Create socket file descriptor */
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL)) < 0) {
        perror("socket");
        return false;
    }

    /* Bind socket to server address */
    if (bind(socket_fd, (const struct sockaddr *) &(router_addr), sizeof(router_addr)) < 0) {
        perror("bind");
        return false;
    }

    return true;
}

void send_packet() {
    cout << "Router is sending..." << endl;

    while (true) {
        if (buffer_size == 0)
            continue;
        buffer_mutex.lock();
        /* Forward the front frame to socket */
        struct Frame front_frame = buffer.front();
        buffer.pop();
        buffer_size--;
        buffer_mutex.unlock();

        int dest_port = RECEIVER_PORT;
        int seq_num;
        memcpy(&seq_num, front_frame.data + SEQ_INDEX, 4);

        if (front_frame.data[IS_ACK_INDEX] != 0x0) {  // Is ACK
            memcpy(&dest_port, front_frame.data + 2, 4);
            cout << "Sending ACK " << seq_num << " to " << ntohs(forwarding_table[dest_port].sin_port) << endl;
        }
        else
            cout << "Sending packet " << seq_num << " to " << ntohs(forwarding_table[dest_port].sin_port) << endl;

        sendto(socket_fd, front_frame.data, MAX_FRAME_SIZE, MSG_CONFIRM,
               (struct sockaddr *) &forwarding_table[dest_port],
               sizeof(forwarding_table[dest_port]));
    }
}

void read_frame() {
    sockaddr_in station_addr{};
    socklen_t station_addr_len;

    memset(&station_addr, 0, sizeof(station_addr));

    struct Frame frame{};

    int frame_size = recvfrom(socket_fd, (char *) frame.data, MAX_FRAME_SIZE,
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

    int seq_num;
    memcpy(&seq_num, frame.data + SEQ_INDEX, 4);
    cout << "Received frame " << seq_num << " from " << ntohs(station_addr.sin_port) << endl;

    forwarding_table[ntohs(station_addr.sin_port)] = station_addr;

    buffer.push(frame);
    cout << buffer.size() << endl;
    buffer_size++;
    buffer_mutex.unlock();
}

void recv_packet() {
    cout << "Router is listening..." << endl;
    srand(time(nullptr));

    while (true) {
        read_frame();
        cout << "SIZE: " << forwarding_table.size() << endl;
    }
}

int main(int argc, char *argv[]) {
    struct sockaddr_in receiver_addr{};
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_addr.s_addr = INADDR_ANY;
    receiver_addr.sin_port = htons(RECEIVER_PORT);

    forwarding_table[RECEIVER_PORT] = receiver_addr;

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
    sleep(3000);

    return 0;
}
