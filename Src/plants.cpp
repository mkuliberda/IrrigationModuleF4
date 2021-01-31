#include "plants.h"

float  Plant::getMoisturePercent(void) {
	return soil_moisture_percent;
}

std::string Plant::getName(void) {
	return name;
}

uint8_t Plant::getId(void) {
	return id;
}

bool Plant::setMoisturePercent(const float& _moisture) {
	if (_moisture <= 100 && _moisture >= 0) {
		soil_moisture_percent = _moisture;
		return true;
	}
	else {
		return false;
	}
}

void Plant::updateRaw(const uint32_t& _raw_measurement) {
}

void Plant::setPlantType(const plant_type_t& _p_type) {
	this->p_type = _p_type;
}

plant_type_t Plant::getPlantType(void) {
	return this->p_type;
}

void Plant::setRainExposition(const bool& _rain_exposed) {
	rain_exposed = _rain_exposed;
}

bool Plant::isRainExposed() {
	return rain_exposed;
}


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

void PlantWithSensor::setPlantType(const plant_type_t& _p_type) {
	m_wrappee->setPlantType(_p_type);
}

plant_type_t PlantWithSensor::getPlantType(void) {
	return m_wrappee->getPlantType();
}

void PlantWithSensor::setRainExposition(const bool& _rain_exposed) {
	m_wrappee->setRainExposition(_rain_exposed);
}

bool PlantWithSensor::isRainExposed() {
	return m_wrappee->isRainExposed();
}

float PlantWithDMAMoistureSensor::getMoisturePercent() {
	return PlantWithSensor::getMoisturePercent();
}

float PlantWithDMAMoistureSensor::getMoistureVolts() {
	return soil_moisture_volts;
}

float PlantWithDMAMoistureSensor::getMoistureRaw() {
	return soil_moisture_raw;
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

void PlantWithDMAMoistureSensor::setPlantType(const plant_type_t& _p_type) {
	PlantWithSensor::setPlantType(plant_type_t::plant_with_dma_moisture_sensor);
}

plant_type_t PlantWithDMAMoistureSensor::getPlantType() {
	return PlantWithSensor::getPlantType();
}

void PlantWithDMAMoistureSensor::setRainExposition(const bool& _rain_exposed) {
	PlantWithSensor::setRainExposition(_rain_exposed);
}

bool PlantWithDMAMoistureSensor::isRainExposed() {
	return PlantWithSensor::isRainExposed();
}
