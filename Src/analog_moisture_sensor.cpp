/*
 * adc_moisture_sensor.cpp
 *
 *  Created on: 21.01.2020
 *      Author: Mati
 */

#include <analog_moisture_sensor.h>


/***********************************/
/*! AnalogDMAMoistureSensor class implementation */
/***********************************/

void AnalogDMAMoistureSensor::voltsUpdate(void){
	this->moistureVolts = this->moistureRaw * 3.3f/4095.0f;
}

void AnalogDMAMoistureSensor::percentUpdate(void){
	this->moisturePercent = this->moistureRaw * 100.0f/4095.0f;
}

float AnalogDMAMoistureSensor::read(void){
	return 0; //Not used in this type of sensor
}

bool& AnalogDMAMoistureSensor::isValid(void){
	return this->valid;
}

void AnalogDMAMoistureSensor::rawUpdate(const uint16_t & _raw_value){
	this->moistureRaw = _raw_value;
	this->percentUpdate();
	this->voltsUpdate();
}

float& AnalogDMAMoistureSensor::voltsGet(void){
	return this->moistureVolts;
}



