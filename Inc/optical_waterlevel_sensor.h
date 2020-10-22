/*
 * optical_waterlevel_sensor.h
 *
 *  Created on: 21.01.2020
 *      Author: Mati
 */

#ifndef OPTICAL_WATERLEVEL_SENSOR_H_
#define OPTICAL_WATERLEVEL_SENSOR_H_

#include "sensors_base.h"
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "main.h"


class OpticalWaterLevelSensor: public WaterLevelSensor{

private:

	float 							mountpositionMeters;
	fixedwaterlevelsensorstate_t 	state;
	struct gpio_s 					pinout;

	void 							read(void);

public:

	OpticalWaterLevelSensor():
		mountpositionMeters(0),
		state(fixedwaterlevelsensorstate_t::undetermined)
		{
			this->type = waterlevelsensortype_t::optical;
			this->subtype = waterlevelsensorsubtype_t::fixed;
			this->interfacetype = sensorinterfacetype_t::gpio;
		};

	const float& 					getMountPosition(void);
	bool 							init(const float & _mountpositionMeters, const struct gpio_s & _pinout);
	bool 							isValid(void);
	bool 							isSubmersed(void);

};



#endif /* OPTICAL_WATERLEVEL_SENSOR_H_ */
