#include "pumps.h"


void encodePumpStatus(const struct PumpInfo_s & _pump, uint32_t & status) {

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

void decodePumpStatus(std::array<struct PumpInfo_s, 4> & a_pump, const std::bitset<32> & _status) {

	const std::bitset<32> pumpstatemask(0x0000000F);
	std::bitset<32> tmp;

	for (uint8_t i = 0; i < 4; i++)
	{
		tmp = _status;
		if (i > 0) tmp >>= 8 * i;
		tmp &= pumpstatemask;
		a_pump[i].id = i;
		a_pump[i].state = tmp.to_ulong();
		if (_status.test(6)) a_pump[i].forced = true;
		if (_status.test(7)) a_pump[i].cmd_consumed = true;
	}
}

//Pumps based on bridge pattern (WIP)

bool PumpImp::isValid() const {
	return state != pump_state_t::init ? true : false;
}

pump_type_t	PumpImp::getType() const {
	return this->type;
}

bool PumpImp::start() {
	bool success = false;
	this->status.forced = false;

	if (this->isRunning() == false) {

		this->setState(pump_state_t::running);
		this->resetIdletime();
		this->resetRuntime();
		success = true;
	}

	return success;
}

bool PumpImp::stop() {
	bool success = false;
	this->status.forced = false;

	if (this->isRunning() == true) {

		this->setState(pump_state_t::stopped);
		this->resetIdletime();
		this->resetRuntime();
		success = true;
	}

	return success;
}

bool PumpImp::init(const uint32_t& _idletime_required_seconds, const uint32_t& _runtime_limit_seconds) {
	this->idletime_required_seconds = _idletime_required_seconds;
	this->idletime_seconds = this->idletime_required_seconds;
	this->runtime_limit_seconds = _runtime_limit_seconds;
	this->stop();

	return true;
}

bool PumpImp::run(const double& _dt, bool& cmd_consumed, const pump_cmd_t& _cmd) { //TODO: implement generic algorithm
	cmd_consumed = true;
	return true;
}

void PumpImp::setState(const pump_state_t& _state) {
	this->state = _state;
	this->status.state = static_cast<uint32_t>(_state);

}
pump_state_t PumpImp::getState(void) const{
	return this->state;
}

bool PumpImp::isRunning(void) const{
	return this->getState() == pump_state_t::running || this->getState() == pump_state_t::reversing ? true : false;
}

struct PumpInfo_s PumpImp::getStatus(void) const{
	return this->status;
}

void PumpImp::resetRuntime(void) {
	this->runtime_seconds = 0.0;
}

void PumpImp::increaseRuntime(const double& _dt) {
	this->runtime_seconds += _dt;
}

double PumpImp::getRuntimeSeconds(void) const{
	return this->runtime_seconds;
}

void PumpImp::resetIdletime(void) {
	this->idletime_seconds = 0.0;
}

void PumpImp::increaseIdletime(const double & _dt) {
	this->idletime_seconds += _dt;
}

double PumpImp::getIdletimeSeconds(void) const{
	return this->idletime_seconds;
}



bool Pump::isValid() const {
	return imp_->isValid();
}

pump_type_t	Pump::getType() const {
	return imp_->getType();
}

bool Pump::start() {
	return imp_->start();
}

bool Pump::stop() {
	return imp_->stop();
}

bool Pump::init(const uint32_t& _idletime_required_seconds, const uint32_t& _runtime_limit_seconds) {
	return imp_->init(_idletime_required_seconds, _runtime_limit_seconds);
}

bool Pump::run(const double& _dt, bool& cmd_consumed, const pump_cmd_t& _cmd) {
	return imp_->run(_dt, cmd_consumed, _cmd);
}

void Pump::setState(const pump_state_t& _state) {
	imp_->setState(_state);
}

pump_state_t Pump::getState(void) const {
	return imp_->getState();
}

bool Pump::isRunning(void) const {
	return imp_->isRunning();
}

struct PumpInfo_s Pump::getStatus(void) const {
	return imp_->getStatus();
}

void Pump::resetRuntime(void) {
	imp_->resetRuntime();
}

void Pump::increaseRuntime(const double& _dt) {
	imp_->increaseRuntime(_dt);
}

double Pump::getRuntimeSeconds(void) const {
	return imp_->getRuntimeSeconds();
}

void Pump::resetIdletime(void) {
	imp_->resetIdletime();
}

void Pump::increaseIdletime(const double& _dt) {
	imp_->increaseIdletime(_dt);
}

double Pump::getIdletimeSeconds(void) const {
	return imp_->getIdletimeSeconds();
}


bool DRV8833DcPumpImp::init(const uint32_t& _idletime_required_seconds, const uint32_t& _runtime_limit_seconds, \
	const std::array<struct gpio_s, 2>& _pinout, const struct gpio_s& _led_pinout, \
	const struct gpio_s& _fault_pinout, const struct gpio_s& _mode_pinout) {

	bool success = true;

	this->fault.pin = _fault_pinout.pin;
	this->fault.port = _fault_pinout.port;
	this->mode.pin = _mode_pinout.pin;
	this->mode.port = _mode_pinout.port;
	this->led.pin = _led_pinout.pin;
	this->led.port = _led_pinout.port;
	this->idletime_seconds = this->idletime_required_seconds;
	this->runtime_limit_seconds = _runtime_limit_seconds;

	this->aIN[0].pin = _pinout[0].pin;
	this->aIN[0].port = _pinout[0].port;
	this->aIN[1].pin = _pinout[1].pin;
	this->aIN[1].port = _pinout[1].port;

	this->setEnable();

	if (this->isFault() == false) {
		HAL_GPIO_WritePin(this->aIN[0].port, this->aIN[0].pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(this->aIN[1].port, this->aIN[1].pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_RESET);
	}
	else {
		this->setState(pump_state_t::fault);
		success = false;
	}

	return success;
}

void DRV8833DcPumpImp::setEnable(void) {
	this->stop();
	HAL_GPIO_WritePin(this->mode.port, this->mode.pin, GPIO_PIN_SET);
}

bool DRV8833DcPumpImp::isFault(void) {
	return HAL_GPIO_ReadPin(this->fault.port, this->fault.pin) == GPIO_PIN_RESET ? true : false;
}

bool DRV8833DcPumpImp::run(const double& _dt, bool& cmd_consumed, const pump_cmd_t& _cmd) {
	cmd_consumed = false;
	bool fault = false;

	if (this->isFault() == true) this->setState(pump_state_t::fault);

	switch (this->getState()) {
	case pump_state_t::init:
		this->setState(pump_state_t::stopped);
		if (_cmd == pump_cmd_t::stop) cmd_consumed = true;
		else cmd_consumed = false;
		break;

	case pump_state_t::waiting:
		this->increaseIdletime(_dt);
		if (this->getIdletimeSeconds() > this->idletime_required_seconds) {
			if (_cmd == pump_cmd_t::start) this->start();
			else this->stop();
			cmd_consumed = true;
		}
		break;

	case pump_state_t::stopped:
		this->increaseIdletime(_dt);
		if (_cmd == pump_cmd_t::start) {
			if (this->getIdletimeSeconds() > this->idletime_required_seconds) {
				this->start();
				cmd_consumed = true;
			}
			else if (this->getIdletimeSeconds() <= this->idletime_required_seconds) {
				this->setState(pump_state_t::waiting);
			}
		}
		else {
			cmd_consumed = true;
		}
		break;

	case pump_state_t::running:
		this->increaseRuntime(_dt);
		if (_cmd == pump_cmd_t::start) {
			cmd_consumed = true;
		}
		else if (_cmd == pump_cmd_t::stop) {
			this->stop();
			cmd_consumed = true;
		}
		else {
			this->stop();
			cmd_consumed = false;
		}
		if (this->getRuntimeSeconds() > this->runtime_limit_seconds && this->status.forced == false) this->stop(); //TODO: how to handle force.... commands?
		break;

	case pump_state_t::reversing:
		this->increaseRuntime(_dt);
		if (_cmd == pump_cmd_t::start) {
			this->start();
			cmd_consumed = true;
		}
		else {
			cmd_consumed = true;
		}
		if (this->getRuntimeSeconds() > 30.0 && this->status.forced == false) this->stop(); //TODO: how to handle force.... commands?
		break;

	case pump_state_t::fault:
		fault = true;
		this->increaseIdletime(_dt);
		this->stop(); //TODO: or forcestop?
		if (_cmd == pump_cmd_t::stop) cmd_consumed = true;
		else cmd_consumed = false;
		break;

	case pump_state_t::sleep:
		this->increaseIdletime(_dt);
		this->stop(); //TODO: or forcestop?
		if (_cmd == pump_cmd_t::stop) cmd_consumed = true;
		else cmd_consumed = false;
		break;

	default:
		break;
	}

	return fault;

}

bool DRV8833DcPumpImp::start(void) {

	bool success = false;
	this->status.forced = false;

	if (this->isRunning() == false) {
		this->resetIdletime();
		this->resetRuntime();

		HAL_GPIO_WritePin(this->aIN[0].port, this->aIN[0].pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(this->aIN[1].port, this->aIN[1].pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_SET);
		success = true;

	}

	this->setState(pump_state_t::running);

	return success;
}

bool DRV8833DcPumpImp::stop(void) {

	bool success = false;
	this->status.forced = false;

	if (this->isRunning() == true) {
		this->resetIdletime();
		this->resetRuntime();

		HAL_GPIO_WritePin(this->aIN[0].port, this->aIN[0].pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(this->aIN[1].port, this->aIN[1].pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_RESET);
		success = true;

	}

	this->setState(pump_state_t::stopped);

	return success;
}

bool DRV8833DcPumpImp::reverse(void) {

	bool success = false;
	this->status.forced = false;

	this->resetIdletime();
	this->resetRuntime();

	HAL_GPIO_WritePin(this->aIN[0].port, this->aIN[0].pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(this->aIN[1].port, this->aIN[1].pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_SET);
	success = true;

	this->setState(pump_state_t::reversing);

	return success;
}

bool DRV8833DcPumpImp::forcestart(void) {

	bool success = true;
	this->status.forced = true;

	HAL_GPIO_WritePin(this->aIN[0].port, this->aIN[0].pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(this->aIN[1].port, this->aIN[1].pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_SET);
	this->setState(pump_state_t::running);

	if (this->isRunning() == false) {
		this->resetRuntime();
	}
	else {
		success = false;
	}

	return success;
}
bool DRV8833DcPumpImp::forcestop(void) {

	bool success = true;
	this->status.forced = true;

	HAL_GPIO_WritePin(this->aIN[0].port, this->aIN[0].pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(this->aIN[1].port, this->aIN[1].pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_RESET);
	this->setState(pump_state_t::stopped);

	if (this->isRunning() == true) {
		this->resetIdletime();
	}
	else {
		success = false;
	}

	return success;
}

bool DRV8833DcPumpImp::forcereverse(void) {

	bool success = true;
	this->status.forced = true;

	HAL_GPIO_WritePin(this->aIN[0].port, this->aIN[0].pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(this->aIN[1].port, this->aIN[1].pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(this->led.port, this->led.pin, GPIO_PIN_SET);
	this->setState(pump_state_t::reversing);

	if (this->isRunning() == false) {
		this->resetRuntime();
	}
	else {
		success = false;
	}

	return success;
}

void DRV8833DcPumpImp::setSleep(void) {
	this->stop();
	HAL_GPIO_WritePin(this->mode.port, this->mode.pin, GPIO_PIN_RESET);
	this->setState(pump_state_t::sleep);
}


/***********************************/
/*! PumpController class implementation */
/***********************************/

bool PumpController::update(const double& _dt, bool& _activate_watering) {

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
	 * |-------->(7) 1 if pumps_count is 0
	 *************************/

	if (this->pumps_count > 0)
	{
		switch (this->mode)
		{

		case pump_controller_mode_t::init:
			errcode.set(6, true);
			break;

		case pump_controller_mode_t::external:
			if (pPump != nullptr)
			{
				if (_activate_watering == true && pPump->getState() != pump_state_t::waiting)
				{
					fault = pPump->run(_dt, consumed, pump_cmd_t::start);
					if (consumed == false) errcode.set(0, true);
				}
				else if (_activate_watering == true && pPump->getState() == pump_state_t::waiting)
				{
					fault = pPump->run(_dt, consumed, pump_cmd_t::stop);
					if (consumed == false) errcode.set(0, true);
					errcode.set(2, true); //runtime timeout
					_activate_watering = false;
				}
				else if (_activate_watering == false)
				{
					fault = pPump->run(_dt, consumed, pump_cmd_t::stop); //TODO: impelment after refactor
				}
			}
			else errcode.set(5, true);
			break;

		case pump_controller_mode_t::manual:
			errcode.set(6, true);
			break;

		case pump_controller_mode_t::automatic:
			errcode.set(6, true);
			break;

		case pump_controller_mode_t::sleep:
			errcode.set(6, true);
			break;

		default:
			errcode.set(6, true);
			break;
		}

		if (pPump != nullptr) {
			if (pPump->getState() == pump_state_t::running) errcode.set(1, true);
		}

		if (fault == true) {
			if (++this->pump_fault_occurence_cnt > 0) errcode.set(3, true);
		}

	}
	else errcode.set(7, true);

	this->pump_encoded_status = static_cast<uint8_t>(errcode.to_ulong());

	return consumed;
}

bool PumpController::createPump(const pump_type_t& _pump_type, const uint8_t& _id, const uint32_t& _idletime_required_seconds, \
	const uint32_t& _runtime_limit_seconds, const std::array<struct gpio_s, 4>& _pinout, const struct gpio_s& _led_pinout, \
	const struct gpio_s& _fault_pinout, const struct gpio_s& _mode_pinout) {

	bool success = true;

	switch (_pump_type) {

	case pump_type_t::drv8833_dc:
		if (this->pumps_count < (this->pumps_limit + 1))
		{
			const std::array<struct gpio_s, 2> pins = { _pinout[0], _pinout[1] };
			pPump = new DRV8833DcPump(_id, _idletime_required_seconds, _runtime_limit_seconds, pins, _led_pinout, _fault_pinout, _mode_pinout);
			this->pumps_count++;
		}
		else
		{
			success = false;
		}
		break;

	case pump_type_t::drv8833_bldc:
		if (this->pumps_count < (this->pumps_limit + 1))
		{
			//this->pumps_count++;
			success = false; //TODO: not implemented yet
		}
		else
		{
			success = false;
		}
		break;

	case pump_type_t::binary:
		if (this->pumps_count < (this->pumps_limit + 1))
		{
			//this->pumps_count++;
			success = false; //TODO: not implemented yet
		}
		else
		{
			success = false;
		}
		break;

	case pump_type_t::generic:
		if (this->pumps_count < (this->pumps_limit + 1))
		{
			pPump = new Pump(_id, _idletime_required_seconds, _runtime_limit_seconds);
			this->pumps_count++;
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

bool PumpController::setMode(const pump_controller_mode_t & _mode) {

	bool changed = true;

	if (this->mode != _mode && _mode != pump_controller_mode_t::init)
	{
		this->mode = _mode;
	}
	else changed = false;

	return changed;
}

const pump_controller_mode_t&	PumpController::getMode(void) {
	return this->mode;
}

uint8_t& PumpController::getPumpStatusEncoded(void) {
	return this->pump_encoded_status;
}





