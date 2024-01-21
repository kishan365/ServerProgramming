#pragma once
#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include<winsock2.h>
#include<ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#else
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<unistd.h>
#include<errno.h>
#endif

#if defined(_WIN32)
#define ISVALIDSOCKET(s) (s!=INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())

#else 
#define ISVALIDSOCCKET(s) ((s)>=0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int 
#define GETSOCKETERRNO() (errno)
#endif

#include<stdio.h>
#include<time.h>
#include<string.h>

int main() {
#if defined(_WIN32)
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d)) {
		fprintf(stderr, "Could not initialize");
		return 1;
	}
#endif
	/*Getting the iP address */
	printf("Configuring the address");
	struct addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *bind_address;
	
	getaddrinfo(0, "8080", &hints, &bind_address);
	/*Print the address brother*/
	printf("Getting the server address...");
	char address[1000];
	char service[1000];
	//GetNameInfo(bind_address->ai_addr, bind_address->ai_addrlen, address, sizeof(address),service,sizeof(service), NI_NUMERICHOST);



}