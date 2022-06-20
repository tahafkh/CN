#ifndef DEFS_HPP
#define DEFS_HPP

// helper libraries
#include <chrono>
#include <stdlib.h> 
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>

// connection libraries
#include <iostream>
#include <thread>
#include <mutex>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#include <queue>

#define MAX_DATA_SIZE 1024
#define MAX_FRAME_SIZE 1034
#define ACK_SIZE 6

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

char checksum(char *frame, int count);
// int create_frame(int seq_num, char *frame, char *data, int data_size, bool eot);
// void create_ack(int seq_num, char *ack, bool error);
// bool read_frame(int *seq_num, char *data, int *data_size, bool *eot, char *frame);
// bool read_ack(int *seq_num, bool *error, char *ack);

#endif // DEFS_HPP
