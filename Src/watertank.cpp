#include "watertank.h"
#include <bitset>



/*!
* Watertank class implementation
*/

float& Watertank::getWaterTemperatureCelsius(void) {
	return this->mean_water_temperature_celsius;
}

float Watertank::getWaterTemperatureKelvin(void) {
	return (this->mean_water_temperature_celsius - 273.15_Ce);
}

float Watertank::getWaterTemperatureFahrenheit(void) {
	return ((this->mean_water_temperature_celsius - 32.0_Ce) * 5.0_Ce / 9.0_Ce);
}

void Watertank::setWaterLevel(const contentlevel_t & _water_level) {
	this->water_level = _water_level;
}

Watertank::contentlevel_t& Watertank::getWaterLevel(void) {
	return this->water_level;
}

void Watertank::setWaterState(const contentstate_t & _state) {
	this->water_state = _state;
}

Watertank::contentstate_t& Watertank::getWaterState(void) {
	return this->water_state;
}

bool Watertank::update(const double& _dt) {

	/******************************errcode_bitmask****************************************
	 * *Upper 16 bits										Lower 16 bits
	 * 00000000 00000000 									00000000 00000000
	 * |||||||| ||||||||->water temperature too low	 (16)	|||||||| ||||||||->(0)
	 * |||||||| |||||||-->water temperature too high (17)	|||||||| |||||||-->(1)
	 * |||||||| ||||||--->water level too low		 (18)	|||||||| ||||||--->(2)
	 * |||||||| |||||---->temperature sensor1 invalid(19)	|||||||| |||||---->(3)
	 * |||||||| ||||----->temperature sensor2 invalid(20)	|||||||| ||||----->(4)
	 * |||||||| |||------>temperature sensor3 invalid(21)	|||||||| |||------>(5)
	 * |||||||| ||------->wl sensor1 invalid         (22)	|||||||| ||------->(6)
	 * |||||||| |-------->wl sensor2 invalid         (23)	|||||||| |-------->(7)
	 * ||||||||---------->wl sensor3 invalid         (24)	||||||||---------->(8)
	 * |||||||----------->wl sensor4 invalid         (25)	|||||||----------->(9)
	 * ||||||------------>wl sensor5 invalid         (26)	||||||------------>(10)
	 * |||||------------->wl sensor6 invalid         (27)	|||||------------->(11)
	 * ||||-------------->wl sensor7 invalid         (28)	||||-------------->(12)
	 * |||--------------->wl sensor8 invalid         (29)	|||--------------->(13)
	 * ||---------------->wl sensor9 invalid         (30)	||---------------->(14)
	 * |----------------->wl sensor10 invalid        (31)	|----------------->(15)
	 */

	
	uint8_t temp_water_level_percent = 0;
	uint8_t water_level_percent = 0;
	bool is_ok = true;
	float water_temperature = 0.0_Ce;
	uint8_t water_temperature_readings_count = 0;
	uint8_t water_level_readings_count = 0;
	std::bitset<32> errcode; errcode.set();  //initialize bitset and set all bits to 1

	abs_time += _dt;

	for (auto &sensor : vSensors) {
		switch (sensor->getType()) {
		case sensor_type_t::waterlevel_sensor:
			if (sensor->isValid() == true) {
				errcode.reset(22 + water_level_readings_count);
				if (sensor->read()) { //TODO: check if 1 means submersed on STm32
					temp_water_level_percent = this->waterlevelConvertToPercent(sensor->getMountHeightMeters());
					if (temp_water_level_percent > water_level_percent) water_level_percent = temp_water_level_percent;
				}
			}
			break;
		case sensor_type_t::temperature_sensor:
			water_temperature += sensor->read(_dt);
			errcode.reset(19 + water_temperature_readings_count);
			water_temperature_readings_count++;
			break;
		default:
			break;
		}
	}

	if (water_temperature_readings_count > 0) {
		this->mean_water_temperature_celsius = water_temperature / water_temperature_readings_count;

		if (this->mean_water_temperature_celsius < 1.0_Ce) {
			this->setWaterState(contentstate_t::frozen);
			errcode.reset(17);
			is_ok = false;
		}
		else if (this->mean_water_temperature_celsius > 90.0_Ce)
		{
			this->setWaterState(contentstate_t::boiling);
			errcode.reset(16);
			is_ok = false;
		}
		else {
			this->setWaterState(contentstate_t::liquid);
			errcode.reset(16);
			errcode.reset(17);
		}
	}
	


	if (water_level_percent >= 98) { this->setWaterLevel(contentlevel_t::full); errcode.reset(18);}
	else if (water_level_percent > 90) { this->setWaterLevel(contentlevel_t::above90); errcode.reset(18);}
	else if (water_level_percent > 80) { this->setWaterLevel(contentlevel_t::above80); errcode.reset(18);}
	else if (water_level_percent > 70) { this->setWaterLevel(contentlevel_t::above70); errcode.reset(18);}
	else if (water_level_percent > 60) { this->setWaterLevel(contentlevel_t::above60); errcode.reset(18);}
	else if (water_level_percent > 50) { this->setWaterLevel(contentlevel_t::above50); errcode.reset(18);}
	else if (water_level_percent > 40) { this->setWaterLevel(contentlevel_t::above40); errcode.reset(18);}
	else if (water_level_percent > 30) { this->setWaterLevel(contentlevel_t::above30); errcode.reset(18);}
	else if (water_level_percent > 20) { this->setWaterLevel(contentlevel_t::above20); errcode.reset(18);}
	else if (water_level_percent > 10) { this->setWaterLevel(contentlevel_t::above10); errcode.reset(18);}
	else if (water_level_percent >= 0) { this->setWaterLevel(contentlevel_t::empty); is_ok = false;}

	watertank_info.water_level = static_cast<uint8_t>(water_level_percent);
	watertank_info.errors = errcode.to_ulong();

	tank_state_hysteresis.set_state_and_update(is_ok, abs_time);

	return tank_state_hysteresis.get_state();
}

uint8_t Watertank::waterlevelConvertToPercent(const float& _level_meters) {
	return static_cast<uint8_t>(_level_meters / this->tank_height_meters * 100);
}

uint8_t Watertank::getWaterLevelPercent(void) {
	return static_cast<uint8_t>(this->water_level);
}

void  Watertank::setTankStateHysteresis(const double& _time_from_false_ms, const double& _time_from_true_ms) {
	tank_state_hysteresis.set_hysteresis_time_from(false, _time_from_false_ms);
	tank_state_hysteresis.set_hysteresis_time_from(true, _time_from_false_ms);
}


uint8_t& Watertank::getId(void) {
	return this->watertank_info.id;
}

void Watertank::setVolume(const double& _volume) {
	this->tank_volume_liters = _volume;
}

void Watertank::setHeight(const double& _height) {
	this->tank_height_meters = _height;
}

double& Watertank::getHeightMeters(void) {
	return this->tank_height_meters;
}

uint8_t Watertank::incrementTemperatureSensorsCount(void) {
	if (temperature_sensors_count <= temperature_sensors_limit) {
		temperature_sensors_count++;
		return W_SUCCESS;
	}
	else {
		return W_LIMIT_REACHED;
	}
}

uint8_t Watertank::incrementFixedWaterLevelSensorsCount() {
	if (fixed_water_level_sensors_count <= fixed_water_level_sensors_limit) {
		fixed_water_level_sensors_count++;
		return W_SUCCESS;
	}
	else {
		return W_LIMIT_REACHED;
	}
}


void ConcreteWatertankBuilder::Reset() {
	watertank.reset(new Watertank(static_cast<uint8_t>(++watertank_count)));
}
/**
 * All production steps work with the same watertank instance.
 */

void ConcreteWatertankBuilder::produceOpticalWaterLevelSensor(const float& _mount_height_meters, const struct gpio_s& _pinout) {
	if (watertank->incrementFixedWaterLevelSensorsCount() == W_SUCCESS && _mount_height_meters <= watertank->getHeightMeters()) {
		watertank->vSensors.emplace_back(new OpticalWaterLevelSensor(_mount_height_meters, _pinout));
		watertank->vSensors.shrink_to_fit();
	}
}

void ConcreteWatertankBuilder::produceDS18B20TemperatureSensor(const struct gpio_s& _pinout, TIM_HandleTypeDef* _tim_baseHandle) {
	if (watertank->incrementTemperatureSensorsCount() == W_SUCCESS) {
		watertank->vSensors.emplace_back(new DS18B20TemperatureSensor(_pinout, _tim_baseHandle));
		watertank->vSensors.shrink_to_fit();
	}
}

std::unique_ptr<Watertank> ConcreteWatertankBuilder::GetProduct() {
	std::unique_ptr<Watertank>(result);
	result = std::move(watertank);
	Reset();
	return result;
}

void ConcreteWatertankBuilder::setWatertankVolume(const double& _volume) {
	this->watertank->setVolume(_volume);
}
void ConcreteWatertankBuilder::setWatertankHeight(const double& _height) {
	this->watertank->setHeight(_height);
}
