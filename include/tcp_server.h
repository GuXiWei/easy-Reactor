#include "thread_pool.h"
#include "tcp_conn.h"
#include <sys/socket.h>
#include <netinet/in.h>

class tcp_server
{
public:
	tcp_server(evens_loop* loop, const char* ip, uint16_t port);
	~tcp_server();

 	// accept the connect 
	void do_accept();

	// deal with error when connect
	bool handle_exception_conn();

	// del with normal when connect
	bool handle_normal_conn();

	static get_conn_num(int& cnt);

private:
	int _sockfd;
	int _reservfd;
	event_loop* _loop;
	thread_pool* _pool;
	struct sockaddr_in _connaddr;
	socklen_t _addrLen;
	bool _bKeepLive;

	static int _conns_size;
	static int _max_conns;
	static int _curr_conns;
	static pthread_mutex_t _mutex;

public:
	static tcp_conn** _conns;
};