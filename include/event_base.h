#include <stdint.h>
#include <stdio.h>

/**
 * authors    gxw
 * date       2017-11-23
 * brief      the base struct: i/o event and the time event
 */

class event_loop;
typedef void io_callback(event_loop* loop, int fd, void* args);
typedef void time_callback(event_loop* loop, void* usr_data);

/**
 * @brief the io event struct 
 */							
struct io_event
{
	io_event():_read_cb(NULL), _write_cb(NULL), _rcb_args(NULL), _wcb_args(NULL) {}
	int _mask;	//EPOLLIN EPOLLOUT
	io_callback* _read_cb;	//callback when EPOLLIN comming
	io_callback* _write_cb;	//callback when EPOLLOUT comming
	void* _rcb_args;	//arguments for read_cb
	void* _wcb_args;	//arguments for write_cb
};

/**
 * @brief  the time event struct
 */
struct time_event
{
	time_event(time_callback* cb, void* cb_data, uint64_t ts, uint32_t interval)
	:_cb(cb), _cb_data(cb_data), _ts(ts), _interval(interval) {}
	time_callback* _cb;
	void* _cb_data;
	uint64_t _ts;
	uint32_t _interval;
	uint32_t _time_id; 
};
