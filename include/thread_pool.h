#include "thread_queue.h"
#include <pthread.h>

extern void* thread_domain(void* args);

class thread_pool
{
public:
	// create thread pool
	thread_pool(int thread_cnt);
	// ~thread_pool();	

	// 
	thread_queue<queue_msg>* get_next_queue();

private:
	int _cur_index;
	int _thread_cnt;
	thread_queue<queue_msg>** _pool;
	pthread_t* _tids;
};