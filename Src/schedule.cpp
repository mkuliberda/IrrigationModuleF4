/*
 * schedule.cpp
 *
 *  Created on: 05.09.2020
 *      Author: Mati
 */

#include "schedule.h"

bool Schedule::compare_time(const time_t &_time1, const time_t &_time2){
	return difftime(_time1, _time2) > 0.0 ? true : false;
}

bool& Schedule::isActive(const TimeStamp_t &_timestamp){

	std::vector<activity_s> currently_active;
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
				time_t activity_start, activity_end, activity_duration;

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
				activity_duration = mktime(&tm_timestamp);

				activity_end = activity_start + activity_duration;

				if(this->compare_time(now, activity_start) && this->compare_time(activity_end, now)){
					return this->is_active = true;
				}
				else{
					return this->is_active = false;
				}
			}
		}
	}

	return is_active = false;

}

bool Schedule::addActivity(const struct activity_s &_activity){
	this->vActivities.push_back(_activity);
	return true;
}

//01234567891123456789212345678931234567894
/*A:20-05-01,20-09-30,MON,06-00-00,00-10-00
A:20-05-01,20-09-30,MON,19-00-00,00-10-00
A:20-05-01,20-09-30,TUE,06-00-00,00-10-00
A:20-05-01,20-09-30,TUE,19-00-00,00-10-00
A:20-05-01,20-09-30,THU,06-00-00,00-10-00
A:20-05-01,20-09-30,THU,20-00-00,00-10-00
E:20-05-15,00-00-01,20-05-15,16-00-00*/
bool Schedule::addActivity(const char *_activity){
	const std::string str(_activity);
	if (str.length() == 41){
		//try:
		activity_s activity = {{0,0,0,0,0,0},{0,0,0,0,0,0},0,{0,0,0},{0,0,0}};
		if (str.substr(0,2) == "A:"){
			activity.begin.year = atoi(str.substr(2,2).c_str());
			activity.begin.month = atoi(str.substr(5,2).c_str());
			activity.begin.day = atoi(str.substr(8,2).c_str());
			activity.end.year = atoi(str.substr(11,2).c_str());
			activity.end.month = atoi(str.substr(14,2).c_str());
			activity.end.day = atoi(str.substr(17,2).c_str());
			const std::string weekday_str = str.substr(20,3);
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
			activity.start.hours = atoi(str.substr(24,2).c_str());
			activity.start.minutes = atoi(str.substr(27,2).c_str());
			activity.start.seconds = atoi(str.substr(30,2).c_str());
			activity.duration.hours = atoi(str.substr(33,2).c_str());
			activity.duration.minutes = atoi(str.substr(36,2).c_str());
			activity.duration.seconds = atoi(str.substr(39,2).c_str());
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

bool Schedule::addException(const struct exception_s &_exception){
	this->vExceptions.push_back(_exception);
	return true;
}

bool Schedule::addException(const char *_exception){
	const std::string str(_exception);
	if (str.length() == 38){
		//try:
		exception_s exception = {{0,0,0,0,0,0}, {0,0,0,0,0,0}};
		if (str.substr(0,2) == "E:"){
			exception.begin.year = atoi(str.substr(2,2).c_str());
			exception.begin.month = atoi(str.substr(5,2).c_str());
			exception.begin.day = atoi(str.substr(8,2).c_str());
			exception.begin.hours = atoi(str.substr(11,2).c_str());
			exception.begin.minutes = atoi(str.substr(14,2).c_str());
			exception.begin.seconds = atoi(str.substr(17,2).c_str());
			exception.end.year = atoi(str.substr(20,2).c_str());
			exception.end.month = atoi(str.substr(23,2).c_str());
			exception.end.day = atoi(str.substr(26,2).c_str());
			exception.end.hours = atoi(str.substr(29,2).c_str());
			exception.end.minutes = atoi(str.substr(32,2).c_str());
			exception.end.seconds = atoi(str.substr(35,2).c_str());
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

bool Schedule::addLine(const char *_line){
	const std::string str(_line);
	if (str.length() == 38 || str.length() == 41 ){
		if (str.substr(0,2) == "E:"){
			this->addException(_line);
			return true;
		}
		else if (str.substr(0,2) == "A:"){
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







