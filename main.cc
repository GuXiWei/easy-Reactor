#include "include/tcp_server.h"
#include <string>
#include <stdio.h>


int main()
{
	event_loop loop;
	std::string ip = "127.0.0.1";
	short port = 4012;

	tcp_server server(&loop, ip.c_str(), port);
	loop.process_evs();
}