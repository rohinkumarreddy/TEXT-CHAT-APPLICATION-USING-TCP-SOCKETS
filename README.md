# TEXT-CHAT-APPLICATION-USING-TCP-SOCKETS
The project includes a server and can support upto 4 clients at a time. Features: CLIENT • We make use of TCP sockets to establish connections among the clients and the server. A client when logged in to the server with help of server's IP address and port number establishes a TCP connection. • The client then sends it's details such as IP address, port number and host name to the server upon reception of which the server sends the updated list of clients along with their details to the new client. • The client can now access the list of other clients online and can communicate with them anytime by sending a message to their corresponding IP via the server. If the recipient of the message is offline then the server buffers the message and forwards them when the recipient logs in. • A client can also broadcast a message to all the clients logged in even if it doesn't know that a particular client is not logged in. • At any point of time a client can logout and login again. Also a client can receive an updated list of clients from the server by refreshing it's list. • A client can block/unblock any other client at any point of time without the other client noticing it. • Finally, a client can terminate/quit by exiting, which terminates the client program with a '0' return value. SERVER • A server on the other hand can access the list of all the clients currently logged in and at the same time can also access the statistics of all the clients that have logged in at some point of time and may have logged out but have not exited. • Also a server can access a list of clients blocked by a client • The server forwards all the messages from a client to another client and there is no peer to peer communication here.