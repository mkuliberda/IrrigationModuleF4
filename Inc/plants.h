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

#define NAME_LENGTH 11
#define PLANTS_LENGTH 20

struct plantstatus_s{
	char name[NAME_LENGTH];
	uint8_t id;
	float health;
};

struct sectorstatus_s {
	uint8_t id;
	uint32_t state;
	char plants[PLANTS_LENGTH];
};



class Plant{

private:

	std::string name;
	float soilMoisture;
	const uint8_t id;

public:

	Plant(const std::string & _name, const uint8_t & _id):
	name(_name),
	soilMoisture(-1000),
	id(_id)
	{};

	~Plant(){};

	void 					moisturePercentSet(const float & _soilmoisture);
	float& 					moisturePercentGet(void);
	std::string& 			nameGet(void);
	void					nameChange(const std::string & _new_name);
	const uint8_t&			idGet(void);

};

class IrrigationSector{

private:

	const uint8_t 			id;
	const uint8_t 			plantsCountMax = 20;
	uint8_t 				plantsCount;
	std::vector<Plant> 		vPlants;
	uint8_t 				status;
	bool					water_plants;

public:

	PumpController irrigationController;

	IrrigationSector(const uint8_t & _id):
	id(_id),
	plantsCount(0),
	status(0),
	water_plants(false)
	{
	};

	~IrrigationSector()
	{
	}

	const uint8_t& 			getSector(void);
	bool 					createPlant(const std::string & _name, const uint8_t & _id);
	uint8_t& 				update(const double & _dt);
	uint8_t& 				getPlantsCount(void);
	struct sectorstatus_s	getInfo(void);
	float 					getPlantHealth(const std::string & _name);
	float 					getPlantHealth(const uint8_t & _id);
	std::string				getPlantNameByID(const uint8_t & _id);
	void					setMeasurements(uint16_t *_raw_adc_values_array, const uint8_t & _raw_adc_values_cnt);
	uint8_t& 				getStatus(void);
	pumpstate_t 			getPumpState(void);
	uint8_t					getPumpStatusEncoded(void);
	void					wateringSet(const bool & _activate_watering);
	bool&					wateringGet(void);
	bool 					handleConfirmation(void);

};

#endif /* PLANTS_H_ */
