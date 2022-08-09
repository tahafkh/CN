#include "../include/receiver.hpp"

using namespace std;

ofstream logger = ofstream("./logs/receiver.log");

int socket_fd;
struct sockaddr_in receiver_addr, router_addr;

bool read_frame(int *sender_port, int *seq_num, char *data, int *data_size, bool *eot, char *frame) {
    *eot = frame[1] == 0x1; // first byte is ISACK
    int ptr = 2;
    memcpy(sender_port, frame + ptr, 4);
    ptr += 4;

    // Receiver port
    ptr += 4;

    memcpy(seq_num, frame + ptr, 4);
    ptr += 4;

    memcpy(data_size, frame + ptr, 4);
    ptr += 4;

    memcpy(data, frame + ptr, *data_size);

    return false;
}

void create_ack(int sender_port, int seq_num, char *ack, bool error) {
    ack[0] = 0x1;
    ack[1] = error ? 0x1 : 0x0;
    int ptr = 2;
    uint32_t net_sender_port = sender_port;
    memcpy(ack + ptr, &net_sender_port, 4);
    ptr += 4;

    uint32_t net_receiver_port = RECEIVER_PORT;
    memcpy(ack + ptr, &net_receiver_port, 4);
    ptr += 4;

    uint32_t net_seq_num = seq_num;
    memcpy(ack + ptr, &net_seq_num, 4);
    ptr += 4;

    ack[ptr] = checksum(ack, ACK_SIZE - (int) 1);
    ack[ptr+1] = 0x0;
}

void send_ack() {
    char frame[MAX_FRAME_SIZE];
    char data[MAX_DATA_SIZE];
    char ack[ACK_SIZE];
    int frame_size;
    int data_size;
    socklen_t router_addr_size;
    
    int sender_port;
    int recv_seq_num;
    bool frame_error;
    bool eot;

    /* Listen for frames and send ack */
    while (true) {
        recvfrom(socket_fd, (char *)frame, MAX_FRAME_SIZE,
                MSG_WAITALL, (struct sockaddr *) &router_addr, 
                &router_addr_size);
        frame_error = read_frame(&sender_port, &recv_seq_num, data, &data_size, &eot, frame);

        create_ack(sender_port, recv_seq_num, ack, frame_error);
        int res = sendto(socket_fd, ack, ACK_SIZE, MSG_CONFIRM,
               (struct sockaddr *) &router_addr, sizeof(router_addr));
        if (res < 0)
            perror("send failed");
    }
}

int main(int argc, char * argv[]) {
    int window_len;
    int max_buffer_size;
    char *fname;

    if (argc == 4) {
        string file_str = RECEIVER_DIR + string(argv[1]);
        fname = strcpy(new char[file_str.length() + 1], file_str.c_str());        
        window_len = (int) atoi(argv[2]);
        max_buffer_size = MAX_DATA_SIZE * (int) atoi(argv[3]);
    } else {
        logger << argc << endl;
        logger << "usage: receiver <filename> <window_size> <buffer_size>" << endl;
        return 1;
    }

    memset(&receiver_addr, 0, sizeof(receiver_addr)); 
    memset(&router_addr, 0, sizeof(router_addr)); 
      
    /* Fill server address data structure */
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_addr.s_addr = INADDR_ANY; 
    receiver_addr.sin_port = htons(RECEIVER_PORT);

    router_addr.sin_family = AF_INET;
    router_addr.sin_addr.s_addr = INADDR_ANY; 
    router_addr.sin_port = htons(ROUTER_PORT);

    /* Create socket file descriptor */ 
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        logger << "socket creation failed" << endl;
        return 1;
    }

    /* Bind socket to server address */
    if (::bind(socket_fd, (const struct sockaddr *)&receiver_addr, 
            sizeof(receiver_addr)) < 0) { 
        perror("socket binding failed");
        return 1;
    }

    FILE *file = fopen(fname, "wb");
    char buffer[max_buffer_size];
    int buffer_size;

    /* Initialize sliding window variables */
    char frame[MAX_FRAME_SIZE];
    char data[MAX_DATA_SIZE];
    char ack[ACK_SIZE];
    int frame_size;
    int data_size;
    int lfr, laf;
    int sender_port;
    int recv_seq_num;
    bool eot;
    bool frame_error;

    /* Receive frames until EOT */
    bool recv_done = false;
    int buffer_num = 0;
    while (!recv_done) {
        buffer_size = max_buffer_size;
        memset(buffer, 0, buffer_size);
    
        int recv_seq_count = (int) max_buffer_size / MAX_DATA_SIZE;
        bool window_recv_mask[window_len];
        for (int i = 0; i < window_len; i++) {
            window_recv_mask[i] = false;
        }
        lfr = -1;
        laf = lfr + window_len;
        
        /* Receive current buffer with sliding window */
        while (true) {
            socklen_t router_addr_size;
            recvfrom(socket_fd, (char *) frame, MAX_FRAME_SIZE,
                    MSG_WAITALL, (struct sockaddr *) &router_addr, 
                    &router_addr_size);
            frame_error = read_frame(&sender_port, &recv_seq_num, data, &data_size, &eot, frame);

            logger<< "Received frame " << recv_seq_num << endl;

            create_ack(sender_port, recv_seq_num, ack, frame_error);
            int res = sendto(socket_fd, ack, ACK_SIZE, MSG_CONFIRM,
                             (struct sockaddr *) &router_addr, sizeof(router_addr) );
            if (res < 0)
                perror("send failed");
            else
                logger<< "Sent ACK of " << recv_seq_num << " to " << ntohs(router_addr.sin_port) << endl;

            if (recv_seq_num <= laf) {
                if (!frame_error) {
                    int buffer_shift = recv_seq_num * MAX_DATA_SIZE;

                    if (recv_seq_num == lfr + 1) {
                        memcpy(buffer + buffer_shift, data, data_size);

                        int shift = 1;
                        for (int i = 1; i < window_len; i++) {
                            if (!window_recv_mask[i]) 
                                break;
                            shift += 1;
                        }
                        for (int i = 0; i < window_len - shift; i++) {
                            window_recv_mask[i] = window_recv_mask[i + shift];
                        }
                        for (int i = window_len - shift; i < window_len; i++) {
                            window_recv_mask[i] = false;
                        }
                        lfr += shift;
                        laf = lfr + window_len;
                    } else if (recv_seq_num > lfr + 1) {
                        if (!window_recv_mask[recv_seq_num - (lfr + 1)]) {
                            memcpy(buffer + buffer_shift, data, data_size);
                            window_recv_mask[recv_seq_num - (lfr + 1)] = true;
                        }
                    }

                    /* Set max sequence to sequence of frame with EOT */ 
                    if (eot) {
                        buffer_size = buffer_shift + data_size;
                        recv_seq_count = recv_seq_num + 1;
                        recv_done = true;
                    }
                }
            }
            
            /* Move to next buffer if all frames in current buffer has been received */
            if (lfr >= recv_seq_count - 1) break;
        }

        logger<< "\r" << "[RECEIVED " << (unsigned long long) buffer_num * (unsigned long long) 
                max_buffer_size + (unsigned long long) buffer_size << " BYTES]" << flush;
        fwrite(buffer, 1, buffer_size, file);
        buffer_num += 1;
        break;
    }

    fclose(file);

    /* Start thread to keep sending requested ack to sender for 3 seconds */
    thread stdby_thread(send_ack);
    stdby_thread.join();

    return 0;
}
