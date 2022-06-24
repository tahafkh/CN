#include "../include/defs.hpp"

using namespace std;

char checksum(char *frame, int count) {
    u_long sum = 0;
    while (count--) {
        sum += *frame++;
        if (sum & 0xFFFF0000) {
            sum &= 0xFFFF;
            sum++; 
        }
    }
    return sum & (0xFFFF);
}

int create_frame_zero(char *frame, char *filename, int file_name_len, int sender_port, int sender_id=0) {
    frame[0] = !sender_id ? 0x0 : 0x1;
    int ptr = 1;
    uint32_t net_send_id = htonl(sender_id);
    memcpy(frame + ptr, &net_send_id, 4);
    ptr += 4;
    
    uint32_t net_send_port = htonl(sender_port);
    memcpy(frame + ptr, &net_send_port, 4);
    ptr += 4;
    
    uint32_t net_name_size = htonl(file_name_len);
    memcpy(frame + ptr, &net_name_size, 4);
    ptr += 4;
    
    memcpy(frame + ptr, filename, file_name_len);
    
    frame[file_name_len + ptr] = checksum(frame, file_name_len + ptr);
    frame[file_name_len + ptr+1] = 0x1;
    
    return file_name_len + ptr+2;
}
