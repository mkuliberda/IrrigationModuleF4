
#include <plants.h>


/***********************************/
/*! Plant class implementation */
/***********************************/

float Plant::getMoisturePercent(void) {
	return soil_moisture_percent;
}

bool Plant::setMoisturePercent(const float& _moisture) {
	if (_moisture <= 100 && _moisture >= 0){
		soil_moisture_percent = _moisture;
		return true;
	}
	else {
		return false;
	}
}

std::string Plant::getName(void) {
	return name;
}

uint8_t Plant::getId(void) {
	return id;
}

void Plant::updateRaw(const uint32_t& _raw_measurement){

}

/***********************************/
/*! PlantWithSensor class implementation */
/***********************************/

float PlantWithSensor::getMoisturePercent() {
	return m_wrappee->getMoisturePercent();
}

bool PlantWithSensor::setMoisturePercent(const float& _moisture) {
	return m_wrappee->setMoisturePercent(_moisture);
}

std::string PlantWithSensor::getName() {
	return m_wrappee->getName();
}

uint8_t PlantWithSensor::getId() {
	return m_wrappee->getId();
}

void PlantWithSensor::updateRaw(const uint32_t &_raw_measurement) {
}

float PlantWithDMAMoistureSensor::getMoisturePercent() {
	return PlantWithSensor::getMoisturePercent();
}

std::string PlantWithDMAMoistureSensor::getName() {
	return PlantWithSensor::getName();
}

uint8_t PlantWithDMAMoistureSensor::getId() {
	return PlantWithSensor::getId();
}

void PlantWithDMAMoistureSensor::updateRaw(const uint32_t& _raw_measurement) {
	soil_moisture_raw = _raw_measurement;
	soil_moisture_volts = _raw_measurement * ref_voltage / quantization_levels;
	float moisture_percent = soil_moisture_raw * 100 / quantization_levels;
	PlantWithSensor::setMoisturePercent(moisture_percent);
}




