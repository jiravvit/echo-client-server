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
	cout << "echo-server:\n";
	cout << "syntax: echo-server <port> [-e[-b]]\n";
	cout << "sample: echo-server 1234 -e -b\n";
}

struct Param 
{
	bool echo{false};
	bool broadcast{false};
	uint16_t port{0};

	bool parse(int argc, char* argv[]) 
	{
		for (int i = 1; i < argc; i++) 
		{
			if (strcmp(argv[i], "-e") == 0) 
			{
				echo = true;
				continue;
			}
			if (strcmp(argv[i], "-b") == 0) 
			{
				broadcast = true;
				continue;
			}
			port = stoi(argv[i]);
		}
		return port != 0;
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

