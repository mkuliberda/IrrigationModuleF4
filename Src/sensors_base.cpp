/*
 * sensors_base.cpp
 *
 *  Created on: 21.01.2020
 *      Author: Mati
 */

#include "sensors_base.h"


/******************************************/
/*! TemperatureSensor class implementation */
/******************************************/

temperaturesensortype_t& TemperatureSensor::typeGet(void){
	return this->type;
}

sensorinterfacetype_t& TemperatureSensor::interfacetypeGet(void){
	return this->interfacetype;
}
/***********************************/
/*! MoistureSensor class implementation */
/***********************************/

moisturesensortype_t& MoistureSensor::typeGet(void){
	return this->type;
}

sensorinterfacetype_t& MoistureSensor::interfacetypeGet(void){
	return this->interfacetype;
}

float& MoistureSensor::percentGet(void){
	return this->moisturePercent;
}


/******************************************/
/*! WaterLevelSensor class implementation */
/******************************************/

waterlevelsensorsubtype_t&  WaterLevelSensor::subtypeGet(void){
	return this->subtype;
}

waterlevelsensortype_t& WaterLevelSensor::typeGet(void){
	return this->type;
}

sensorinterfacetype_t& WaterLevelSensor::interfacetypeGet(void){
	return this->interfacetype;
}


