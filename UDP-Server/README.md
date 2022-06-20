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