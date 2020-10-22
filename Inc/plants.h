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


//Plants based on Decorator design pattern

class PlantInterface {
public:
	virtual ~PlantInterface() {}
	virtual float getMoisturePercent() = 0;
	virtual bool setMoisturePercent(const float &_moisture) = 0;
	virtual std::string getName() = 0;
	virtual uint8_t getId() = 0;
	virtual void updateRaw(const uint32_t &_raw_measurement) = 0;
};

class Plant : public PlantInterface {
public:
	Plant(const std::string& _name, const uint8_t& _id) :
		name(_name),
		id(_id)
	{
		name.shrink_to_fit();
	}

	Plant(const std::string&& _name, const uint8_t&& _id) :
		name(std::move(_name)),
		id(std::move(_id))
	{
		name.shrink_to_fit();
	}

	~Plant()
	{
	}
	/*virtual*/
	float getMoisturePercent(void);
	std::string getName(void);
	uint8_t getId(void);
	bool setMoisturePercent(const float& _moisture);
	void updateRaw(const uint32_t& _raw_measurement);

private:
	std::string name;
	float soil_moisture_percent = -1000;
	uint8_t id;
};

class PlantWithSensor : public PlantInterface {
public:
	PlantWithSensor(PlantInterface *inner) {
		m_wrappee = inner;
	}
	~PlantWithSensor() {
		delete m_wrappee;
	}

	float getMoisturePercent();
	bool setMoisturePercent(const float& _moisture);
	std::string getName();
	uint8_t getId();
	void updateRaw(const uint32_t &_raw_measurement);

private:
	PlantInterface *m_wrappee;
};

class PlantWithDMAMoistureSensor : public PlantWithSensor {
public:
	PlantWithDMAMoistureSensor(PlantInterface *core, const float& _ref_voltage = 3.3, const uint32_t& _quantization_levels = 4095) :
		PlantWithSensor(core),
		ref_voltage(_ref_voltage),
		quantization_levels(_quantization_levels){
	}

	PlantWithDMAMoistureSensor(PlantInterface *core, const float&& _ref_voltage = 3.3, const uint32_t&& _quantization_levels = 4095) :
		PlantWithSensor(core),
		ref_voltage(std::move(_ref_voltage)),
		quantization_levels(std::move(_quantization_levels)) {
	}

	~PlantWithDMAMoistureSensor() {
	}

	float getMoisturePercent();
	std::string getName();
	uint8_t getId();
	void updateRaw(const uint32_t& _raw_measurement);

private:
	float ref_voltage;
	uint32_t quantization_levels;
	float soil_moisture_volts;
	float soil_moisture_raw;
};


#endif /* PLANTS_H_ */
