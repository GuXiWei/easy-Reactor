#include <queue>
#include <pthread.h>
#include <sys/eventfd.h>
#include "event_loop.h"
#include "print_error.h"

template <typename T>

/**
 * @brief      first the thread create a eventfd to commicate with
 *             mian process.and the thread add the _evfd to the event_loop,
 *             also every thread has own event_loop.the mask of _evfd is 
 *             EPOLLIN, means if _evfd get the message it will execute the cb.
 *             
 */
class thread_queue
{
public:
	thread_queue(): _loop(NULL)
	{
		::pthread_mutex_init(&_mutex, NULL);
		_evfd = ::eventfd(0, EFD_NONBLOCK);
		exit_if(_evfd == -1, "eventfd()");
	}

	~thread_queue()
	{
		::pthread_mutex_destroy(&_mutex);
		::close(_evfd);
	}

	void send_msg (const T& msg)
	{
		unsigned long long number = 1;
		::pthread_mutex_lock(&_mutex);
		_queue.push(msg);
		int ret = ::write(_evfd, &number, sizeof(unsigned long long));
		error_if(ret == -1, "evenfd write");
		::pthread_mutex_unlock(&_mutex);
	}

	void recv_msg(std::queue<T>& tmp_queue)
	{
		unsigned long long number;
		::pthread_mutex_lock(&_mutex);
		int ret = ::read(_evfd, &number, sizeof(unsigned long long));
		error_if(ret == -1, "eventfd read");
		std::swap(tmp_queue, _queue);
		::pthread_mutex_unlock(&_mutex);
	}

	// set loop and install message coming event's cb
	void set_loop(event_loop* loop, io_callback* cb, void* args = NULL)
	{
		_loop = loop;
		_loop->add_ioev(_evfd, cb, EPOLLIN, args);
	}


	event_loop* get_loop () const { return _loop };
private:
	// the connection between the this(thread) and the main process
	int _evfd;
	// 
	event_loop* _loop;
	std::queue<T> _queue;
	pthread_mutex_t _mutex;
};
