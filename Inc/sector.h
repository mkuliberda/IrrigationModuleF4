
#ifndef SECTOR_H_
#define SECTOR_H_

#include "freertos_memory.h"
#include <string>
#include "main.h"
#include <vector>
#include "plants.h"
#include "pumps.h"
#include <memory>

#define NAME_LENGTH 20
#define PLANTS_LENGTH 20

struct IrrigationSectorInfo_s {
	uint32_t state;
	uint32_t errors;
	uint8_t id;
	char plants[PLANTS_LENGTH];
};

/**
 * It makes sense to use the Builder pattern only when your sectors are quite
 * complex and require extensive configuration.
 *
 * Unlike in other creational patterns, different concrete builders can produce
 * unrelated sectors. In other words, results of various builders may not
 * always follow the same interface.
 */

class IrrigationSector{
private:
	const uint8_t 										plants_count_limit = 20;
	const float											plant_dry_level = 10;
	bool												watering;
	struct IrrigationSectorInfo_s						sector_info;

	void												encodeErrors() const;
	void												encodeState() const;


public:
	std::vector<std::unique_ptr<PlantInterface>>		vPlants;
	PumpController										pump_controller;

	const uint8_t&										getId();
	const uint8_t&										getPlantsCountLimit();
	uint8_t												getPlantsCount() const;
	float												getPlantHealth(const std::string& _name) const;
	float												getPlantHealth(const uint8_t& _id) const;
	void												setWateringState(const bool& _watering_active);
	const bool&											getWateringState();
	void												update() const;
	const struct IrrigationSectorInfo_s&				getInfo();
	void												addPlantNameToList(const std::string&& _plant_name) const;
	bool												setPlantMoistureByName(const std::string& _plant_name, const float& _moisture_percent) const;


	IrrigationSector(const uint8_t& _id){
		sector_info.id = _id;
	}
	IrrigationSector(const uint8_t&& _id){
		sector_info.id = std::move(_id);
	}
	~IrrigationSector() {}

	//To avoid runtime errors, delete copy constructor and copy assignment operator. If sth's wrong, compile time error will fire.
	IrrigationSector(IrrigationSector const &) = delete;
	IrrigationSector& operator=(IrrigationSector const&) = delete;

};


/**
 * The Builder interface specifies methods for creating the different parts of
 * the IrrigationSector objects.
 */
class IrrigationSectorBuilder{
    public:
    virtual ~IrrigationSectorBuilder(){}
    virtual IrrigationSectorBuilder& ProducePartA() =0;
    virtual IrrigationSectorBuilder& ProducePartB() =0;
    virtual IrrigationSectorBuilder& ProducePartC() =0;
	virtual IrrigationSectorBuilder& producePlantWithDMAMoistureSensor(const std::string& _p_name, const float& _ref_voltage = 3.3,
		const uint32_t& _quantization_levels = 4095) =0;
	virtual IrrigationSectorBuilder& produceDRV8833PumpWithController(const pump_controller_mode_t& _controller_mode, const uint32_t& _idletime_required_seconds,
		const uint32_t& _runtime_limit_seconds, const std::array<struct gpio_s, 2>& _pinout, const struct gpio_s& _led_pinout, 
		const struct gpio_s& _fault_pinout, const struct gpio_s& _mode_pinout) = 0;
	virtual IrrigationSectorBuilder& produceDRV8833PumpWithController(const pump_controller_mode_t& _controller_mode, const uint32_t& _idletime_required_seconds,
		const uint32_t& _runtime_limit_seconds, const std::array<struct gpio_s, 4>& _pinout, const struct gpio_s& _led_pinout,
		const struct gpio_s& _fault_pinout, const struct gpio_s& _mode_pinout) = 0;
	virtual IrrigationSectorBuilder& produceBinaryPumpWithController(const pump_controller_mode_t& _controller_mode, const uint32_t& _idletime_required_seconds,
		const uint32_t& _runtime_limit_seconds, const struct gpio_s& _pinout, const struct gpio_s& _led) = 0;
};
/**
 * The Concrete Builder classes follow the Builder interface and provide
 * specific implementations of the building steps. Your program may have several
 * variations of Builders, implemented differently.
 */
class ConcreteIrrigationSectorBuilder : public IrrigationSectorBuilder{
private:
	std::unique_ptr<IrrigationSector> sector;
	int8_t sector_count = -1;

    /**
     * A fresh builder instance should contain a blank sector object, which is
     * used in further assembly.
     */
public:

    ConcreteIrrigationSectorBuilder(){
        this->Reset();
    }

    ~ConcreteIrrigationSectorBuilder(){}

	//To avoid runtime errors, delete copy constructor and copy assignment operator. If sth's wrong, compile time error will fire.
	ConcreteIrrigationSectorBuilder(ConcreteIrrigationSectorBuilder const &) = delete;
	ConcreteIrrigationSectorBuilder& operator=(ConcreteIrrigationSectorBuilder const&) = delete;

	void														Reset();
	ConcreteIrrigationSectorBuilder&							ProducePartA() override;
	ConcreteIrrigationSectorBuilder&							ProducePartB() override;
	ConcreteIrrigationSectorBuilder&							ProducePartC() override;
	ConcreteIrrigationSectorBuilder&							producePlantWithDMAMoistureSensor(const std::string& _p_name, const float& _ref_voltage = 3.3,
																const uint32_t& _quantization_levels = 4095) override;
	ConcreteIrrigationSectorBuilder&							produceDRV8833PumpWithController(const pump_controller_mode_t& _controller_mode, const uint32_t& _idletime_required_seconds, \
																const uint32_t& _runtime_limit_seconds, const std::array<struct gpio_s, 2>& _pinout, const struct gpio_s& _led_pinout, \
																const struct gpio_s& _fault_pinout, const struct gpio_s& _mode_pinout) override;
	ConcreteIrrigationSectorBuilder&							produceDRV8833PumpWithController(const pump_controller_mode_t& _controller_mode, const uint32_t& _idletime_required_seconds, \
																const uint32_t& _runtime_limit_seconds, const std::array<struct gpio_s, 4>& _pinout, const struct gpio_s& _led_pinout, \
																const struct gpio_s& _fault_pinout, const struct gpio_s& _mode_pinout) override;
	ConcreteIrrigationSectorBuilder&							produceBinaryPumpWithController(const pump_controller_mode_t& _controller_mode, const uint32_t& _idletime_required_seconds, \
																const uint32_t& _runtime_limit_seconds, const struct gpio_s& _pinout, const struct gpio_s& _led) override;
	std::unique_ptr<IrrigationSector>							GetProduct();
};

#endif /* SECTOR_H_ */
