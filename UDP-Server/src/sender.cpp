#include "../include/sender.hpp"

using namespace std;

int socket_fd;
struct sockaddr_in sender_addr, router_addr;
socklen_t router_addr_size;

int window_len;
bool *window_ack_mask;
time_stamp *window_sent_time;
int lar, lfs;

time_stamp TMIN = current_time();
mutex window_info_mutex;

bool read_ack(int *seq_num, bool *neg, char *ack) {
    *neg = ack[1] == 0x0; // first byte is ISACK

    uint32_t net_seq_num;
    memcpy(&net_seq_num, ack + 10, 4);
    *seq_num = net_seq_num;

    return ack[ACK_SIZE-1] != checksum(ack, ACK_SIZE - (int) 1);
}

void listen_ack() {
    char ack[ACK_SIZE];
    int ack_size;
    int ack_seq_num;
    bool ack_error;
    bool ack_neg;

    /* Listen for ack from receiver */
    while (true) {
        sockaddr_in new_addr{};
        socklen_t new_addr_size;
        ack_size = recvfrom(socket_fd, (char *)ack, ACK_SIZE, 
                MSG_WAITALL, (struct sockaddr *) &new_addr, 
                &new_addr_size);
        cout << htons(new_addr.sin_port) << " " << ack_size << endl;
        ack_error = read_ack(&ack_seq_num, &ack_neg, ack);
        cout << "Received ACK: " << ack_seq_num << endl;

        window_info_mutex.lock();

        if (!ack_error && ack_seq_num > lar && ack_seq_num <= lfs) {
            if (!ack_neg) {
                window_ack_mask[ack_seq_num - (lar + 1)] = true;
            } else {
                window_sent_time[ack_seq_num - (lar + 1)] = TMIN;
            }
        }

        window_info_mutex.unlock();
    }
}

int create_frame(int seq_num, char *frame, char *data, int data_size, bool eot) {
    frame[0] = 0x0;
    frame[1] = eot ? 0x0 : 0x1;
    int ptr = 2;
    // Set src port
    uint32_t net_send_port = sender_addr.sin_port;
    memcpy(frame + ptr, &net_send_port, 4);
    ptr += 4;

    // Set dest port
    uint32_t net_recv_port = RECEIVER_PORT;
    memcpy(frame + ptr, &net_recv_port, 4);
    ptr += 4;

    uint32_t net_seq_num = seq_num;
    memcpy(frame + ptr, &net_seq_num, 4);
    ptr += 4;

    uint32_t net_data_size = data_size;
    memcpy(frame + ptr, &net_data_size, 4);
    ptr += 4;

    memcpy(frame + ptr, data, data_size);

    frame[data_size + ptr] = checksum(frame, data_size + ptr);
    frame[data_size + ptr+1] = 0x0;

    return data_size + ptr+2;
}

int main(int argc, char *argv[]) {
    int sender_port;
    int max_buffer_size;
    char *fname;

    if (argc == 5) {
        string file_str = SENDER_DIR + string(argv[1]);
        fname = strcpy(new char[file_str.length() + 1], file_str.c_str());
        window_len = atoi(argv[2]);
        max_buffer_size = MAX_DATA_SIZE * (int) atoi(argv[3]);
        sender_port = atoi(argv[4]);
    } else {
        cerr << "usage: sender <filename> <window_len> <buffer_size> <destination_port>" << endl;
        return 1; 
    }

    memset(&sender_addr, 0, sizeof(sender_addr)); 
    memset(&router_addr, 0, sizeof(router_addr)); 

    /* Fill server address data structure */
    sender_addr.sin_family = AF_INET;
    sender_addr.sin_addr.s_addr = INADDR_ANY; 
    sender_addr.sin_port = htons(sender_port);

    /* Fill router address data structure */
    router_addr.sin_family = AF_INET;
    router_addr.sin_addr.s_addr = INADDR_ANY; 
    router_addr.sin_port = htons(ROUTER_PORT);

    /* Create socket file descriptor */ 
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return 1;
    }

    /* Bind socket to client address */
    if (::bind(socket_fd, (const struct sockaddr *)&sender_addr, 
            sizeof(sender_addr)) < 0) { 
        perror("bind");
        return 1;
    }
    
    if (access(fname, F_OK) == -1) {
        cerr << "file doesn't exist: " << fname << endl;
        return 1;
    }
    
    /* Open file to send */
    FILE *file = fopen(fname, "rb");
    char buffer[max_buffer_size];
    int buffer_size;

    /* Start thread to listen for ack */
    thread recv_thread(listen_ack);
    char frame[MAX_FRAME_SIZE];
    char data[MAX_DATA_SIZE];
    int frame_size;
    int data_size;

    /* Send file */
    bool read_done = false;
    int buffer_num = 0;
    while (!read_done) {

        /* Read part of file to buffer */
        buffer_size = fread(buffer, 1, max_buffer_size, file);
        if (buffer_size == max_buffer_size) {
            char temp[1];
            int next_buffer_size = fread(temp, 1, 1, file);
            if (next_buffer_size == 0) 
                read_done = true;
            int error = fseek(file, -1, SEEK_CUR);
        } else if (buffer_size < max_buffer_size) {
            read_done = true;
        }
        
        window_info_mutex.lock();

        /* Initialize sliding window variables */
        int seq_count = buffer_size / MAX_DATA_SIZE + ((buffer_size % MAX_DATA_SIZE == 0) ? 0 : 1);
        int seq_num;
        window_sent_time = new time_stamp[window_len];
        window_ack_mask = new bool[window_len];
        bool window_sent_mask[window_len];
        for (int i = 0; i < window_len; i++) {
            window_ack_mask[i] = false;
            window_sent_mask[i] = false;
        }
        lar = -1;
        lfs = lar + window_len;

        window_info_mutex.unlock();
        
        /* Send current buffer with sliding window */
        bool send_done = false;
        while (!send_done) {

            window_info_mutex.lock();

            /* Check window ack mask, shift window if possible */
            if (window_ack_mask[0]) {
                int shift = 1;
                for (int i = 1; i < window_len; i++) {
                    if (!window_ack_mask[i]) 
                        break;
                    shift += 1;
                }
                for (int i = 0; i < window_len - shift; i++) {
                    window_sent_mask[i] = window_sent_mask[i + shift];
                    window_ack_mask[i] = window_ack_mask[i + shift];
                    window_sent_time[i] = window_sent_time[i + shift];
                }
                for (int i = window_len - shift; i < window_len; i++) {
                    window_sent_mask[i] = false;
                    window_ack_mask[i] = false;
                }
                lar += shift;
                lfs = lar + window_len;
            }

            window_info_mutex.unlock();

            /* Send frames that has not been sent or has timed out */
            for (int i = 0; i < window_len; i ++) {
                seq_num = lar + i + 1;

                if (seq_num < seq_count) {
                    window_info_mutex.lock();

                    if (!window_sent_mask[i] || (!window_ack_mask[i] && (elapsed_time(current_time(), window_sent_time[i]) > TIMEOUT))) {
                        int buffer_shift = seq_num * MAX_DATA_SIZE;
                        data_size = (buffer_size - buffer_shift < MAX_DATA_SIZE) ? (buffer_size - buffer_shift) : MAX_DATA_SIZE;
                        memcpy(data, buffer + buffer_shift, data_size);
                        
                        bool eot = (seq_num == seq_count - 1) && (read_done);
                        frame_size = create_frame(seq_num, frame, data, data_size, eot);

                        cout << "Sending frame " << seq_num << " with size " << frame_size << endl;
                        
                        sendto(socket_fd, frame, frame_size, 0, 
                                (const struct sockaddr *) &router_addr, sizeof(router_addr));
                        window_sent_mask[i] = true;
                        window_sent_time[i] = current_time();
                    }

                    window_info_mutex.unlock();
                }
            }

            /* Move to next buffer if all frames in current buffer has been acked */
            send_done = lar >= seq_count - 1;
        }

        cout << "\r" << "[SENT " << (unsigned long long) buffer_num * (unsigned long long) 
                max_buffer_size + (unsigned long long) buffer_size << " BYTES]" << flush;
        buffer_num += 1;
        if (read_done) break;
    }
    
    fclose(file);
    delete [] window_ack_mask;
    delete [] window_sent_time;

    cout << "\nGoodbye" << endl;
    return 0;
}
