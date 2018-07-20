# tcpip_limitorder_book
Implementation of "Top of Order Limit Book" used in shared market using C++ and TCP/IP client server communication 

This project is an implementation of "Top of Order Limit Book" used in share market. The project uses TCP/IP client-server 
communication model to send market data to the server and for sending updated data from the server to all the connected clients.

External application can be connected to the server to send share market message, the server builds top of the order limit book 
and sends the updated "best bid" and "best ask" rates to all the connected clients. 

The server also creates a shared memory segment and writes the updated data to it so that other processes can read the updated data.


=> Makefile could be used to build the server(main_server is the executable to run the server)

=> Client can be built separately using "g++ -o client client.cpp"

=> The server can handle multiple clients at the same time(configure max_clients variable in server.cpp as per the requirement)

C++ Standard Template Library's containers such as multimap and vectors are incorporated to build the top of order limit book
and to store the updates respectively. Stack could be a better option to store updates instead of vector! 


The project was built on Ubuntu 16.04 and GCC 7.2.
If connecting to the server using PUTTY, use your local ip address, 1500 as the port number and 115200 baud rate.
