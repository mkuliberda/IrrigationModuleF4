#include "sensors.h"

float SensorImp::read(const double& _dt) {
	return 0.0;
}

sensor_type_t SensorImp::getType() const{
	return sensor_type;
}

bool SensorImp::isValid() const{
	return this->valid;
}

float SensorImp::getMountHeightMeters() const {
	return this->mount_height_meters;
}

float OpticalWaterLevelSensorImp::read(const double& _dt) {
	if (isValid()) {
		if (HAL_GPIO_ReadPin(this->pinout.port, this->pinout.pin) == GPIO_PIN_SET) {
			return static_cast<float>(this->state = fixedwaterlevelsensorstate_t::dry);
		}
		else {
			return static_cast<float>(this->state = fixedwaterlevelsensorstate_t::wet);
		}
	}
	else {
		return static_cast<float>(fixedwaterlevelsensorstate_t::undetermined);
	}
}

void OpticalWaterLevelSensorImp::init(const float& _mount_height_meters, const struct gpio_s& _pinout) {
	mount_height_meters = _mount_height_meters;
	pinout.pin = _pinout.pin;
	pinout.port = _pinout.port;
	valid = true;
	read();
}

/*const float& OpticalWaterLevelSensorImp::getMountHeightMeters(void) {
	return this->mount_height_meters;
}*/

/*const bool& OpticalWaterLevelSensorImp::isValid(void) const{
	return valid;
}*/

bool OpticalWaterLevelSensorImp::isSubmersed(void) {
	if(isValid())	return this->state == fixedwaterlevelsensorstate_t::wet ? true : false;
	else return false;
}

float WaterFlowSensorImp::read(const double& _dt) {
	return 1.0;
}

float DS18B20TemperatureSensorImp::read(const double& _dt) {
	return this->readTemperatureCelsius(_dt);
}

float Sensor::read(const double& _dt) {
	return imp_->read(_dt);
}
sensor_type_t Sensor::getType() {
	return imp_->getType();
}

bool Sensor::isValid() {
	return imp_->isValid();
}

float Sensor::getMountHeightMeters() const {
	return imp_->getMountHeightMeters();
}
