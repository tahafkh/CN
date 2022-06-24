# How to Run

For single sender and receiver<br>
`$ ./receiver <filename> <window_size> <buffer_size> <port>`<br>
`$ ./sender <filename> <window_size> <buffer_size> <ip> <port>`<br>

For multiple senders and a receiver<br>
`$ ./receiver <filename> <window_size> <buffer_size> <port>`<br>
todo: code the shell script

# How it Works
## Sliding Window
<p>
    Sliding window is a protocol used instead of the Stop-and-Wait protocol. This method is more efficient than Stop-and-Wait since it sends packets without waiting for the previous packet to be acknowledged, ofcourse this depends on the sliding window size. The sliding window size is the number of packets that can be sent without waiting for the previous packet to be acknowledged. 
    There are two types of sliding windows:
        Selective Repeat (SR)
        Go-Back-N (GBN)
    In the Selective Repeat protocol, the receiver sends a packet and waits for an acknowledgment. If the acknowledgment is received, the receiver moves the sliding window forward. If the acknowledgment is not received, the receiver retransmits the packet. Only those frames are re-transmitted which are not acknowledged.
    In Go-Back-N Protocol, if the sent frame are not acknowledged then all the frames are re-transmitted from the lost packet to the last packet transmitted.
        
</p>
<li> Go-Back-N is simpler than SR.
<li> In selective Repeat protocol, receiver side needs sorting to sort the frames but in GBN, there's no need for sorting.
<li> In Go-Back-N Protocol, Out-of-Order packets are NOT Accepted (discarded) and the entire window is re-transmitted but in SR, these packets are accepted.
<li> GBN uses less memory than SR.
<li> Both are as efficient, but Selective Repeat is far better than Go back N in terms of retransmissions and bandwidth required.
    
## Sender
<p>
    The sender sends the file to the receiver through the router.
    The sender uses sliding window selective repeat protocol to send the file.
    The file is divided into chunks of size MAX_DATA_SIZE. Then, each chunk is appended to the header of the frame.
    The frame is formatted as follows:
        |1[IS_ACK]|1[EOT]|4[sender_id]|4[seq_num]|4[data_size]|1024[data]|1[checksum]|

    The frame is then sent to the router.
    After all the file is sent, the sender will send an EOT frame to the router.
    While the sender sends the frames, it'll also receive ACK messages from the receiver.
    These messages are received by the thread recv_thread.
</p>
## Receiver
<p>
    The receiver receives the file from the sender through the router.
    The receiver also uses sliding window selective repeat protocol to receive the file.
    The receiver receives the frames from the sender and checks the checksum.
    If the checksum is correct, the frame is appended to the file and an ACK is sent back.
    Otherwise, the frame is discarded and a NACK is sent back.
    After all the file is recieved, which is determined by the EOT frame, the receiver will wait for 3 seconds to send all the remaining ACKs to sender.
    The ACK message is also formatted as follows:
        |1[IS_ACK]|1[N/ACK]|4[sender_id]|4[seq_num]|1[checksum]|
</p>
## Router
<p>
    The router is a simple UDP server that receives packets from the sender and forwards them to the receiver. In return, the receiver sends an ACK message which is then forwarded to the sender.<br>
    The router needs a forwarding table in order to forward messages from senders to receiver vice-versa.<br>
    The forwarding table is filled using fram_zero messages:<br>
        These messages are the first message that each station sends to the router.
        The frame format is as follows:<br>
            |1[FROM_RECV]|4[sender_id]|4[sender_port]|4[file_name_size]|32[file_name]|1[check_sum]|1[is_frame_zero]|
        "FROM_RECV" indicates wether the frame is from a sender or a receiver.
        "sender_id" is used for the forwarding table map.
        "sender_port" is not needed.
        "file_name_size" and "file_name" are used to make a file for the receiver so that it can write the recieved data on the correct file.
    The router has a buffer that stores the packets that are received from the senders/receiver.<br>
    ACK messages and packets are distinguished by the first byte of each message.<br>
    If the first (ISACK) byte is 0, it's a packet, otherwise it's an ACK message.<br>
    If the message is an ACK message, it'll be forwarded to the sender with the id that's stored in the message.<br>
    Otherwise, it'll be forwarded to the receiver.<br>
</p>

## RED Implementation
<p>
    RED is among the first Active Queue Management (AQM) algorithms. The main goals of RED are:<br>

    Avoid congestion: Passive Queue Management suffers from this problem. AQM detects the congestion beforehand and avoids it.

    Avoid the problem of global synchronization: Passive Queue Management suffers from this problem. AQM avoids this problem.

    Avoid the problem of lockout: The short-lived flows suffer the problem of lockout when long-lasting flows occupy the buffers of routers all the time. AQM solves this problem, now short-lived flows can get space in the buffer.

    Maximize the ‘Power’ function, which is the ratio of throughput to delay: AQM handles the buffer queue properly, therefore there will no packet loss, as a result, there will be no retransmission of packets. The flow will be fluent and the delay will be minimum.
</p>
<p>
    RED is implmeneted inside the router. RED is invoked whenever a new message arrives. The router will check the buffer and see if there is enough space to store the message. If there is, the message will be stored in the buffer. If there is not, the message will be dropped. If enqueuing the packet increases overall delay or makes the queue full then it will cause the congestion to occur. Ideally, if RED drops the packet before the queue is full then the sender will know about it indirectly and it will reduce its congestion window size and thus it avoids the congestion to happen in the future.

    The RED algorithm first calculates the average queue length. The average queue length is calculated as follows:
        1. On arrival of every message, RED calculates the average queue length using the Exponential Weighted Moving Average.
        2. The calculation is implemented inside the function 'get_avg_queue_len'.
    Next, a drop probability is calculated. The drop probability is calculated as follows:
        1. If the new average is less than the minimum threshold for ‘average queue length’ the packet is pushed to the end of the queue.
        2. Otherwise, if the new average is more than the maximum threshold for ‘average queue length’ the packet is dropped.
        3. Finally, a probability of drop is calculated which is implemented in function 'calc_pd_pa'.
    Finally, a decision-making logic is designed to decide whether to drop the packet or not. The decision-making logic is implemented in function 'red_check_queue'. It uses the probability calculated in the previous stage and a uniformly distributed random number generated between [0, 1].
</p>

# Problems

1. Router Connection Setup<br>
    - How should the router sockets behave?<br>
        - Should it be able to connect to multiple senders and/or receivers?<br>
            - Yes<br>
        - There is one main socket i.e. server socket (socket_fd)<br>
        - The stations connect to this socket and communicate through this socket<br>
    - How can the senders know where router is?<br>
        - They know the router's IP and port<br>
    - How can receiver know?<br>
        - Just like the senders<br>
    

2. Router<br>
    - Connect multiple senders<br>
        - Simply used an fd_set.<br>
    - Send to receiver<br>
        - Use send()<br>
        - The problem is how to distinguish the reciever from others<br>
            - Simply don't distinguish between them and just use the IS_ACK byte<br>
            - TODO (also find a way to assign IDs)<br>
    - Determine which sender must receive the ACK<br>
        - Assign an id to each sender.<br>
        - Append the id to the start of the frame.<br>
        - Now, read frame and extract the id as well as seq num.<br>
    - Send ACK to sender<br>
        - Use send()<br>
        - Check the message on the queue and just send it to the id that's stored in the message.<br>
