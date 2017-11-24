#include "event_base.h"
#include <sys/epoll.h>
#include <ext/hash_map>

#define MAXEVENTS 10

class event_loop
{
public:
	event_loop();
	~event_loop();

	// operqtor for IO event
	void add_ioev(int fd, io_callback* cb, int mask, void* args = NULL);

	// del the io mask from fd 
	void del_ioev(int fd, int mask);

	// de the io event from epoll and _io_evs by fd
	void del_ioev(int fd);

	// start epoll
	void process_evs();

private:
	int _epfd;	//epoll socket
	struct epoll_event _fired_evs[MAXEVENTS];
	__gnu_cxx::hash_map<int, io_event> _io_evs;
	typedef __gnu_cxx::hash_map<int, io_event>::iterator _ioevs_it;
	static event_loop* _thisLoopS;
};