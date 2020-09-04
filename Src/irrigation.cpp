/**
 * Irrigation control file with classes for sensors, tanks, pumps etc
 */

#include <irrigation.h>


/******************************/
/*! Pump class implementation */
/******************************/

bool Pump::init(void){
	return true;
}

pumpstate_t& Pump::stateGet(void){
	return this->state;
}

bool Pump::isRunning(void){
	return this->stateGet() == pumpstate_t::running || this->stateGet() == pumpstate_t::reversing ? true : false;
}

void Pump::run(const double & _dt){

}

struct pumpstatus_s& Pump::statusGet(void){
	return this->status;
}

pumptype_t& Pump::typeGet(void){
	return this->type;
}

void Pump::runtimeReset(void){
	this->runtimeSeconds = 0.0;
}

void Pump::runtimeIncrease(const double & _dt){
	this->runtimeSeconds += _dt;
}

double& Pump::runtimeGetSeconds(void){
	return this->runtimeSeconds;
}

void Pump::idletimeReset(void){
	this->idletimeSeconds = 0.0;
}

void Pump::idletimeIncrease(const double & _dt){
	this->idletimeSeconds += _dt;
}

double& Pump::idletimeGetSeconds(void){
	return this->idletimeSeconds;
}



/************************************/
/*! BinaryPump class implementation */
/************************************/

bool BinaryPump::init(const uint8_t & _id, const uint32_t & _idletimeRequiredSeconds, const uint32_t & _runtimeLimitSeconds, const struct gpio_s & _pinout, const struct gpio_s & _led){
	this->status.id = _id;
	this->pinout.pin = _pinout.pin;
	this->pinout.port = _pinout.port;
	this->led.pin = _led.pin;
	this->led.port = _led.port;
	this->idletimeRequiredSeconds = _idletimeRequiredSeconds;
	this->idletimeSeconds = this->idletimeRequiredSeconds;
	this->runtimeLimitSeconds = _runtimeLimitSeconds;
	HAL_GPIO_WritePin(this->pinout.port,this->pinout.pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_RESET);
	this->stateSet(pumpstate_t::stopped);

	return true;
}

void BinaryPump::run(const double & _dt, const pumpcmd_t & _cmd, bool & cmd_consumed){

	if(_cmd != pumpcmd_t::reverse){

		switch (this->stateGet())
		{
		case pumpstate_t::init:
			this->stop();
			cmd_consumed = true;
			break;

		case pumpstate_t::waiting:
			this->idletimeIncrease(_dt);
			if(this->idletimeGetSeconds() > this->idletimeRequiredSeconds){
				this->start();
				cmd_consumed = true;
			}

			break;

		case pumpstate_t::stopped:
			this->idletimeIncrease(_dt);
			if((_cmd == pumpcmd_t::start) && (this->idletimeGetSeconds() > this->idletimeRequiredSeconds)){
				this->start();
				cmd_consumed = true;
			}
			else if((_cmd == pumpcmd_t::start) && (this->idletimeGetSeconds() <= this->idletimeRequiredSeconds)){
				this->stateSet(pumpstate_t::waiting);
			}
			break;

		case pumpstate_t::running:
			this->runtimeIncrease(_dt);
			if(_cmd == pumpcmd_t::start){
				cmd_consumed = true;
			}
			else{
				this->stop();
				cmd_consumed = true;
			}
			if(this->runtimeGetSeconds() > this->runtimeLimitSeconds && this->status.forced == false) this->stop(); //TODO: how to handle force.... commands?
			break;

		default:
			break;
		}
	}
	else{
		//Invalid command, not consumed, let's stop pump for safety
		this->stop();
	}
}

bool BinaryPump::start(void){

	bool success = false;
	this->status.forced = false;

	if(this->isRunning() == false){

		HAL_GPIO_WritePin(this->pinout.port,this->pinout.pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_SET);
		this->stateSet(pumpstate_t::running);
		this->idletimeReset();
		this->runtimeReset();
		success = true;
	}

	return success;
}

bool BinaryPump::stop(void){

	bool success = false;
	this->status.forced = false;

	if(this->isRunning() == true){

		HAL_GPIO_WritePin(this->pinout.port,this->pinout.pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_RESET);
		this->stateSet(pumpstate_t::stopped);
		this->idletimeReset();
		this->runtimeReset();
		success = true;
	}

	return success;
}

void BinaryPump::forcestart(void){

	if(this->isRunning() == false) this->runtimeReset();

	HAL_GPIO_WritePin(this->pinout.port,this->pinout.pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_SET);
	this->stateSet(pumpstate_t::running);
	this->status.forced = true;
}
void BinaryPump::forcestop(void){

	if (this->isRunning() == true) this->idletimeReset();

	HAL_GPIO_WritePin(this->pinout.port,this->pinout.pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_RESET);
	this->stateSet(pumpstate_t::stopped);
	this->status.forced = true;
}

void BinaryPump::stateSet(const pumpstate_t & _state){
	this->state = _state;
	this->status.state = static_cast<uint32_t>(_state);
}


/************************************/
/*! DRV8833Pump class implementation */
/************************************/

bool DRV8833Pump::init(const uint8_t & _id, const uint32_t & _idletimeRequiredSeconds, const uint32_t & _runtimeLimitSeconds, \
		const std::array<struct gpio_s, 4> & _pinout, const struct gpio_s & _led_pinout, \
		const struct gpio_s & _fault_pinout, const struct gpio_s & _mode_pinout){

	bool success = true;

	this->status.id = _id;
	this->fault.pin = _fault_pinout.pin;
	this->fault.port = _fault_pinout.port;
	this->mode.pin = _mode_pinout.pin;
	this->mode.port = _mode_pinout.port;
	this->led.pin = _led_pinout.pin;
	this->led.port = _led_pinout.port;
	this->idletimeRequiredSeconds = _idletimeRequiredSeconds;
	this->idletimeSeconds = this->idletimeRequiredSeconds;
	this->runtimeLimitSeconds = _runtimeLimitSeconds;

	switch (this->typeGet()){
	case pumptype_t::drv8833_dc:
		success = false;
		break;
	case pumptype_t::drv8833_bldc:
		this->aIN[0].pin = _pinout[0].pin;
		this->aIN[0].port = _pinout[0].port;
		this->aIN[1].pin = _pinout[1].pin;
		this->aIN[1].port = _pinout[1].port;
		this->aIN[2].pin = _pinout[2].pin;
		this->aIN[2].port = _pinout[2].port;
		this->aIN[3].pin = _pinout[3].pin;
		this->aIN[3].port = _pinout[3].port;
		break;
	default:
		success = false;
		break;
	}

	this->setEnable();

	if (this->isFault() == false){
		//TODO:
	}
	else{
		this->stateSet(pumpstate_t::fault);
		success = false;
	}

	return success;
}

bool DRV8833Pump::init(const uint8_t & _id, const uint32_t & _idletimeRequiredSeconds, const uint32_t & _runtimeLimitSeconds, \
		const std::array<struct gpio_s, 2> & _pinout, const struct gpio_s & _led_pinout, \
		const struct gpio_s & _fault_pinout, const struct gpio_s & _mode_pinout){

	bool success = true;

	this->status.id = _id;
	this->fault.pin = _fault_pinout.pin;
	this->fault.port = _fault_pinout.port;
	this->mode.pin = _mode_pinout.pin;
	this->mode.port = _mode_pinout.port;
	this->led.pin = _led_pinout.pin;
	this->led.port = _led_pinout.port;
	this->idletimeRequiredSeconds = _idletimeRequiredSeconds;
	this->idletimeSeconds = this->idletimeRequiredSeconds;
	this->runtimeLimitSeconds = _runtimeLimitSeconds;

	switch (this->typeGet()){
	case pumptype_t::drv8833_dc:
		this->aIN[0].pin = _pinout[0].pin;
		this->aIN[0].port = _pinout[0].port;
		this->aIN[1].pin = _pinout[1].pin;
		this->aIN[1].port = _pinout[1].port;
		this->aIN[2].pin = 0;
		this->aIN[2].port = nullptr;
		this->aIN[3].pin = 0;
		this->aIN[3].port = nullptr;
		break;
	case pumptype_t::drv8833_bldc:
		success = false;
		break;
	default:
		success = false;
		break;
	}

	this->setEnable();

	if (this->isFault() == false){
		HAL_GPIO_WritePin(this->aIN[0].port, this->aIN[0].pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(this->aIN[1].port, this->aIN[1].pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_RESET);
	}
	else{
		this->stateSet(pumpstate_t::fault);
		success = false;
	}

	return success;
}

void DRV8833Pump::run(const double & _dt, const pumpcmd_t & _cmd, bool & cmd_consumed, bool & fault){

	if (this->isFault() == true) this->stateSet(pumpstate_t::fault);

	switch (this->stateGet()){
	case pumpstate_t::init:
		this->stateSet(pumpstate_t::stopped);
		if (_cmd == pumpcmd_t::stop) cmd_consumed = true;
		else cmd_consumed = false;
		break;

	case pumpstate_t::waiting:
		this->idletimeIncrease(_dt);
		if(this->idletimeGetSeconds() > this->idletimeRequiredSeconds){
			if (_cmd == pumpcmd_t::start) this->start();
			else this->stop();
			cmd_consumed = true;
		}
		break;

	case pumpstate_t::stopped:
		this->idletimeIncrease(_dt);
		if (_cmd == pumpcmd_t::start){
			if (this->idletimeGetSeconds() > this->idletimeRequiredSeconds){
				this->start();
				cmd_consumed = true;
			}
			else if (this->idletimeGetSeconds() <= this->idletimeRequiredSeconds){
				this->stateSet(pumpstate_t::waiting);
			}
		}
		else{
			cmd_consumed = true;
		}
		break;

	case pumpstate_t::running:
		this->runtimeIncrease(_dt);
		if(_cmd == pumpcmd_t::start){
			cmd_consumed = true;
		}
		else if (_cmd == pumpcmd_t::stop){
			this->stop();
			cmd_consumed = true;
		}
		else{
			this->stop();
			cmd_consumed = false;
		}
		if(this->runtimeGetSeconds() > this->runtimeLimitSeconds && this->status.forced == false) this->stop(); //TODO: how to handle force.... commands?
		break;

	case pumpstate_t::reversing:
		this->runtimeIncrease(_dt);
		if(_cmd==pumpcmd_t::start){
			this->start();
			cmd_consumed = true;
		}
		else {
			cmd_consumed = true;
		}
		if(this->runtimeGetSeconds() > 30.0 && this->status.forced == false) this->stop(); //TODO: how to handle force.... commands?
		break;

	case pumpstate_t::fault:
		fault = true;
		this->idletimeIncrease(_dt);
		this->stop(); //TODO: or forcestop?
		if (_cmd == pumpcmd_t::stop) cmd_consumed = true;
		else cmd_consumed = false;
		break;

	case pumpstate_t::sleep:
		this->idletimeIncrease(_dt);
		this->stop(); //TODO: or forcestop?
		if (_cmd == pumpcmd_t::stop) cmd_consumed = true;
		else cmd_consumed = false;
		break;

	default:
		break;
	}


}

bool DRV8833Pump::start(void){

	bool success = false;
	this->status.forced = false;

	if(this->isRunning() == false){
		this->idletimeReset();
		this->runtimeReset();

		switch (this->typeGet()){
		case pumptype_t::drv8833_dc:
			HAL_GPIO_WritePin(this->aIN[0].port, this->aIN[0].pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(this->aIN[1].port, this->aIN[1].pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_SET);
			success = true;
			break;
		case pumptype_t::drv8833_bldc:
			//TODO:
			break;
		default:
			break;
		}
	}

	this->stateSet(pumpstate_t::running);

	return success;
}

bool DRV8833Pump::stop(void){

	bool success = false;
	this->status.forced = false;

	if(this->isRunning() == true){
		this->idletimeReset();
		this->runtimeReset();

		switch (this->typeGet()){
		case pumptype_t::drv8833_dc:
			HAL_GPIO_WritePin(this->aIN[0].port, this->aIN[0].pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(this->aIN[1].port, this->aIN[1].pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_RESET);
			success = true;
			break;
		case pumptype_t::drv8833_bldc:
			//TODO:
			break;
		default:
			break;
		}
	}

	this->stateSet(pumpstate_t::stopped);

	return success;
}

bool DRV8833Pump::reverse(void){

	bool success = false;
	this->status.forced = false;


	this->idletimeReset();
	this->runtimeReset();

	switch (this->typeGet()){
	case pumptype_t::drv8833_dc:
		HAL_GPIO_WritePin(this->aIN[0].port, this->aIN[0].pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(this->aIN[1].port, this->aIN[1].pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_SET);
		success = true;
		break;
	case pumptype_t::drv8833_bldc:
		//TODO:
		break;
	default:
		break;
	}

	this->stateSet(pumpstate_t::reversing);


	return success;

}

bool DRV8833Pump::forcestart(void){

	bool success = true;

	this->status.forced = true;

	switch (this->typeGet()){
	case pumptype_t::drv8833_dc:
		HAL_GPIO_WritePin(this->aIN[0].port, this->aIN[0].pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(this->aIN[1].port, this->aIN[1].pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_SET);
		this->stateSet(pumpstate_t::running);
		break;
	case pumptype_t::drv8833_bldc:
		//TODO:
		success = false;
		break;
	default:
		success = false;
		break;
	}

	if(this->isRunning() == false){
		this->runtimeReset();
	}
	else{
		success = false;
	}

	return success;
}
bool DRV8833Pump::forcestop(void){

	bool success = true;

	this->status.forced = true;

	switch (this->typeGet()){
	case pumptype_t::drv8833_dc:
		HAL_GPIO_WritePin(this->aIN[0].port, this->aIN[0].pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(this->aIN[1].port, this->aIN[1].pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_RESET);
		this->stateSet(pumpstate_t::stopped);
		break;
	case pumptype_t::drv8833_bldc:
		//TODO:
		success = false;
		break;
	default:
		success = false;
		break;
	}

	if(this->isRunning() == true){
		this->idletimeReset();
	}
	else{
		success = false;
	}

	return success;
}

bool DRV8833Pump::forcereverse(void){

	bool success = true;

	this->status.forced = true;

	switch (this->typeGet()){
	case pumptype_t::drv8833_dc:
		HAL_GPIO_WritePin(this->aIN[0].port, this->aIN[0].pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(this->aIN[1].port, this->aIN[1].pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_SET);
		this->stateSet(pumpstate_t::reversing);
		break;
	case pumptype_t::drv8833_bldc:
		//TODO:
		success = false;
		break;
	default:
		success = false;
		break;
	}

	if(this->isRunning() == false){
		this->runtimeReset();
	}
	else{
		success = false;
	}

	return success;
}

void DRV8833Pump::setSleep(void){
	this->stop();
	HAL_GPIO_WritePin(this->mode.port, this->mode.pin, GPIO_PIN_RESET);
	this->stateSet(pumpstate_t::sleep);
}

void DRV8833Pump::setEnable(void){
	this->stop();
	HAL_GPIO_WritePin(this->mode.port, this->mode.pin, GPIO_PIN_SET);
}
bool DRV8833Pump::isFault(void){
	return HAL_GPIO_ReadPin(this->fault.port, this->fault.pin) == GPIO_PIN_RESET ? true : false;
}

void DRV8833Pump::stateSet(const pumpstate_t & _state){
	this->state = _state;
	this->status.state = static_cast<uint32_t>(_state);
}

/***********************************/
/*! WaterTank class implementation */
/***********************************/

bool WaterTank::init(void){
	return true;
}

float& WaterTank::temperatureCelsiusGet(void){
	return this->mean_watertemperatureCelsius;
}

void WaterTank::waterlevelSet(const contentlevel_t & _waterlevel){
	this->waterlevel= _waterlevel;
}

WaterTank::contentlevel_t& WaterTank::waterlevelGet(void){
	return this->waterlevel;
}

void WaterTank::stateSet(const contentstate_t & _state){
	this->waterstate = _state;
}

WaterTank::contentstate_t& WaterTank::stateGet(void){
	return this->waterstate;
}

bool WaterTank::checkStateOK(const double & _dt, uint32_t & errcodeBitmask){

	/******************************errcodeBitmask****************************************
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

	uint8_t temp_waterlevelPercent = 0;
	uint8_t waterlevelPercent = 0;
	bool isOK = true;
	bool tempReadingValid = false;
	std::bitset<32> errcode; errcode.set();  //initialize bitset and set all bits to 1


	if (this->temperatureSensorsCount > 0){

		std::vector <float> vTemperature;

		for(uint8_t i = 0; i < this->temperatureSensorsCount; i++){
			if(this->vTemperatureSensors[i].isValid() == true){
				float temp = this->vTemperatureSensors[i].temperatureCelsiusRead(_dt);
				vTemperature.push_back(temp);
				errcode.reset(19+i);
				tempReadingValid = true;
			}
		}

		if (tempReadingValid == true){
			this->mean_watertemperatureCelsius = (accumulate(vTemperature.begin(), vTemperature.end(), 0))/this->temperatureSensorsCount;

			if(this->mean_watertemperatureCelsius < 0.0){
				this->stateSet(contentstate_t::frozen);
				errcode.reset(17);
				isOK = false;
			}
			else if (this->mean_watertemperatureCelsius > 100.0)
			{
				this->stateSet(contentstate_t::boiling);
				errcode.reset(16);
				isOK = false;
			}
			else{
				this->stateSet(contentstate_t::liquid);
				errcode.reset(16);
				errcode.reset(17);
			}
		}
		else{
			//if temperature reading isn't valid then don't update state
		}
	}
	else{
		//isOK = false;//let's let it work without temperature sensor
	}


	if (this->waterlevelSensorsCount > 0){
		for(uint8_t i=0; i<this->waterlevelSensorsCount; i++){
			if(this->vOpticalWLSensors[i].isValid() == true){
				errcode.reset(22+i);
				if (this->vOpticalWLSensors[i].isSubmersed()){
					temp_waterlevelPercent = this->waterlevelConvertToPercent(this->vOpticalWLSensors[i].mountpositionGet());
					if(temp_waterlevelPercent > waterlevelPercent) waterlevelPercent = temp_waterlevelPercent;
				}
			}
		}
	}

	if		(waterlevelPercent >= 98) 	{ this->waterlevelSet(contentlevel_t::full); errcode.reset(18); }
	else if	(waterlevelPercent > 90) 	{ this->waterlevelSet(contentlevel_t::above90); errcode.reset(18); }
	else if (waterlevelPercent > 80) 	{ this->waterlevelSet(contentlevel_t::above80); errcode.reset(18); }
	else if (waterlevelPercent > 70) 	{ this->waterlevelSet(contentlevel_t::above70); errcode.reset(18); }
	else if (waterlevelPercent > 60) 	{ this->waterlevelSet(contentlevel_t::above60); errcode.reset(18); }
	else if (waterlevelPercent > 50) 	{ this->waterlevelSet(contentlevel_t::above50); errcode.reset(18); }
	else if (waterlevelPercent > 40) 	{ this->waterlevelSet(contentlevel_t::above40); errcode.reset(18); }
	else if (waterlevelPercent > 30) 	{ this->waterlevelSet(contentlevel_t::above30); errcode.reset(18); }
	else if (waterlevelPercent > 20) 	{ this->waterlevelSet(contentlevel_t::above20); errcode.reset(18); }
	else if (waterlevelPercent > 10) 	{ this->waterlevelSet(contentlevel_t::above10); errcode.reset(18); }
	else if (waterlevelPercent >= 0) 	{ this->waterlevelSet(contentlevel_t::empty); isOK = false; }

	errcodeBitmask = errcode.to_ulong();

	return isOK;
}

uint8_t WaterTank::waterlevelConvertToPercent(const float & _valMeters){
	return static_cast<uint8_t>(_valMeters/this->tankheightMeters*100);
}

uint8_t WaterTank::waterlevelPercentGet(void){
	return static_cast<uint8_t>(this->waterlevel);
}

bool WaterTank::waterlevelSensorCreate(const waterlevelsensortype_t & _sensortype){

	bool success = true;

	switch (_sensortype)
	{
	case waterlevelsensortype_t::optical:
		if (this->waterlevelSensorsCount < (this->waterlevelSensorsLimit+1))
		{
			OpticalWaterLevelSensor temp_sensor;
			this->vOpticalWLSensors.push_back(temp_sensor);
			this->waterlevelSensorsCount++;
		}
		else
		{
			success = false;
		}
		break;

	case waterlevelsensortype_t::capacitive:
		success = false;
		break;

	case waterlevelsensortype_t::resistive:
		success = false;
		break;

	default:
		success = false;
		break;
	}

	return success;
}

bool WaterTank::temperatureSensorCreate(const temperaturesensortype_t & _sensortype){

	bool success = true;

	switch (_sensortype)
	{
	case temperaturesensortype_t::ds18b20:
		if (this->temperatureSensorsCount < (this->temperatureSensorsLimit+1))
		{
			DS18B20 temp_sensor;
			this->vTemperatureSensors.push_back(temp_sensor);
			this->temperatureSensorsCount++;
		}
		else
		{
			success = false;
		}
		break;

	case temperaturesensortype_t::generic:
		success = false;
		break;

	default:
		success = false;
		break;
	}

	return success;
}

uint8_t & WaterTank::idGet(void){
	return this->id;
}

/***********************************/
/*! PumpController class implementation */
/***********************************/

bool PumpController::update(const double & _dt, bool & _activate_watering){

	bool consumed = false;
	bool fault = false;
	std::bitset<8> errcode;
	/*******errcode**********
	 * 00000000
	 * ||||||||->(0) 1 if cmd not consumed
	 * |||||||-->(1) 1 if active, 0 if stopped
	 * ||||||--->(2) 1 if runtime timeout
	 * |||||---->(3) 1 if fault occurred at least once
	 * ||||----->(4)
	 * |||------>(5) 1 if none of avbl pumps was correctly initialized/created
	 * ||------->(6) 1 if controller is in wrong or not avbl mode
	 * |-------->(7) 1 if pumpsCount is 0
	 *************************/

	if(this->pumpsCount > 0)
	{
		switch (this->mode)
		{

		case pumpcontrollermode_t::init:
			errcode.set(6,true);
			break;

		case pumpcontrollermode_t::external:
				if (this->pBinPump != nullptr)
				{
					if(_activate_watering)
					{
						this->pBinPump->run(_dt, pumpcmd_t::start, consumed);
						if(consumed == false) errcode.set(0,true);
					}
					else
					{
						this->pBinPump->run(_dt, pumpcmd_t::stop, consumed);
						if(consumed == false) errcode.set(0,true);
					}
				}
				else if (this->p8833Pump != nullptr)
				{
					if(_activate_watering == true && this->p8833Pump->stateGet() != pumpstate_t::waiting)
					{
						this->p8833Pump->run(_dt, pumpcmd_t::start, consumed, fault);
						if(consumed == false) errcode.set(0,true);
					}
					else if (_activate_watering == true && this->p8833Pump->stateGet() == pumpstate_t::waiting)
					{
						this->p8833Pump->run(_dt, pumpcmd_t::stop, consumed, fault);
						if(consumed == false) errcode.set(0,true);
						errcode.set(2,true); //runtime timeout
						_activate_watering = false;
					}
					else if (_activate_watering == false)
					{
						this->p8833Pump->run(_dt, pumpcmd_t::stop, consumed, fault);
					}
				}
				else errcode.set(5,true);

			break;

		case pumpcontrollermode_t::manual:
			errcode.set(6,true);
			break;

		case pumpcontrollermode_t::automatic:
			errcode.set(6,true);
			break;

		case pumpcontrollermode_t::sleep:
			errcode.set(6,true);
			break;

		default:
			errcode.set(6,true);
			break;
		}

		if (this->pBinPump != nullptr)
		{
			if (this->pBinPump->stateGet() == pumpstate_t::running) errcode.set(1,true);
		}
		else if (this->p8833Pump != nullptr)
		{
			if (this->p8833Pump->stateGet() == pumpstate_t::running) errcode.set(1,true);
		}

		if (fault == true){
			if (++this->pumpFaultOccurenceCnt > 0) errcode.set(3, true);
		}

	}
	else errcode.set(7,true);

	this->pumpEncodedStatus = static_cast<uint8_t>(errcode.to_ulong());

	return consumed;
}

bool PumpController::pumpCreate(const pumptype_t & _pumptype){

	bool success = true;

	switch(_pumptype){
	case pumptype_t::binary:
		if (this->pumpsCount < (this->pumpsLimit+1))
		{
			this->pBinPump = new BinaryPump();
			this->pumpsCount++;
		}
		else
		{
			success = false;
		}
		break;

	case pumptype_t::generic:
		success = false;
		break;

	case pumptype_t::drv8833_dc:
		if (this->pumpsCount < (this->pumpsLimit+1))
		{
			this->p8833Pump = new DRV8833Pump(motortype_t::dc_motor);
			this->pumpsCount++;
		}
		else
		{
			success = false;
		}
		break;

	case pumptype_t::drv8833_bldc:
		if (this->pumpsCount < (this->pumpsLimit+1))
		{
			this->p8833Pump = new DRV8833Pump(motortype_t::bldc_motor);
			this->pumpsCount++;
		}
		else
		{
			success = false;
		}
		break;

	default:
		success = false;
		break;
	}

	return success;
}

bool PumpController::moisturesensorCreate(const moisturesensortype_t & _sensortype){
	bool success = true;

	switch(_sensortype){
	case moisturesensortype_t::generic:
		success = false;
		break;

	case moisturesensortype_t::capacitive_noshield:
		if (this->moisturesensorsCount < (this->moisturesensorsLimit+1))
		{
			AnalogDMAMoistureSensor temp_sensor;
			this->vDMAMoistureSensor.push_back(temp_sensor);
			this->moisturesensorsCount++;
		}
		else
		{
			success = false;
		}
		break;

	default:
		success = false;
		break;
	}

	return success;
}

bool PumpController::modeSet(const pumpcontrollermode_t & _mode){

	bool changed = true;

	if (this->mode != _mode && _mode != pumpcontrollermode_t::init)
	{
		this->mode = _mode;
	}
	else changed = false;

	return changed;
}

const pumpcontrollermode_t&	PumpController::modeGet(void){
	return this->mode;
}

uint8_t& PumpController::getPumpStatusEncoded(void){
	return this->pumpEncodedStatus;
}

void pumpStateEncode(const struct pumpstatus_s & _pump, uint32_t & status) {

	switch (_pump.id)
	{
	case 0:
		status |= _pump.state;
		if (_pump.forced == true) 			status |= (1 << 6);
		if (_pump.cmd_consumed == true) 	status |= (1 << 7);
		break;
	case 1:
		status |= _pump.state << 8;
		if (_pump.forced == true) 			status |= (1 << 14);
		if (_pump.cmd_consumed == true) 	status |= (1 << 15);
		break;
	case 2:
		status |= (_pump.state << 16);
		if (_pump.forced == true) 			status |= (1 << 22);
		if (_pump.cmd_consumed == true) 	status |= (1 << 23);
		break;
	case 3:
		status |= (_pump.state << 24);
		if (_pump.forced == true) 			status |= (1 << 30);
		if (_pump.cmd_consumed == true) 	status |= (1 << 31);
		break;
	default:
		break;
	}

}

void pumpStateDecode(std::array<struct pumpstatus_s,4> & a_pump, const std::bitset<32> & _status) {

	const std::bitset<32> pumpstatemask(0x0000000F);
	std::bitset<32> tmp;

	for (uint8_t i = 0; i < 4; i++)
	{
		tmp = _status;
		if(i>0) tmp >>= 8*i;
		tmp &= pumpstatemask;
		a_pump[i].id = i;
		a_pump[i].state = tmp.to_ulong();
		if (_status.test(6)) a_pump[i].forced = true;
		if (_status.test(7)) a_pump[i].cmd_consumed = true;
	}
}


