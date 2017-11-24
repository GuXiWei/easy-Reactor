#include "../include/tcp_conn.h"
#include "../include/tcp_server.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

static void tcp_rcb(event_loop* loop, int fd, void* args)
{
	tcp_conn* conn = (tcp_conn*)args;
	// just read and print
	char msg_buff[1024];
	int ret;
	ret = read(conn->_connfd, msg_buff, sizeof(msg_buff) - 1);
	error_if(ret == -1, "read");
	printf("%s\n", msg_buff);
}

void tcp_conn::init(int connfd, event_loop* loop)
{
	_connfd = connfd;
	_loop = loop;
	// set NONBLOCK
	int flag = ::fcntl(_connfd, F_GETFD, 0);
	::fcntl(_connfd, F_SETFD, O_NONBLOCK | flag);

	//set NODELAY
    int opend = 1;
    int ret = ::setsockopt(_connfd, IPPROTO_TCP, TCP_NODELAY, &opend, sizeof(opend));
    error_if(ret < 0, "setsockopt TCP_NODELAY");

    _loop->add_ioev(_connfd, tcp_rcb, EPOLLIN, this);

    tcp_server::inc_conn();
}