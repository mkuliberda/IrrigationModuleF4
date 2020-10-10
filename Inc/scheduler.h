/*
 * scheduler.h
 *
 *  Created on: 05.09.2020
 *      Author: Mati
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

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
	uint8_t id;
	uint8_t weekday;
	period_s begin;
	period_s end;
	time_s start;
	time_s duration;
};

struct exception_s{
	uint8_t id;
	period_s begin;
	period_s end;
};

class Scheduler{

private:
	std::vector<activity_s> 				vActivities;
	std::vector<exception_s> 				vExceptions;
	const std::string						name;
	bool									is_active;
	bool									is_available;
	const uint8_t							activities_limit;
	const uint8_t							exceptions_limit;
	bool 									compare_time(const time_t &_time1, const time_t &_time2);


public:
	Scheduler(const std::string &_name, uint8_t _activities_limit = 255, uint8_t _exceptions_limit = 40):
	name(_name),
	is_active(false),
	is_available(false),
	activities_limit(_activities_limit),
	exceptions_limit(_exceptions_limit)
	{};

	bool& 									update(const TimeStamp_t &_timestamp);
	bool& 									isActive(void);
	bool 									isExceptionPeriod(const TimeStamp_t &_timestamp);
	bool& 									isAvailable(void);
	void									setAvailable(void);
	bool									addActivity(const struct activity_s &_activity);
	bool									addActivity(const char *_activity);
	bool									addException(const struct exception_s &_exception);
	bool									addException(const char *_exception);
	bool									addLine(const char *_line);
	std::vector<activity_s>&				getActivities(void);
	std::vector<exception_s>&				getExceptions(void);
	const uint8_t							getActivitiesCount(void);
	const uint8_t							getExceptionsCount(void);
	activity_s								parseActivity(const char *_line);
	exception_s								parseException(const char *_line);
};



#endif /* SCHEDULER_H_ */
