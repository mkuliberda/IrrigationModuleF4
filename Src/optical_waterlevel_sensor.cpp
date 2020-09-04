/*
 * optical_waterlevel_sensor.cpp
 *
 *  Created on: 21.01.2020
 *      Author: Mati
 */

#include "optical_waterlevel_sensor.h"


/*************************************************/
/*! OpticalWaterLevelSensor class implementation */
/*************************************************/

bool OpticalWaterLevelSensor::init(const float & _mountpositionMeters, const struct gpio_s & _pinout){
	this->mountpositionMeters = _mountpositionMeters;
	this->pinout.pin = _pinout.pin;
	this->pinout.port = _pinout.port;
	this->read();
	return true;
}

const float& OpticalWaterLevelSensor::mountpositionGet(void){
	return this->mountpositionMeters;
}

void OpticalWaterLevelSensor::read(void){
	if (HAL_GPIO_ReadPin(this->pinout.port, this->pinout.pin) == GPIO_PIN_SET) this->state = fixedwaterlevelsensorstate_t::dry;
	else this->state = fixedwaterlevelsensorstate_t::wet;
}

bool OpticalWaterLevelSensor::isValid(void){
	return this->state != fixedwaterlevelsensorstate_t::undetermined ? true : false;
}

bool OpticalWaterLevelSensor::isSubmersed(void){
	this->read();
	return this->state == fixedwaterlevelsensorstate_t::wet ? true : false;
}

