#ifndef IRRIGATION_SYSTEM_H_
#define IRRIGATION_SYSTEM_H_

#include <irrigation.h>
#include <plants.h>
#include <ds18b20.h>
#include <analog_moisture_sensor.h>
#include <optical_waterlevel_sensor.h>
#include <power.h>

#define NAME_LENGTH 20
#define PLANTS_LENGTH 20

struct sectorstatus_s {
	uint32_t state;
	uint8_t id;
	char plants[PLANTS_LENGTH];
};

class IrrigationSector{

private:

	const uint8_t 			id;
	const uint8_t 			plantsCountMax = 20;
	uint8_t 				plantsCount;
	std::vector<Plant> 		vPlants;
	uint8_t 				status;
	bool					water_plants;

public:

	PumpController irrigationController;

	IrrigationSector(const uint8_t & _id):
	id(_id),
	plantsCount(0),
	status(0),
	water_plants(false)
	{
	};

	~IrrigationSector()
	{
	}

	const uint8_t& 			getSector(void);
	bool 					createPlant(const std::string & _name, const uint8_t & _id);
	uint8_t& 				update(const double & _dt);
	uint8_t& 				getPlantsCount(void);
	struct sectorstatus_s	getInfo(void);
	float 					getPlantHealth(const std::string & _name);
	float 					getPlantHealth(const uint8_t & _id);
	std::string				getPlantNameByID(const uint8_t & _id);
	void					setMeasurements(uint16_t *_raw_adc_values_array, const uint8_t & _raw_adc_values_cnt);
	uint8_t& 				getStatus(void);
	pumpstate_t 			getPumpState(void);
	uint8_t					getPumpStatusEncoded(void);
	void					wateringSet(const bool & _activate_watering);
	bool&					wateringGet(void);
	bool 					handleConfirmation(void);

};

/*class IrrigationSystem{
	public:
    std::vector<std::string> parts;
    void ListParts()const{
        for (size_t i=0; i<parts.size(); ++i){
        	//TODO
        }
    }
};

class IrrigationSystemBuilder{
    public:
    virtual ~IrrigationSystemBuilder(){}
    virtual void ProduceSector() const = 0;
    virtual void ProducePlant() const = 0;
    virtual void ProducePlantWithMoistureSensor() const = 0;
    virtual void ProduceBattery() const = 0;
    virtual void ProduceWatertank() const = 0;
};*/




#endif
