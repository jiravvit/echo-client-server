#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef __linux__
#include <arpa/inet.h>
#include <sys/socket.h>
#endif // __linux
#ifdef WIN32
#include <winsock2.h>
#include "../mingw_net.h"
#endif // WIN32
#include <thread>
#include <vector> // std::vector
#include <algorithm> // std::algorithm::find

#ifdef WIN32
void perror(const char* msg) { fprintf(stderr, "%s %ld\n", msg, GetLastError()); }
#endif // WIN32

std::vector<int> clients; // for broadcast

void usage() {
	printf("syntax: echo-server <port> [-e[-b]]\n");
	printf("  -e : echo\n");
	printf("  -b : broadcast\n");
	printf("sample: echo-server 1234 -e -b\n");
}

struct Param {
	bool echo{false};
	bool broadcast{false};
	uint16_t port{0};

	bool parse(int argc, char* argv[]) {
		for (int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "-e") == 0) {
				echo = true;
				continue;
			}
			if (strcmp(argv[i], "-b") == 0) { // is it broadcast?
				broadcast = true;
				continue;
			}
			port = atoi(argv[i]);
		}
		return port != 0;
	}
} param;

void recvThread(int sd) {
	printf("connected\n");
	static const int BUFSIZE = 65536;
	char buf[BUFSIZE];
	while (true) {
		/*
		 * Receive data from Client
		*/ 
		ssize_t res = ::recv(sd, buf, BUFSIZE - 1, 0);
		if (res == 0 || res == -1) {
			fprintf(stderr, "recv return %ld", res);
			perror(" ");
			break;
		}
		buf[res] = '\0'; 	// last
		printf("%s", buf); 	// print!!
		fflush(stdout); 	// empty
		if (param.echo) {
			if (param.broadcast) {
				for (int sd : clients) {	// for broadcast
					/*
					 * Send data to Client
					*/ 
					res = ::send(sd, buf, res, 0);
					if (res == 0 || res == -1) {
						fprintf(stderr, "send return %ld", res);
						perror(" ");
						break;	
					}
				}
			}
			else {
				/*
				 * Send data to Client
				*/
				res = ::send(sd, buf, res, 0);
				if (res == 0 || res == -1) {
					fprintf(stderr, "send return %ld", res);
					perror(" ");
					break;
				}
			}
		}
	}
	printf("disconnected\n");
	auto it = find(clients.begin(), clients.end(), sd);
	clients.erase(it);	// erase iterator
	clients.clear();	// clear vector
	::close(sd); // close client socket
}

int main(int argc, char* argv[]) {
	// if the input format does not match, it is returned
	if (!param.parse(argc, argv)) { 
		usage();
		return -1;
	}

#ifdef WIN32
	WSAData wsaData;
	WSAStartup(0x0202, &wsaData);
#endif // WIN32

	/* 
	 * Make socket
	 * int socket(int domain, int type, int protocol)  
	 * AF_INET : in IPv4
	 * SOCK_STREAM : TCP
	 * 0 : (it is determined by the type)
	*/
	int sd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		perror("socket");
		return -1;
	}

	int res;
#ifdef __linux__
	int optval = 1;
	res = ::setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (res == -1) {
		perror("setsockopt");
		return -1;
	}
#endif // __linux

	// Server IP
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(param.port);
	
	/*
	 * Bind socket and server information
	 * int bind(int sockfd, struct sockaddr *myaddr, socklen_t arrlen)
	*/
	ssize_t res2 = ::bind(sd, (struct sockaddr *)&addr, sizeof(addr));
	if (res2 == -1) {
		perror("bind");
		return -1;
	}

	/*
	 * When a request comes in from a client, it enters a waiting state so that it can be accepted
	 * int listen(int sockfd, int backlog)
	 */ 
	res = listen(sd, 5);
	if (res == -1) {
		perror("listen");
		return -1;
	}

	while (true) {
		// Accecp the connection when the request comes from the client
		struct sockaddr_in cli_addr;
		socklen_t len = sizeof(cli_addr);
		/*
		 * Client connection to server socket
		 * int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
		 */ 
		int cli_sd = ::accept(sd, (struct sockaddr *)&cli_addr, &len);
		if (cli_sd == -1) {
			perror("accept");
			break;
		}
		clients.push_back(cli_sd);	// for broadcast
		std::thread* t = new std::thread(recvThread, cli_sd);
		t->detach();
	}
	::close(sd); // close server socket
}
