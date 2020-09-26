/*
 * Scheduler.cpp
 *
 *  Created on: 05.09.2020
 *      Author: Mati
 */

#include "scheduler.h"

bool Scheduler::compare_time(const time_t &_time1, const time_t &_time2){
	return difftime(_time1, _time2) > 0.0 ? true : false;
}

bool& Scheduler::isActive(const TimeStamp_t &_timestamp){

	for (auto const& activity_time: this->vActivities){
		if(activity_time.weekday == _timestamp.weekday){
			time_t now, period_begin, period_end;
			struct tm tm_timestamp;

			tm_timestamp.tm_hour = _timestamp.hours;
			tm_timestamp.tm_min = _timestamp.minutes;
			tm_timestamp.tm_sec = _timestamp.seconds;
			tm_timestamp.tm_mday = _timestamp.day;
			tm_timestamp.tm_mon = _timestamp.month - 1;
			tm_timestamp.tm_year = 100 + _timestamp.year;
			now = mktime(&tm_timestamp);

			tm_timestamp.tm_hour = activity_time.begin.hours;
			tm_timestamp.tm_min = activity_time.begin.minutes;
			tm_timestamp.tm_sec = activity_time.begin.seconds;
			tm_timestamp.tm_mday = activity_time.begin.day;
			tm_timestamp.tm_mon = activity_time.begin.month - 1;
			tm_timestamp.tm_year = 100 + activity_time.begin.year;
			period_begin = mktime(&tm_timestamp);

			tm_timestamp.tm_hour = activity_time.end.hours;
			tm_timestamp.tm_min = activity_time.end.minutes;
			tm_timestamp.tm_sec = activity_time.end.seconds;
			tm_timestamp.tm_mday = activity_time.end.day;
			tm_timestamp.tm_mon = activity_time.end.month - 1;
			tm_timestamp.tm_year = 100 + activity_time.end.year;
			period_end = mktime(&tm_timestamp);

			if(this->compare_time(now, period_begin) && this->compare_time(period_end, now)){
				time_t activity_start, activity_duration, activity_end;

				tm_timestamp.tm_mday = _timestamp.day;
				tm_timestamp.tm_mon = _timestamp.month - 1;
				tm_timestamp.tm_year = 100 + _timestamp.year;

				tm_timestamp.tm_hour = activity_time.start.hours;
				tm_timestamp.tm_min = activity_time.start.minutes;
				tm_timestamp.tm_sec = activity_time.start.seconds;
				activity_start = mktime(&tm_timestamp);

				tm_timestamp.tm_hour = activity_time.duration.hours;
				tm_timestamp.tm_min = activity_time.duration.minutes;
				tm_timestamp.tm_sec = activity_time.duration.seconds;
				mktime(&tm_timestamp);
				activity_duration = 3600 * tm_timestamp.tm_hour + 60 * tm_timestamp.tm_min + tm_timestamp.tm_sec;//mktime(&tm_timestamp);

				activity_end = activity_start + activity_duration;

				if(this->compare_time(now, activity_start) && this->compare_time(activity_end, now)){
					return this->is_active = true;
				}
			}
		}
	}

	return is_active = false;

}

bool& Scheduler::isAvailable(void){
	return this->is_available;
}

void Scheduler::setAvailable(void){
	this->is_available = true;
}

bool Scheduler::addActivity(const struct activity_s &_activity){
	if (this->vActivities.size() <= this->activities_limit)
	{
		this->vActivities.push_back(_activity);
		return true;
	}
	else
	{
		return false;
	}
}

//01234567891123456789212345678931234567894
/*A1:20-05-01,20-09-30,MON,06-00-00,00-10-00
A2:20-05-01,20-09-30,MON,19-00-00,00-10-00
A3:20-05-01,20-09-30,TUE,06-00-00,00-10-00
A4:20-05-01,20-09-30,TUE,19-00-00,00-10-00
A5:20-05-01,20-09-30,THU,06-00-00,00-10-00
A6:20-05-01,20-09-30,THU,20-00-00,00-10-00*/
bool Scheduler::addActivity(const char *_activity){
	const std::string str(_activity);
	if (str.length() == 42){
		//try:
		activity_s activity = {0,0,{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0},{0,0,0}};
		if (str.substr(0,1) == "A" && str.substr(2,1) == ":" && this->vActivities.size() <= this->activities_limit){
			activity.id = atoi(str.substr(1,1).c_str());
			activity.begin.year = atoi(str.substr(3,2).c_str());
			activity.begin.month = atoi(str.substr(6,2).c_str());
			activity.begin.day = atoi(str.substr(9,2).c_str());
			activity.end.year = atoi(str.substr(12,2).c_str());
			activity.end.month = atoi(str.substr(15,2).c_str());
			activity.end.day = atoi(str.substr(18,2).c_str());
			const std::string weekday_str = str.substr(21,3);
			if (weekday_str == "MON"){
				activity.weekday = ((uint8_t)0x01);
			}
			else if(weekday_str == "TUE"){
				activity.weekday = ((uint8_t)0x02);
			}
			else if(weekday_str == "WED"){
				activity.weekday = ((uint8_t)0x03);
			}
			else if(weekday_str == "THU"){
				activity.weekday = ((uint8_t)0x04);
			}
			else if(weekday_str == "FRI"){
				activity.weekday = ((uint8_t)0x05);
			}
			else if(weekday_str == "SAT"){
				activity.weekday = ((uint8_t)0x06);
			}
			else if(weekday_str == "SUN"){
				activity.weekday = ((uint8_t)0x07);
			}
			else{
				activity.weekday = ((uint8_t)0xFF);
			}
			activity.start.hours = atoi(str.substr(25,2).c_str());
			activity.start.minutes = atoi(str.substr(28,2).c_str());
			activity.start.seconds = atoi(str.substr(31,2).c_str());
			activity.duration.hours = atoi(str.substr(34,2).c_str());
			activity.duration.minutes = atoi(str.substr(37,2).c_str());
			activity.duration.seconds = atoi(str.substr(40,2).c_str());
			this->vActivities.push_back(activity);
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
}


bool Scheduler::addException(const struct exception_s &_exception){
	if (this->vExceptions.size() <= this->exceptions_limit)
	{
		this->vExceptions.push_back(_exception);
		return true;
	}
	else
	{
		return false;
	}

}

/*E1:20-05-15,00-00-01,20-05-15,16-00-00*/
bool Scheduler::addException(const char *_exception){
	const std::string str(_exception);
	if (str.length() == 39){
		//try:
		exception_s exception = {0,{0,0,0,0,0,0},{0,0,0,0,0,0}};
		if (str.substr(0,1) == "E" && str.substr(2,1) == ":" && this->vExceptions.size() <= this->exceptions_limit){
			exception.id = atoi(str.substr(1,1).c_str());
			exception.begin.year = atoi(str.substr(3,2).c_str());
			exception.begin.month = atoi(str.substr(6,2).c_str());
			exception.begin.day = atoi(str.substr(9,2).c_str());
			exception.begin.hours = atoi(str.substr(12,2).c_str());
			exception.begin.minutes = atoi(str.substr(15,2).c_str());
			exception.begin.seconds = atoi(str.substr(18,2).c_str());
			exception.end.year = atoi(str.substr(21,2).c_str());
			exception.end.month = atoi(str.substr(24,2).c_str());
			exception.end.day = atoi(str.substr(27,2).c_str());
			exception.end.hours = atoi(str.substr(30,2).c_str());
			exception.end.minutes = atoi(str.substr(33,2).c_str());
			exception.end.seconds = atoi(str.substr(36,2).c_str());
			this->vExceptions.push_back(exception);
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
}

bool Scheduler::addLine(const char *_line){
	const std::string str(_line);
	if (str.length() == 39 || str.length() == 42 ){
		if (str.substr(0,1) == "E" && str.substr(2,1) == ":"){
			this->addException(_line);
			return true;
		}
		else if (str.substr(0,1) == "A" && str.substr(2,1) == ":"){
			this->addActivity(_line);
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
}

std::vector<activity_s>& Scheduler::getActivities(void){
	return this->vActivities;
}

const uint8_t Scheduler::getActivitiesCount(void){
	return static_cast<uint8_t>(this->vActivities.size());
}

std::vector<exception_s>& Scheduler::getExceptions(void){
	return this->vExceptions;
}

const uint8_t Scheduler::getExceptionsCount(void){
	return static_cast<uint8_t>(this->vExceptions.size());
}

activity_s Scheduler::parseActivity(const char *_line){
	const std::string str(_line);
	activity_s activity = {0xff,0,{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0},{0,0,0}};
	if (str.length() == 42){
		if (str.substr(0,1) == "A" && str.substr(2,1) == ":")
		{
			activity.id = atoi(str.substr(1,1).c_str());
			activity.begin.year = atoi(str.substr(3,2).c_str());
			activity.begin.month = atoi(str.substr(6,2).c_str());
			activity.begin.day = atoi(str.substr(9,2).c_str());
			activity.end.year = atoi(str.substr(12,2).c_str());
			activity.end.month = atoi(str.substr(15,2).c_str());
			activity.end.day = atoi(str.substr(18,2).c_str());
			const std::string weekday_str = str.substr(21,3);
			if (weekday_str == "MON"){
				activity.weekday = ((uint8_t)0x01);
			}
			else if(weekday_str == "TUE"){
				activity.weekday = ((uint8_t)0x02);
			}
			else if(weekday_str == "WED"){
				activity.weekday = ((uint8_t)0x03);
			}
			else if(weekday_str == "THU"){
				activity.weekday = ((uint8_t)0x04);
			}
			else if(weekday_str == "FRI"){
				activity.weekday = ((uint8_t)0x05);
			}
			else if(weekday_str == "SAT"){
				activity.weekday = ((uint8_t)0x06);
			}
			else if(weekday_str == "SUN"){
				activity.weekday = ((uint8_t)0x07);
			}
			else{
				activity.weekday = ((uint8_t)0xFF);
			}
			activity.start.hours = atoi(str.substr(25,2).c_str());
			activity.start.minutes = atoi(str.substr(28,2).c_str());
			activity.start.seconds = atoi(str.substr(31,2).c_str());
			activity.duration.hours = atoi(str.substr(34,2).c_str());
			activity.duration.minutes = atoi(str.substr(37,2).c_str());
			activity.duration.seconds = atoi(str.substr(40,2).c_str());
		}
	}
	return activity;
}

exception_s Scheduler::parseException(const char *_line){
	const std::string str(_line);
	exception_s exception = {0xff,{0,0,0,0,0,0},{0,0,0,0,0,0}};
	if (str.length() == 39){
		if (str.substr(0,1) == "E" && str.substr(2,1) == ":"){
			exception.id = atoi(str.substr(1,1).c_str());
			exception.begin.year = atoi(str.substr(3,2).c_str());
			exception.begin.month = atoi(str.substr(6,2).c_str());
			exception.begin.day = atoi(str.substr(9,2).c_str());
			exception.begin.hours = atoi(str.substr(12,2).c_str());
			exception.begin.minutes = atoi(str.substr(15,2).c_str());
			exception.begin.seconds = atoi(str.substr(18,2).c_str());
			exception.end.year = atoi(str.substr(21,2).c_str());
			exception.end.month = atoi(str.substr(24,2).c_str());
			exception.end.day = atoi(str.substr(27,2).c_str());
			exception.end.hours = atoi(str.substr(30,2).c_str());
			exception.end.minutes = atoi(str.substr(33,2).c_str());
			exception.end.seconds = atoi(str.substr(36,2).c_str());
		}
	}
	return exception;
}







