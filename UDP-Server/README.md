# How to Run

For single sender and receiver<br>
`$ ./receiver <filename> <window_size> <buffer_size> <port>`<br>
`$ ./sender <filename> <window_size> <buffer_size> <ip> <port>`<br>

For multiple senders and a receiver<br>
`$ ./receiver <filename> <window_size> <buffer_size> <port>`<br>
todo: code the shell script

# How it Works
## Sender
## Receiver
## Router
<p>
    The router is a simple UDP server that receives packets from the sender and forwards them to the receiver. In return, the receiver sends an ACK message which is then forwarded to the sender.<br>
    The router has a buffer that stores the packets that are received from the senders/receiver.<br>
    ACK messages and packets are distinguished by the first byte of each message.<br>
    If the first (ISACK) byte is 0, it's a packet, otherwise it's an ACK message.<br>
    If the message is an ACK message, it'll be forwarded to the sender with the id that's stored in the message.<br>
    Otherwise, it'll be forwarded to the receiver.<br>
    TODO: explain RED
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
            - TODO<br>
    - Determine which sender must receive the ACK<br>
        - Assign an id to each sender.<br>
        - Append the id to the start of the frame.<br>
        - Now, read frame and extract the id as well as seq num.<br>
    - Send ACK to sender<br>
        - Use send()<br>
        - Check the message on the queue and just send it to the id that's stored in the message.<br>
