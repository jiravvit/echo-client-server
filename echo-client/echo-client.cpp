#include <iostream>
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

#ifdef WIN32
void perror(const char* msg) { fprintf(stderr, "%s %ld\n", msg, GetLastError()); }
#endif // WIN32

using namespace std;

void usage()
{
	cout << "echo-client:\n";
	cout << "syntax : echo-client <ip> <port>\n";
	cout << "sample : echo-client 192.168.10.2 1234\n";
}


struct Param {
	struct in_addr ip;
	uint16_t port{0};

	bool parse(int argc, char* argv[])
   	{
		for (int i = 1; i < argc; i++) 
		{
			int res = inet_pton(AF_INET, argv[i++], &ip);
			switch (res) 
			{
				case 1: break;
				case 0: fprintf(stderr, "not a valid network address\n"); return false;
				case -1: perror("inet_pton"); return false;
			}
			port = atoi(argv[i++]);
		}
		return (ip.s_addr != 0) && (port != 0);
	}
} param;


int main(int argc, char* argv[])
{
	if (!param.parse(argc, argv))
	{
		usage();
		return -1;
	}

	return 0;
}
