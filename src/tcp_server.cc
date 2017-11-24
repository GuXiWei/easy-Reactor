#include "../include/tcp_server.h"

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <strings.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

void accepter_cb(event_loop* loop, int fd, void* arg)
{
	tcp_server* server = (tcp_server*)arg;
	server->do_accept();
}

tcp_conn** tcp_server::_conns = NULL;
int tcp_server::_conns_size = 0;
int tcp_server::_max_conns = 0;
int tcp_server::_curr_conns = 0;
pthread_mutex_t tcp_server::_mutex = PTHREAD_MUTEX_INITIALIZER;

tcp_server::tcp_server(evens_loop* loop, const char* ip, uint16_t port)
{
	memset(&_connaddr, 0, sizeof(_connaddr));

	// step 1: create socket bind and listening
	_sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	exit_if(_sockfd == -1, "socket()");
	struct sockaddr_in servaddr;
    ::bzero(&servaddr, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    int ret = ::inet_aton(ip, &servaddr.sin_addr);
    exit_if(ret == 0, "ip format %s", ip);
    servaddr.sin_port = htons(port);
    int opend = 1;
    ret = ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opend, sizeof(opend));
    error_if(ret < 0, "setsockopt SO_REUSEADDR");

    ret = ::bind(_sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr));
    exit_if(ret == -1, "bind()");

    ret = ::listen(_sockfd, 500);
    exit_if(ret == -1, "listen()");

    // step 2: create thread pool and conns
    int thread_cnt = 5;	// test 5 threads
    _max_conns = 1000;	// test the 1000 conns
    _pool = NULL;
    _pool = new thread_pool(thread_cnt);
    exit_if(_pool == NULL, "new thread_poll");
    _conns = new tcp_conn*[_max_conns];
    exit_if(_conns == NULL, "new tcp_conn");
    for (int i = 0; i < _max_conns; ++i)
    {
    	_conns[i] = NULL;
    }

    // add the _sockfd to the _loop
    // if a new connect, call accepter_cb
    _loop->add_ioev(_sockfd, accepter_cb, EPOLLIN, this);
}

bool tcp_server::handle_exception_conn()
{
    switch (errno)
    {
        case EINTR:
            return true;
        case EMFILE:
            return true;
        case EAGAIN:
            return false;
        default:    //unpredictable error, just exit
            exit_log("accept");
    }
}

bool tcp_server::handle_normal_conn(int& fd)
{
    thread_queue<queue_msg>* cq = _pool->get_next_queue();
    queue_msg msg;
    msg._cmdType = queue_msg::NEW_CONN;
    msg._connfd = fd;
    cq->send_msg(msg);
    return true;
}

void tcp_server::get_conn_num(int &cnt)
{
    ::pthread_mutex_lock(&_mutex);
    cnt = _curr_conns;
    ::pthread_mutex_unlock(&_mutex);
}

void tcp_server::inc_conn()
{
    ::pthread_mutex_lock(&_mutex);
    _curr_conns ++;
    ::pthread_mutex_unlock(&_mutex);
}

// the EPOLLIN of _sockfd
void tcp_server::do_accept()
{
    int connfd;
    bool conn_full = false;
    while(true)
    {
        // if no more access conns, return
        int cur_conns;
        get_conn_num(cur_conns);
        if (cur_conns >= _max_conns)
        {
            continue;
        }
        // connect
        connfd = ::accept(_sockfd, (struct sockaddr *)_connaddr, &_addrLen);
        //
        if (connfd == -1)
        {
            ::close(connfd);
            if(handle_exception_conn())
                continue;
            else
                break;
        }
        else
        {
            handle_normal_conn(connfd);
        }
    }
}