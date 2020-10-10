/*
 * Plants.h
 *
 *  Created on: 28.11.2019
 *      Author: Mati
 */

#ifndef PLANTS_H_
#define PLANTS_H_

#include "stm32f4xx_hal.h"
#include "freertos_memory.h"
#include <string>
#include "gpio.h"
#include "tim.h"
#include <array>
#include <vector>
#include <memory>
#include <utilities.h>
#include <bitset>
#include <numeric>
#include <irrigation.h>
#include <cstring>


class Plant{

private:

	const std::string name;
	float soil_moisture;
	const uint8_t id;

public:

	Plant(const std::string &_name, const uint8_t &_id):
	name(_name),
	soil_moisture(-1000),
	id(_id)
	{};

	~Plant(){};

	float& 					getMoisturePercent(void);
	void 					setMoisturePercent(const float &_soil_moisture);
	const std::string& 		getName(void);
	const uint8_t&			getId(void);

};

class PlantWithDMAMoistureSensor: private Plant
{

public:

	PlantWithDMAMoistureSensor():Plant("pel",0){};

	~PlantWithDMAMoistureSensor(){};

	AnalogDMAMoistureSensor MoistureSensor;

};


#endif /* PLANTS_H_ */
