#include "sector.h"
#include "utilities.h"

void IrrigationSector::encodeErrors(){
	sector_info.errors = 0;
	//TODO: implement this
}

void IrrigationSector::encodeState(){
	sector_info.state = 0;
	//TODO: implement this
}

void IrrigationSector::encodePlants() {
	std::string sector_plants;
	for (auto &plant : vPlants) sector_plants += patch::to_string(plant->getId()) + plant->getName().substr(0, 3) + ";";
	sector_plants.copy(sector_info.plants_aliases, (PLANTS_FIELD_LEN < sector_plants.length()) ? PLANTS_FIELD_LEN : sector_plants.length());
}

const uint8_t&  IrrigationSector::getId() const{
	return sector_info.id;
}

const uint8_t&  IrrigationSector::getPlantsCountLimit() const{
	return plants_count_limit;
}

uint8_t  IrrigationSector::getPlantsCount() const {
	return static_cast<uint8_t>(vPlants.size());
}

float IrrigationSector::getPlantHealth(const std::string& _name) const {
	for (auto &plant : vPlants) {
		if (_name.compare(plant->getName()) == 0) return plant->getMoisturePercent();
	}
	return -1;
}

float IrrigationSector::getPlantHealth(const uint8_t& _id) const {
	for (auto &plant : vPlants) {
		if (plant->getId() == _id) return plant->getMoisturePercent();
	}
	return -1;
}

void IrrigationSector::setWateringState(const bool& _watering_active) {
	watering = _watering_active;
}

bool& IrrigationSector::getWateringState(){
	return watering;
}

void IrrigationSector::update() {
	encodeState();
	encodeErrors();
	encodePlants();
}

struct IrrigationSectorInfo_s&	IrrigationSector::getInfo(){
	update();
	return sector_info;
}

bool IrrigationSector::setPlantMoistureByName(const std::string& _plant_name, const float& _moisture_percent){
	for (auto &plant : vPlants) {
		if (plant->getName() == _plant_name) {
			return plant->setMoisturePercent(_moisture_percent);
		}
	}
	return false;
}


void ConcreteIrrigationSectorBuilder::Reset() {
	sector.reset(new IrrigationSector(static_cast<uint8_t>(++sector_count)));
}
/**
 * All production steps work with the same sector instance.
 */

ConcreteIrrigationSectorBuilder& ConcreteIrrigationSectorBuilder::producePlantWithDMAMoistureSensor(const std::string& _p_name, const float& _ref_voltage, const uint32_t& _quantization_levels){
	uint8_t idx = static_cast<uint8_t>(sector->vPlants.size());
	if (idx <= sector->getPlantsCountLimit()) {
		sector->vPlants.emplace_back(std::unique_ptr<PlantInterface>(new PlantWithDMAMoistureSensor(new Plant(_p_name, idx), _ref_voltage, _quantization_levels)));
		sector->vPlants.shrink_to_fit();
	}
	return *this;
}

ConcreteIrrigationSectorBuilder& ConcreteIrrigationSectorBuilder::produceDRV8833PumpWithController(const pump_controller_mode_t& _controller_mode, const uint32_t& _idletime_required_seconds, const uint32_t& _runtime_limit_seconds, \
	const std::array<struct gpio_s, 2>& _pinout, const struct gpio_s& _led_pinout,
	const struct gpio_s& _fault_pinout, const struct gpio_s& _mode_pinout){

	sector->pump_controller.setMode(_controller_mode);
	const std::array<struct gpio_s, 4> pins = { _pinout[0], _pinout[1], 0, 0 };
	sector->pump_controller.createPump(pump_type_t::drv8833_dc, PumpId++, _idletime_required_seconds, _runtime_limit_seconds, pins, _led_pinout, _fault_pinout, _mode_pinout);
	return *this;
}

ConcreteIrrigationSectorBuilder& ConcreteIrrigationSectorBuilder::produceDRV8833PumpWithController(const pump_controller_mode_t& _controller_mode, const uint32_t& _idletime_required_seconds, const uint32_t& _runtime_limit_seconds, \
	const std::array<struct gpio_s, 4>& _pinout, const struct gpio_s& _led_pinout,
	const struct gpio_s& _fault_pinout, const struct gpio_s& _mode_pinout) {

	sector->pump_controller.setMode(_controller_mode);
	sector->pump_controller.createPump(pump_type_t::drv8833_dc, PumpId++, _idletime_required_seconds, _runtime_limit_seconds, _pinout, _led_pinout, _fault_pinout, _mode_pinout);
	return *this;
}

ConcreteIrrigationSectorBuilder& ConcreteIrrigationSectorBuilder::produceBinaryPumpWithController(const pump_controller_mode_t& _controller_mode, const uint32_t& _idletime_required_seconds, const uint32_t& _runtime_limit_seconds, \
	const struct gpio_s& _pinout, const struct gpio_s& _led){

	sector->pump_controller.setMode(_controller_mode);
	const std::array<struct gpio_s, 4> pins = { _pinout, 0, 0, 0 };
	this->sector->pump_controller.createPump(pump_type_t::binary, PumpId++, _idletime_required_seconds, _runtime_limit_seconds, pins, _led);
	return *this;
}

std::unique_ptr<IrrigationSector> ConcreteIrrigationSectorBuilder::GetProduct() {
	std::unique_ptr<IrrigationSector>(result);
	result = std::move(sector);
	Reset();
	return result;
}
