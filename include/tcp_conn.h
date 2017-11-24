#include "event_loop.h"
 
/**
 * details: create a tcp_conn for every connection between ser and client
 */

class tcp_conn
{
public:
	tcp_conn(int connfd, event_loop* loop) { init(connfd, loop); };
	~tcp_conn();

	/**
	 * @brief      init the tcp_conn
	 *
	 * @param[in]  connfd  server accept socket
	 * @param      loop    The loop
	 */
	void init(int connfd, event_loop* loop);

	//void handle_read();

	//void handle_write();

	//int send_data(const char* data, int dataLen, int cmdId);

private:
	int _connfd;
	event_loop* _loop;
};