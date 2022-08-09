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
#include <fstream>
#include <thread>
#include <mutex>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

// utility libraries
#include <queue>
#include <map>
#include <math.h>

// IP constants
#define IP_PROTOCOL 0 
#define IP_ADDRESS "127.0.0.1"

#define ROUTER_PORT 8079
#define RECEIVER_PORT 8000

#define TOTAL_STATIONS 20+1

#define MAX_DATA_SIZE 1024

#define IS_ACK_INDEX 0

#define LOSS_RATE 10

// |1[IS_ACK]|1[EOT]|4[sender_port]|4[receiver_port]|4[seq_num]|4[data_size]|1024[data]|1[checksum]|
#define MAX_FRAME_SIZE 19+MAX_DATA_SIZE
#define EOT_INDEX 1

// |1[IS_ACK]|1[N/ACK]|4[sender_port]|4[receiver_port]|4[seq_num]|4[REQ_SEQ]|1[checksum]|
#define ACK_SIZE 19
#define NACK_INDEX 1

#define SEQ_INDEX 10

// time definitions
#define current_time chrono::high_resolution_clock::now
#define time_stamp chrono::high_resolution_clock::time_point
#define elapsed_time(end, start) chrono::duration_cast<chrono::milliseconds>(end - start).count()
#define sleep_for(x) this_thread::sleep_for(chrono::milliseconds(x));

// timeout threshold
#define TIMEOUT 50

// sender/receiver file directories
#define SENDER_DIR "sender_files/"
#define RECEIVER_DIR "receiver_files/"

typedef unsigned char byte;

/* Router RED constants */
const double wq = 0.8; // weight associated with the current router queue length
const double maxp = 0.5; // maximum drop probability
const double minth_coeff = 0.25; // min threshold coefficient
const double maxth_coeff = 0.75; // max threshold coefficient

/* utility functions */
char checksum(char *frame, int count);

#endif // DEFS_HPP
