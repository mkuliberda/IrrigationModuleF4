
#include <plants.h>


/***********************************/
/*! Plants class implementation */
/***********************************/

float& Plant::getMoisturePercent(void){
	return this->soil_moisture;
}

void Plant::setMoisturePercent(const float &_soil_moisture){
	this->soil_moisture = _soil_moisture;
}

const std::string& Plant::getName(void){
	return this->name;
}

const uint8_t& Plant::getId(void){
	return this->id;
}




