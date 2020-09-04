
#include <plants.h>

struct confirmation_s{
	uint8_t target;
	uint8_t target_id;
	uint8_t cmd;
	uint8_t subcmd1;
	uint8_t subcmd2;
	bool consumed;
};

/***********************************/
/*! Plants class implementation */
/***********************************/

void Plant::moisturePercentSet(const float & _soilmoisture){
	this->soilMoisture = _soilmoisture;
}

float& Plant::moisturePercentGet(void){
	return this->soilMoisture;
}

std::string& Plant::nameGet(void){
	return this->name;
}

void Plant::nameChange(const std::string & _new_name){
	this->name.assign(_new_name);
}

const uint8_t& Plant::idGet(void){
	return this->id;
}


/***********************************/
/*! IrrigationSector class implementation */
/***********************************/

const uint8_t& IrrigationSector:: getSector(void){
	return this->id;
}

bool IrrigationSector::createPlant(const std::string & _name, const uint8_t & _id){

	bool success = true;

	if (this->plantsCount <= this->plantsCountMax)
	{
		Plant temp_plant(_name, _id);
		this->vPlants.push_back(temp_plant);
		this->plantsCount++;
	}
	else
	{
		success = false;
	}

	return success;
}

uint8_t& IrrigationSector::update(const double & _dt){
	//get status of pump for now, later get overall sector status
	this->irrigationController.update(_dt, this->water_plants); //returns true if cmd consumed
	this->status = this->irrigationController.getPumpStatusEncoded();
	return this->status;
}

uint8_t& IrrigationSector::getPlantsCount(void){
	return this->plantsCount;
}

float IrrigationSector::getPlantHealth(const std::string & _name){

	float tempHealth = -1000.0;

	if(this->plantsCount > 0)
	{
		for (uint8_t i = 0; i < this->plantsCount; i++)
		{
			if (_name.compare(this->vPlants.at(i).nameGet())){
				return this->vPlants.at(i).moisturePercentGet();
			}
		}
	}

	return tempHealth;
}

void IrrigationSector::setMeasurements(uint16_t *_raw_adc_values_array, const uint8_t & _raw_adc_values_cnt){
	for (uint8_t i=0; i<_raw_adc_values_cnt; ++i){
		if (i < this->plantsCount){
			this->irrigationController.vDMAMoistureSensor.at(i).rawUpdate(_raw_adc_values_array[i]);
			this->vPlants.at(i).moisturePercentSet(this->irrigationController.vDMAMoistureSensor.at(i).percentGet());
		}
	}
}

float IrrigationSector::getPlantHealth(const uint8_t & _id){

	if(this->plantsCount > 0)
	{
		for (uint8_t i = 0; i < this->plantsCount; i++)
		{
			if (this->vPlants.at(i).idGet() == _id){
				return this->vPlants.at(i).moisturePercentGet();
			}
		}
	}

	return -1000;
}

std::string IrrigationSector::getPlantNameByID(const uint8_t & _id){

	if(this->plantsCount > 0)
	{
		for (uint8_t i = 0; i < this->plantsCount; ++i)
		{
			if (this->vPlants.at(i).idGet() == _id){
				return this->vPlants.at(i).nameGet();
			}
		}
	}

	return "";

}

struct sectorstatus_s	IrrigationSector::getInfo(void){

	struct sectorstatus_s sector_info;
	std::string plants;

	sector_info.id = this->id;
	sector_info.state = this->status;
	for(auto &plant : this->vPlants){
		plants += plant.nameGet();
		plants += ",";
	}
	std::strcpy (sector_info.plants, plants.c_str());
	return sector_info;
}

uint8_t& IrrigationSector::getStatus(void){
	return this->status;
}

pumpstate_t IrrigationSector::getPumpState(void){
	if 		(this->irrigationController.pBinPump != nullptr){
		return this->irrigationController.pBinPump->stateGet();
	}
	else if (this->irrigationController.p8833Pump != nullptr){
		return this->irrigationController.p8833Pump->stateGet();
	}
	else{
		return pumpstate_t::init;
	}
}

uint8_t IrrigationSector::getPumpStatusEncoded(void){
		return this->irrigationController.getPumpStatusEncoded();
}

void IrrigationSector::wateringSet(const bool & _activate_watering){
	this->water_plants = _activate_watering;
}

bool& IrrigationSector::wateringGet(void){
	return this->water_plants;
}

bool IrrigationSector::handleConfirmation(void){

	struct confirmation_s confirmation = {0x04, 0, 0x00, 0, 0, false};
	bool not_confirmed = true;

	if (this->water_plants == true and (this->getPumpState() == pumpstate_t::running or this->getPumpState() == pumpstate_t::waiting)){
		confirmation.target_id = this->getSector();
		confirmation.cmd = 0x10;
		confirmation.consumed = true;
		not_confirmed = xQueueSendToFront(confirmationsQueue, (void *)&confirmation, ( TickType_t ) 5) == pdTRUE ? false : true;

	}
	if (this->water_plants == false and this->getPumpState() == pumpstate_t::stopped){
		confirmation.target_id = this->getSector();
		confirmation.cmd = 0x11;
		confirmation.consumed = true;
		not_confirmed = xQueueSendToFront(confirmationsQueue, (void *)&confirmation, ( TickType_t ) 5) == pdTRUE ? false : true;
	}

	return not_confirmed;

}


