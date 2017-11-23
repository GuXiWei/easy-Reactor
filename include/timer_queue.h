#include <vector>
#include <ext/hash_map>
#include "event_base.h"

/**
 * author     gxw
 * date       2017-11-22
 * finish	  2017-11-22
 * brief      manage the time_events by mini_heap （occurrence time）
 */
class timer_queue
{
public:
	timer_queue();
	~timer_queue();

	int add_time(time_event& te);

	void del_time(int time_id);

	int notifier() const { return _timerfd; }

	int size() const { return _count; }

	void get_time(std::vector<time_event>& fired_evs);

private:
	void reset_time();

	// heap operation
	void heap_add(time_event& te);
	void heap_del(int pos);
	void heap_pop();
	void heap_hold();


	std::vector<time_event> _time_event_list;
	typedef std::vector<time_event>::iterator _time_event_it;
	
	__gnu_cxx::hash_map<int, int> _pos;
	typedef __gnu_cxx::hash_map<int, int>::iterator _pos_it;

	int _count;
	int _next_time_id;
	int _timerfd;
	uint64_t _pioneer;
};