#include "socket_apis.h"
#include <time.h>
#include <string.h>

// Winsock requires initialisation and cleanup as compared to POSIX sockets
// There are two types of sockets;
//	- Connection-oriented sockets 
//	- Connetionless sockets
// 
// Two protocals used today are;
//	- Transmission Control Protocal (TCP). It is a connection-oriented protocal
//	- User Datagram Protocal (UDP). It is a connectionless protocal
// 
// Some common socket functions
//	- socket()
//	- bind() - associates a socket created by socket() with a particular local IP address and port number
//	- listen()
//	- connect() (client side)
//	- accept()	(server side)
//	- send(), - recv()
//	- sendto(), - recvfrom()
//	- close() (for POSIX sockets), - closesocket() (for Winsock sockets)
//	- shutdown()
//	- select()
//	- getnameinfo(), - getaddrinfor()
//	- setsockopt()
//	- fcntl() (for POSIX sockets), - ioctlsocket() (for Winsock sockets)
//	- poll() and dup() will not be used for portability reasons
// 
// Network programs can be described as one of the four types
//	- a TCP server
//	- a UDP server
//	- a TCP client
//	- a UDP client
//

/**
* @brief Program description: A web server that tells the user what time it is.
* It uses the TCP protocal for communication between server and client
*/
int main()
{
	WSADATA d;

	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		fprintf(stderr, "Failed to initialize");
		system("pause");
		return -1;
	}
	printf("Configuring local address....\n");

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // ipv4
	hints.ai_socktype = SOCK_STREAM; // tcp (SOCK_DGRAM for udp)
	hints.ai_flags = AI_PASSIVE;	 // tells getaddrinfo that we want to
									 // listen on any available network interface
	struct addrinfo* bind_address;
	getaddrinfo(0, "8080", &hints, &bind_address); // configure local address info

	printf("Creating socket...\n");
	SOCKET socket_listen;
	socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
	if (!ISVALIDSOCKET(socket_listen)) // check if call to socket() was a success
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		system("pause");
		return 1;
	}

	// socketing was successful, next we bind the socket to the local addr
	printf("Binding socket to local address....\n");
	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
		system("pause");
		return 1;
	}
	freeaddrinfo(bind_address); // clean up after binding

	// binding was successful, finally we make the socket to start listening for any connections
	printf("Listening....\n");
	if (listen(socket_listen, 10) < 0)
	{
		fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
		system("pause");
		return 1;
	}
	
	// we start accepting incoming connections
	printf("Waiting for connection...\n");
	struct sockaddr_storage client_address;
	socklen_t client_len = sizeof(client_address);
	SOCKET socket_client = accept(socket_listen, (struct sockaddr*)&client_address, &client_len);
	if (!ISVALIDSOCKET(socket_client))
	{
		fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
		system("pause");
		return 1;
	}

	// display client's address after successful connection
	printf("Client is connected...\n");
	char address_buf[100];
	getnameinfo((struct sockaddr*)&client_address, client_len, address_buf,
		sizeof(address_buf), NULL, 0, NI_NUMERICHOST);
	printf("Client address: %s\n", address_buf);

	// we expect a HTTP request since this is a web server
	printf("Reading requesting...\n");
	char request[1024];
	int bytes_received = recv(socket_client, request, sizeof(request), 0);
	printf("Received %d bytes.\n", bytes_received);

	// sending response back to user
	printf("Sending response...\n");
	const char* response =
		"HTTP/1.1 200 Ok\r\n"
		"Connection: close\r\n"
		"Content-Type: text/html\r\n\r\n"
		"<p style='color: orange'>Local time is: ";
	int bytes_sent = send(socket_client, response, strlen(response), 0);
	printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(response));

	// sending the actual time to the user
	time_t now;
	time(&now);
	char* time_msg = ctime(&now);
	bytes_sent = send(socket_client, time_msg, strlen(time_msg), 0);
	printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));

	// close connection to indicate to browser that all of our data is sent
	printf("Closing connection...\n");
	closesocket(socket_client);

	// we close listening socket
	printf("Closing listening socket...\n");
	closesocket(socket_listen);

	// clean up
	WSACleanup();
	printf("Done...\n");
	system("pause");
	return 0;
}
