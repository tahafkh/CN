#ifndef DEFS_HPP
#define DEFS_HPP

// helper libraries
#include <chrono>
#include <stdlib.h> 
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>

// main libraries
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

// utility libraries
#include <queue>
#include <map>
#include <math.h>

// IP constants
#define IP_PROTOCOL 0 
#define IP_ADDRESS "127.0.0.1"

#define ROUTER_PORT 4554

#define TOTAL_STATIONS 20+1

#define MAX_DATA_SIZE 1024
#define MAX_FILENAME_SIZE 32

#define IS_ACK_INDEX 0

#define LOSS_RATE 10

// |1[FROM_RECV]|4[sender_id]|4[sender_port]|4[file_name_size]|32[file_name]|1[check_sum]|1[is_frame_zero]|
#define FRAME_ZERO_SIZE 47
#define RECV_ID 0

// |1[IS_ACK]|1[EOT]|4[sender_id]|4[seq_num]|4[data_size]|1024[data]|1[checksum]|1[is_frame_zero]|
#define MAX_FRAME_SIZE 16+MAX_DATA_SIZE
#define EOT_INDEX 1

// |1[IS_ACK]|1[N/ACK]|4[sender_id]|4[seq_num]|1[checksum]|1[is_frame_zero]|
#define ACK_SIZE 12
#define NACK_INDEX 1

// time definitions
#define current_time chrono::high_resolution_clock::now
#define time_stamp chrono::high_resolution_clock::time_point
#define elapsed_time(end, start) chrono::duration_cast<chrono::milliseconds>(end - start).count()
#define sleep_for(x) this_thread::sleep_for(chrono::milliseconds(x));

// timeout threshold
#define TIMEOUT 10

#define STDBY_TIME 3000

// sender/receiver file directories
#define SENDER_DIR "sender_files/"
#define RECEIVER_DIR "receiver_files/"

typedef unsigned char byte;

/* Router RED constants */
const double wq = 0.002; // weight associated with the current router queue length
const double maxp = 0.5; // maximum drop probability
const double minth_coeff = 1/4; // min threshold coefficient
const double maxth_coeff = 3/4; // max threshold coefficient

/* utility functions */
char checksum(char *frame, int count);
int create_frame_zero(char *frame, char *filename, int sender_port, int sender_id=0);

#endif // DEFS_HPP
