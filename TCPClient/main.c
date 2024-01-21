
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
#include<stdlib.h>
#include<time.h>
#include<string.h>

int main(int argc, char* argv[]) {
	/*Steps for creating a TCP CLient
		Getting address by getAddressInfo()
		Creating a socket
		Calling connect() to bind the socket with the remote IP address
		Sending and recieving the data

	*/
	/*Initializing..*/
#if defined(_WIN32)
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d)) {
		fprintf(stderr, "Failed to initialize...\n");
		return 1;
	}
#endif
	/*We check that the IP address/hostname and the Port number is given as the argument from the client */
	/*The first argument is always our programs name, so at least there should be the presence of the three arguments in total*/
	if (argc < 3) {
		fprintf(stderr, "usuage: TCP client hostname and port..\n");
		return 1;
	}

	printf("Getting the remote address information");
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo* peer_address;

	if (GetAddrInfoA(argv[1], argv[2], &hints, &peer_address)) {
		fprintf(stderr, "GetAddInfo() failed %d..\n", GETSOCKETERRNO());
		return 1;
	}
	/*After Getting the address info we print the address info*/
	printf("Remote address is:");
	char address_buffer[1000];
	char service_buffer[1000];
	getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, address_buffer, sizeof(address_buffer), service_buffer, sizeof(service_buffer), NI_NUMERICHOST);
	printf("%s, %s\n", address_buffer, service_buffer);
	/*Now we can create our socket*/
	printf("Creating socket..");
	SOCKET socket_peer;
	socket_peer = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
	if (!ISVALIDSOCKET(socket_peer)) {
		fprintf(stderr, "Creating the socket failed %d \n", GETSOCKETERRNO());
		return 1;

	}

	/*Now we need to connect the socket with the remote address that is stored in the peer_address variable*/
	printf("Connecting.....");
	if (connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen)) {
		fprintf(stderr, "Error connecting to the socket..%d\n", GETSOCKETERRNO());
		return 1;
	}
	FreeAddrInfoA(peer_address);
	printf("Connected");
	/*Now we can start sending and recieving the data*/
	printf("To send data enter the text followed by the enter");
	/*AT this point we should loop to monitor the both socket and the terminal. If the socket is ready to be read we print the socket
	data to the terminal and if the input is detected to the terminal we present it to the socket*/
	while (1) {

		fd_set reads;
		FD_ZERO(&reads);
		FD_SET(socket_peer, &reads);
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;
		if (select(socket_peer + 1, &reads, 0, 0, &timeout) < 1) {
			fprintf(stderr, "select() failed..%d\n", GETSOCKETERRNO());
			return 1;

		}

		if (FD_ISSET(socket_peer, &reads)) {
			/*The socket is ready to be read so we just print the socket data into the terminal*/
			char read[4096];
			int bytes_recieved = recv(socket_peer, read, 4096, 0);
			if (bytes_recieved < 1) {
				printf("Connection closed by the peer");
				break;
			}
			printf("Recieved (%d bytes): %.*s", bytes_recieved, bytes_recieved, read);
		}
		/*Now checking for the terminal input*/
#if defined(_WIN32)
		if (_kbhit()) {
#else
		if (FD_ISSET(0, &reads)) {
			jhvjhvbjhvjhvj
#endif // defined(_WIN32)

			char read[4096];
			if (!gets(read, 4096, stdin)) break;
			printf("Sending %s\n", reads);
			int bytes_sent = send(socket_peer, read, strlen(read), 0);
			printf("Sent %d bytes.", bytes_sent);
		}
		printf("Closing socket...\n");
		CLOSESOCKET(socket_peer);
#if defined(_WIN32)
		WSACleanup();
#endif
		printf("Finished.\n");
		return 0;
	}
}