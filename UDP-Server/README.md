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
    If the byte is 0, it's a packet, otherwise it's an ACK message.<br>
    TODO: explain RED
</p>

# Problems

1. Router Connection Setup<br>
    - How should the router sockets behave?<br>
    - How can the senders know where router is?<br>
    - How can receiver know?<br>
    

2. Router<br>
    - Connect multiple senders<br>
    - Send to receiver<br>
    - Receive the ACK<br>
    - Determine which sender must receive the ACK<br>
        - Assign an id to each sender.<br>
        - Append the id to the start of the frame.<br>
        - Now, read frame and extract the id as well as seq num.<br>
    - Send ACK to sender<br>
