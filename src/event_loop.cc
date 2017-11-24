#include "../include/event_loop.h"
#include "../include/print_error.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief      Constructs the event_loop.
 * date       2017-11-23
 * author     gxw
 * finish     ~~
 */

event_loop::event_loop()
{
	_epfd = ::epoll_create1(0);	//create _epoll
	exit_if(_epfd == -1, "when epoll_create1()");
}

event_loop::~event_loop()
{
	::close(_epfd);
}

/*
 * attention:
 * {
 * 		* if EPOLLIN in mask, EPOLLOUT must not in mask;
 *      * if EPOLLOUT in mask, EPOLLIN must not in mask;
 *      * if want to register EPOLLOUT | EPOLLIN event, just call add_ioev twice!
 * }
 * todo maybe wil change in the future
 */
void event_loop::add_ioev(int fd, io_callback* cb, int mask, void* args = NULL)
{
	int f_mask;
	int op;
	// step 1: where the fd is already in _io_evs
	_ioevs_it it = _io_evs.find(fd);
	// step 1-1: if not,the operation is add
	if (it = _io_evs.end())
	{
		f_mask = mask;
		op = EPOLL_CTL_ADD;
	}
	// step 1-2: if existing, the operation is mod
	else
	{
		f_mask = it->second._mask | mask;
		op = EPOLL_CTL_MOD;
	}

	// step 2: set the io_event callback function by the mask
	if (mask & EPOLLIN)
	{
		_io_evs[fd]._read_cb = cb;
		_io_evs[fd]._rcb_args = args;
	}
	else if (mask & EPOLLOUT)
	{
		_io_evs[fd]._write_cb = cb;
		_io_evs[fd]._wcb_args = args;
	}
	_io_evs[fd]._mask = f_mask;

	// step 3: cteate epoll_event for the fd
	struct epoll_event event;
	event.events = f_mask;
	event.data.fd = fd;
	int ret = ::epoll_ctl(_epfd, op, &event);
	error_if(ret == -1, "epoll_ctl()");
}

/**
 * @brief      { listening }
 */
void event_loop::process_evs()
{
	while(1)
	{
		_ioevs_it it;
		// step 1: wait for io event
		int nfds = ::epoll_wait(_epfd, _fired_evs, MAXEVENTS, 10);
		// step 2: traversal the events
		for(int i = 0; i < nfds; ++ i)
		{
			it = _io_evs.find(_fired_evs[i].data.fd);
			io_event* ev = it->second;
			// step 2-1: mask is EPOLLIN, then read
			if(ev->_mask & EPOLLIN)
			{
				void* args = ev->_rcb_args;
				ev->_read_cb(this, _fired_evs[i].data.fd, args);
			}
			// step 2-2: mask is EPOLLOUT, then write
			else if (ev->_mask & EPOLLOUT)
			{
				void*args = ev->_wcb_args;
				ev->_write_cb(this, _fired_evs[i].data.fd, args);
			}
		}
	}
}

void event_loop::del_ioev(int fd, int mask)
{
	// step 1: find fd event
	_ioevs_it it = _io_evs.find(fd);
	if (it == _io_evs.end())
	{
		return;
	}
	int& o_mask = it->second._mask;
	int ret;
    // step 2: remove mask from o_mask
	o_mask = o_mask & (~mask);

	// step 3-1: if o_mask == 0, del io event
	if (o_mask == 0)
	{
		_io_evs.erase(it);
		ret = ::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
		error_if(ret == -1, "epoll_ctl EPOLL_CTL_DEL");
	}
	// step 3-1: else mod the io event
	else
	{
		struct epoll_event event;
		event.events = o_mask;
		event.data.fd = fd;
		ret = ::epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &event);
		error_if(ret == -1, "epoll_ctl EPOLL_CTL_MOD");
	}
}

void event_loop::del_ioev(int fd)
{
	_ioevs_it it = _io_evs.find(fd);
	if (it == _io_evs.end())
	{
		return;
	}
	_io_evs.erase(it);
	int ret;
	ret = ::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
	error_if(ret == -1, "epoll_ctl EPOLL_CTL_DEL");
}
