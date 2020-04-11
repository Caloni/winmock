#define _CRT_SECURE_NO_WARNINGS
#include "client.h"

#include <winsock2.h>
#if INCL_WINSOCK_API_PROTOTYPES
#include <ws2tcpip.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <time.h>

#pragma comment(lib, "Ws2_32.lib")


struct CONNECTION
{
	SOCKET socket;
	const char* host;
	unsigned short port;
};

char* duplicate_string(const char* src)
{
	char* buf;

	if(!src)
		return NULL;

	buf = (char*)malloc(strlen(src) + 1);

	strcpy(buf, src);
	return buf;
}

void winmock_initialize()
{
	static int initialized = 0;

	if (!initialized)
	{
		WSADATA wsadata;
		WSAStartup(MAKEWORD(2, 2), &wsadata);
		initialized = 1;
	}
}




int winmock_connect(const char* host, short port, struct CONNECTION** connection)
{
	struct CONNECTION conn = { 0, host, port };
	struct sockaddr_in serv_addr;
	char port_string[20];
	struct addrinfo* addr_result = NULL, hints;
	int result;

	winmock_initialize();

	*connection = NULL;

	conn.socket = (SOCKET) socket(AF_INET, SOCK_STREAM, 0);
	if (conn.socket < 0)
	{
		printf("Can't create socket\n");
		return -1;
	}

	sprintf(port_string, "%d", port);
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	result = getaddrinfo(host, port_string, &hints, &addr_result);

	if (addr_result == NULL ) {
		printf("Can't resolve server name.\n");
		return -1;
	}
	serv_addr = *(struct sockaddr_in*) addr_result->ai_addr;
	freeaddrinfo(addr_result);

	if (connect(conn.socket,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	{
		printf("Error while trying to connect to server\n");
		return -1;
	}

	*connection = (struct CONNECTION*)calloc(1, sizeof(struct CONNECTION));
	**connection = conn;

	return 0;
}


int winmock_send(struct CONNECTION* connection, const void* buffer, unsigned int len) {
	int ret = send(connection->socket, (const char*)buffer, len, 0);

	if(ret <= 0)
		ret = -1;
	
	return ret;
}


int winmock_receive(struct CONNECTION* connection, void* buffer, int len, const unsigned* timeout_in_seconds) {
	int ret = 0;
	char* char_buffer = (char*)buffer;
	int received = 0;
	time_t start = 0;
	int time_left;

	fd_set recvset;
	struct timeval tv;

	if(timeout_in_seconds)
		start = time(NULL);

	while(received < len) {
		if(timeout_in_seconds) {
			time_left = *timeout_in_seconds - (int)(time(NULL) - start);

			if(time_left < 0)
				return -1;

			tv.tv_sec = *timeout_in_seconds;
			tv.tv_usec = 0;

			FD_ZERO(&recvset);
			FD_SET(connection->socket, &recvset);

			ret = select(0, &recvset, NULL, NULL, (timeout_in_seconds ? &tv : NULL));

			if(ret == SOCKET_ERROR) {
				printf("Error reading data from server. Server is down or there is a network problem.\n");
				return -1;
			}

			if(ret == 0)
				return -1;
		}
		
		ret = recv(connection->socket, char_buffer + received, len - received, 0);
		if(ret <= 0)
		{
			printf("Error reading data from server. Server is down or there is a network problem.\n");
			return -1;
		}

		received += ret;
	}
		
	return received;
}

