#include "socket_apis.h"
#include <stdio.h>
#include <time.h>

#define _PORT_NUMBER "8080"

/**
* @brief Extension of 2_socket_apis.c to enable dual support
* of IPv4 and IPv6 addresses
*/
int main()
{
	WSADATA d;

	// initializing Winsock sockets
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		fprintf(stderr, "Failed to initialize.\n");
		return -1;
	}

	printf("Configuring local address...\n");
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6; // ipv6
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo* bind_address;
	getaddrinfo(0, _PORT_NUMBER, &hints, &bind_address);

	printf("Creating socket...\n");
	SOCKET socket_listen;
	socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
	if (!ISVALIDSOCKET(socket_listen))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
		system("pause");
	}

	// Support for both IPv6 and IPv4
	int option = 0;
	if (setsockopt(socket_listen, IPPROTO_IPV6, IPV6_V6ONLY, (void*)&option, sizeof(option)))
	{
		fprintf(stderr, "setsocketopt() failed. (%d)\n", GETSOCKETERRNO());
		system("pause");
		return 1;
	}

	printf("Binding socket to local address.\n");
	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
		system("pause");
	}
	freeaddrinfo(bind_address);

	printf("Listening....\n");
	if (listen(socket_listen, 10) < 0)
	{
		fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
		system("pause");
	}

	printf("Waiting for a connnection...\n");
	struct sockaddr_storage client_address;
	socklen_t client_len = sizeof(client_address);
	SOCKET socket_client = accept(socket_listen, (struct sockaddr*)&client_address, &client_len);
	if (!ISVALIDSOCKET(socket_client))
	{
		fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
		system("pause");
		return 1;
	}

	printf("Client is connected...\n");
	char address_buf[100];
	getnameinfo((struct sockaddr*)&client_address, client_len, address_buf,
		sizeof(address_buf), NULL, 0, NI_NUMERICHOST);
	printf("Client address: %s\n", address_buf);

	printf("Reading request...\n");
	const char* response =
		"HTTP/1.1 200 Ok\r\n"
		"Connection: close\r\n"
		"Content-Type: text/html\r\n\r\n"
		"<p style='color: orange'>Local time is: ";
	int bytes_sent = send(socket_client, response, strlen(response), 0);
	printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(response));

	time_t now;
	time(&now);
	char* time_msg = ctime(&now);
	bytes_sent = send(socket_client, time_msg, strlen(time_msg), 0);
	printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));

	printf("Closing connection...\n");
	closesocket(socket_client);
	
	printf("Closing listening socket...\n");
	closesocket(socket_listen);

	// clean up
	WSACleanup();
	printf("Done...\n");
	system("pause");

	return 0;
}
