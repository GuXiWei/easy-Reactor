#include "../include/thread_pool.h"

thread_pool::thread_pool(int thread_cnt): _thread_cnt(thread_cnt), _curr_index(0)
{
	// step 1: if illegal thread_cnt, exit
	exit_if(thread_cnt > 30 || thread_cnt <= 0, "error thread cnt");
	// step2: create thread_cnt thread_queues and threads.one thread_queue one thread.
	_pool = new thread_queue<queue_msg>* [thread_cnt];
	_tids = new pthread_t[thread_cnt];
	int ret;
	for (int i = 0; i < thread_cnt; ++i)
	{
		_pool[i] = new thread_queue<queue_msg>();
		ret = ::pthread_create(_tids[i], NULL, thread_domain, _pool[i]);	//when constract the thread_queue, create eventfd(communicate with main process)
		exit_if(ret == -1, "pthread_create");

		ret = ::pthread_detach(_tid[i]);
		error_if(ret == -1, "pthread_detach");
	}
}
thread_queue<queue_msg>* thread_pool::get_next_thread()
{
	if (_cur_index == _thread_cnt)
	{
		_cur_index = 0;
	}
	return _pool[_cur_index++];
}

// the thread recv the queue_msg from main process
void msg_coming_cb(event_loop* loop, int fd, void* args)
{
	thread_queue<queue_msg>* queue = (thread_queue<queue_msg>*)args;
	std::queue<queue_msg> msgs;
	// step 1: get all the queue_msg int thread_queue,and clear thread_queue'_queue
	queue->recv_msg(msgs);
	while(!msgs.empty())
	{
		queue_msg msg = msgs.front();
		msgs.pop();
		// step 2: if the msg type is a new onnect, then
		if (msg._cmdType == queue_msg::NEW_CONN)
		{
			/* code */
		}
	}
}

// the thread method
void* thread_domain(void *args)
{
	thread_queue<queue_msg>* queue = (thread_queue<queue_msg>*)args;
	event_loop* loop = new event_loop();    //when constract the loop, create epoll.
	queue->set_loop(loop, msg_coming_cb, queue);  //when the thread_queue's eventfd rev msg, call msg_coming_cb
	loop->process_evs(); // starting listening
}