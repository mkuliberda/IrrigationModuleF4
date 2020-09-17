/*
 * schedule.h
 *
 *  Created on: 05.09.2020
 *      Author: Mati
 */

#ifndef SCHEDULE_H_
#define SCHEDULE_H_

#include <vector>
#include <string>
#include <time.h>

typedef struct {
	uint8_t weekday;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint8_t day;
	uint8_t month;
	uint8_t year;
}TimeStamp_t;

struct period_s{
	uint8_t day;
	uint8_t month;
	uint8_t year;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
};

struct time_s{
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
};

struct activity_s{
	period_s begin;
	period_s end;
	uint8_t weekday;
	time_s start;
	time_s duration;
};

struct exception_s{
	period_s begin;
	period_s end;
};

class Schedule{

private:
	std::vector<activity_s> 				vActivities;
	std::vector<exception_s> 				vExceptions;
	std::string 							name;
	bool									is_active;
	bool 									compare_time(const time_t &_time1, const time_t &_time2);


public:
	Schedule(const std::string &_name):
	name(_name),
	is_active(false)
	{};

	bool& 									isActive(const TimeStamp_t &_timestamp);
	bool									addActivity(const struct activity_s &_activity);
	bool									addActivity(const char *_activity);
	bool									addException(const struct exception_s &_exception);
	bool									addException(const char *_exception);
	bool									addLine(const char *_line);
};



#endif /* SCHEDULE_H_ */
