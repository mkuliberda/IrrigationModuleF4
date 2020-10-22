/*
 * Irrigation.h
 *
 *  Created on: 28.11.2019
 *      Author: Mati
 */

#ifndef IRRIGATION_H_
#define IRRIGATION_H_


#include "stm32f4xx_hal.h"
#include <string>
#include "main.h"
#include "gpio.h"
#include "tim.h"
#include <array>
#include <vector>
#include <memory>
#include <utilities.h>
#include <bitset>
#include <numeric>
#include "optical_waterlevel_sensor.h"
#include "analog_moisture_sensor.h"
#include "ds18b20.h"


struct pumpstatus_s {
	uint8_t id = 0;
	uint32_t state = 0;
	bool forced = false;
	bool cmd_consumed = false;
};

struct tankstatus_s {
	uint8_t id;
	uint32_t state;
};


enum class pumpstate_t{
	init,
	running,
	reversing,
	stopped,
	waiting,
	fault,
	sleep
};

enum class pumptype_t{
	generic,
	binary,
	drv8833_dc,
	drv8833_bldc
};

enum class motortype_t{
	dc_motor,
	bldc_motor
};

enum class pumpcmd_t{
	start,
	stop,
	reverse
};


enum class pumpcontrollermode_t{
	init,
	automatic,
	manual,
	external,
	sleep
};


class Pump{

protected:

	pumptype_t					type = pumptype_t::generic;
	pumpstate_t 				state = pumpstate_t::init;		///< current pump's working state based on enum pumpstate_t
	struct pumpstatus_s 		status;

	double 						runtimeSeconds;					///< current runtime, incrementing in running state [seconds]
	double 						idletimeSeconds;				///< current idletime incrementing in stopped and waiting state [seconds]
	uint32_t 					runtimeLimitSeconds;			///< runtime limit for particular pump [seconds]
	uint32_t 					idletimeRequiredSeconds; 		///< idletime required between two consecutive runs [seconds]

	virtual bool 				start() = 0;
	virtual bool 				stop() = 0;
	pumptype_t& 				getType();

public:

	Pump():
		runtimeSeconds(0.0),
		idletimeSeconds(0.0),
		runtimeLimitSeconds(0),
		idletimeRequiredSeconds(0)
	{};

	virtual ~Pump(){};

	bool 						init();
	void 						run(const double & _dt);
	virtual void 				setState(const pumpstate_t & _st) = 0;
	pumpstate_t& 				getState(void);
	virtual bool 				isRunning(void);
	struct pumpstatus_s&		getStatus(void);

	void 						runtimeReset(void);
	void 						runtimeIncrease(const double & _dt);
	double&						runtimeGetSeconds(void);
	void 						idletimeReset(void);
	void 						idletimeIncrease(const double & _dt);
	double& 					idletimeGetSeconds(void);

};

class BinaryPump: public Pump{

private:

	struct gpio_s 				pinout;
	struct gpio_s 				led;

protected:

	bool 						start(void) override;
	bool 						stop(void) override;

public:

	BinaryPump()
	{
		this->type=pumptype_t::binary;
	};

	bool 						init(const uint8_t & _id, const uint32_t & _idletimeRequiredSeconds, const uint32_t & _runtimeLimitSeconds, \
									const struct gpio_s & _pinout, const struct gpio_s & _led);
	void 						run(const double & _dt, const pumpcmd_t & _cmd, bool & cmd_consumed);
	void 						setState(const pumpstate_t & _st) override;
	void 						forcestart(void);
	void 						forcestop(void);

};

class DRV8833Pump: public Pump{

private:

	std::array<struct gpio_s, 4> 	aIN;							///< in1, in2, in3, in4
	struct gpio_s 					led;
	struct gpio_s 					fault;
	struct gpio_s 					mode;

protected:

	bool 						start(void) override;
	bool 						stop(void) override;
	bool						reverse(void);


public:

	DRV8833Pump(const motortype_t & _type)
	{
		if (_type == motortype_t::bldc_motor) this->type = pumptype_t::drv8833_bldc;
		if (_type == motortype_t::dc_motor) this->type = pumptype_t::drv8833_dc;
	};

	bool 						init(const uint8_t & _id, const uint32_t & _idletimeRequiredSeconds, const uint32_t & _runtimeLimitSeconds, \
									const std::array<struct gpio_s, 4> & _pinout, const struct gpio_s & _led_pinout, \
									const struct gpio_s & _fault_pinout, const struct gpio_s & _mode_pinout);
	bool 						init(const uint8_t & _id, const uint32_t & _idletimeRequiredSeconds, const uint32_t & _runtimeLimitSeconds, \
									const std::array<struct gpio_s, 2> & _pinout, const struct gpio_s & _led_pinout, \
									const struct gpio_s & _fault_pinout, const struct gpio_s & _mode_pinout);
	void 						run(const double & _dt, const pumpcmd_t & _cmd, bool & cmd_consumed, bool & fault);
	void 						setState(const pumpstate_t & _st) override;
	bool 						forcestart(void);
	bool 						forcereverse(void);
	bool 						forcestop(void);
	void 						setSleep(void);
	void 						setEnable(void);
	bool						isFault(void);

};


class WaterTank{

private:

	enum class contentstate_t: uint8_t{
		unknown 	= 255,
		liquid 		= 1,
		frozen 		= 2,
		boiling		= 3
	};

	enum class contentlevel_t: uint8_t{
		unknown		= 255,
		empty		= 0,
		above10		= 10,
		above20		= 20,
		above30		= 30,
		above40		= 40,
		above50		= 50,
		above60		= 60,
		above70		= 70,
		above80		= 80,
		above90		= 90,
		full 		= 100
	};

	float								mean_watertemperatureCelsius;
	contentlevel_t						waterlevel;
	contentstate_t 						waterstate;
	const double 						tankheightMeters;
	const double 						tankvolumeLiters;
	const uint8_t 						waterlevelSensorsLimit;
	uint8_t								waterlevelSensorsCount;
	const uint8_t 						temperatureSensorsLimit;
	uint8_t								temperatureSensorsCount;
	uint8_t								id;

	void 								setWaterLevel(const contentlevel_t & _waterlevel);
	contentlevel_t&						getWaterLevel(void);
	void 								setState(const contentstate_t & _waterstate);
	contentstate_t&						getState(void);
	uint8_t 							waterlevelConvertToPercent(const float & _valMeters);

public:

	WaterTank(const double & _tankheightMeters, const double & _tankvolumeLiters, const uint8_t & _id):
		mean_watertemperatureCelsius(0.0),
		waterlevel(contentlevel_t::unknown),
		waterstate(contentstate_t::unknown),
		tankheightMeters(_tankheightMeters),
		tankvolumeLiters(_tankvolumeLiters),
		waterlevelSensorsLimit(10),
		waterlevelSensorsCount(0),
		temperatureSensorsLimit(3),
		temperatureSensorsCount(0),
		id(_id)
	{};

	~WaterTank()
	{};

	std::vector <OpticalWaterLevelSensor> 	vOpticalWLSensors;
	std::vector <DS18B20> 					vTemperatureSensors;

	bool 								init(void);
	bool 								checkStateOK(const double &_dt, uint32_t & errcodeBitmask);
	float& 								getTemperatureCelsius(void);
	uint8_t		 						getWaterLevelPercent(void);
	bool 								createWaterLevelSensor(const waterlevelsensortype_t & _sensortype);
	bool 								createTemperatureSensor(const temperaturesensortype_t & _sensortype);
	uint8_t&							getId(void);

};

class PumpController{

private:

	const uint8_t 						pumpsLimit = 1;
	uint8_t								pumpsCount;
	const uint8_t						moisturesensorsLimit = 10;
	uint8_t								moisturesensorsCount;
	pumpcontrollermode_t				mode;
	uint8_t								pumpEncodedStatus;
	uint32_t							pumpFaultOccurenceCnt;

public:

	PumpController():
		pumpsCount(0),
		moisturesensorsCount(0),
		mode(pumpcontrollermode_t::init),
		pumpEncodedStatus(255),
		pumpFaultOccurenceCnt(0)
	{};

	~PumpController()
	{
		if (pBinPump != nullptr) delete pBinPump;
		if (p8833Pump != nullptr) delete p8833Pump;
	};

	BinaryPump							*pBinPump = nullptr;
	DRV8833Pump							*p8833Pump = nullptr;
	//std::vector <AnalogDMAMoistureSensor> 	vDMAMoistureSensor;

	bool								update(const double & _dt, bool & _activate_watering);
	bool								createPump(const pumptype_t & _pumptype);
	//bool 								createMoistureSensor(const moisturesensortype_t & _sensortype);
	bool								setMode(const pumpcontrollermode_t & _mode);
	const pumpcontrollermode_t&			getMode(void);
	uint8_t&							getPumpStatusEncoded(void);


};


void pumpStateEncode(const struct pumpstatus_s & _pump, uint32_t & status);
void pumpStateDecode(std::array<struct pumpstatus_s,4> & a_pump, const std::bitset<32> & _status);




#endif /* IRRIGATION_H_ */
