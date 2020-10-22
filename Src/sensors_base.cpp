/*
 * sensors_base.cpp
 *
 *  Created on: 21.01.2020
 *      Author: Mati
 */

#include "sensors_base.h"


/******************************************/
/*! Sensor class implementation */
/******************************************/
sensorinterfacetype_t& Sensor::getInterface(void)
{
	return this->interfacetype;
}


/******************************************/
/*! TemperatureSensor class implementation */
/******************************************/

temperaturesensortype_t& TemperatureSensor::getType(void){
	return this->type;
}

//sensorinterfacetype_t& TemperatureSensor::getInterface(void){
//	return this->interfacetype;
//}


/***********************************/
/*! MoistureSensor class implementation */
/***********************************/

moisturesensortype_t& MoistureSensor::getType(void){
	return this->type;
}

//sensorinterfacetype_t& MoistureSensor::getInterface(void){
//	return this->interfacetype;
//}

float& MoistureSensor::getMoisturePercent(void){
	return this->moisturePercent;
}


/******************************************/
/*! WaterLevelSensor class implementation */
/******************************************/

waterlevelsensorsubtype_t&  WaterLevelSensor::getSubtype(void){
	return this->subtype;
}

waterlevelsensortype_t& WaterLevelSensor::getType(void){
	return this->type;
}

//sensorinterfacetype_t& WaterLevelSensor::getInterface(void){
//	return this->interfacetype;
//}


