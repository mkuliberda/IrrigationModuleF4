/*
 * battery_manager.h
 *
 *  Created on: 18.04.2020
 *      Author: Mati
 */

#ifndef POWER_H_
#define POWER_H_

#include "stm32f4xx_hal.h"
#include <utilities.h>
#include <vector>
#include <bitset>

enum class batterytype_t: uint8_t{
	undefined,
	lipo,
	lipo_hv,
	liion,
	liion_hv,
	nimh
};

enum class batterystate_t: uint8_t{
	undetermined,
	charging,
	discharging
};

struct batteryerror_t{
	uint8_t overvoltage 	: 1;
	uint8_t overdischarge 	: 1;
	uint8_t overheated 		: 1;
	uint8_t overloaded 		: 1;
	uint8_t calc_error		: 1;
	uint8_t iface_error		: 1;
	uint8_t reserved1		: 1;
	uint8_t reserved2		: 1;
};

enum class batteryinterface_t: uint8_t{
	undefined,
	adc,
	i2c,
	can,
	spi
};

struct batterystatus_s{
	uint16_t remaining_time_min;
	uint8_t id;
	uint8_t percentage;
	uint8_t status;
};


class Battery{

private:
	const uint8_t id;
	float cell_voltage_min;
	float cell_voltage_max;
	float voltage;
	float voltage_previous;
	float current;
	float percentage;
	bool valid = false;
	const batterytype_t type = batterytype_t::undefined;
	batterystate_t state = batterystate_t::undetermined;
	struct batteryerror_t errors;
	uint16_t remaining_time_min;
	const batteryinterface_t interface;
	const uint8_t  cell_count;
	const float capacity_full;
	float internal_resistance;
	float adc_voltage_divider_error_factor = 1.0;
	float adc_reference_voltage = 3.0;
	uint32_t adc_levels = 4095;
	float dt_from_last_calc = 0;
	const float calc_timespan_s = 300;  //5min timespan
	static const uint8_t voltage_buffer_length = 30;
	float voltage_buffer[voltage_buffer_length];
	uint8_t voltage_buffer_counter = 0;


	void 												calculatePercentage(void);
	void												determineState(const float &_dt);
	void												calculateRemainingTimeMinutes(const float &_dt);
	void												determineErrors(void);
	void												updateBuffer(const float & _voltage);

public:
	Battery(const uint8_t &_id, const batterytype_t & _type, const batteryinterface_t &_interface, const uint8_t & _cell_count):
	id(_id),
	type(_type),
	interface(_interface),
	cell_count(_cell_count),
	capacity_full(-1000),
	internal_resistance(-1000)
	{
		switch(_type){
		case batterytype_t::lipo:
			this->cell_voltage_max = this->cell_count * 4.2;
			this->cell_voltage_min = this->cell_count * 3.0;
			break;
		case batterytype_t::lipo_hv:
			this->cell_voltage_max = this->cell_count * 4.35;
			this->cell_voltage_min = this->cell_count * 3.0;
			break;
		case batterytype_t::liion:
			this->cell_voltage_max = this->cell_count * 4.2;
			this->cell_voltage_min = this->cell_count * 3.0;
			break;
		case batterytype_t::liion_hv:
			this->cell_voltage_max = this->cell_count * 4.35;
			this->cell_voltage_min = this->cell_count * 3.0;
			break;
		case batterytype_t::nimh:
			this->cell_voltage_max = this->cell_count * 1.5;
			this->cell_voltage_min = this->cell_count * 1.0;
			break;
		default:
			this->valid = false;
			break;
		}
	}
	Battery(const uint8_t &_id, const batterytype_t & _type, const batteryinterface_t &_interface, const uint8_t & _cell_count, const float & _capacity_full):
	id(_id),
	type(_type),
	interface(_interface),
	cell_count(_cell_count),
	capacity_full(_capacity_full),
	internal_resistance(-1000)
	{
		switch(_type){
		case batterytype_t::lipo:
			this->cell_voltage_max = this->cell_count * 4.2;
			this->cell_voltage_min = this->cell_count * 3.0;
			break;
		case batterytype_t::lipo_hv:
			this->cell_voltage_max = this->cell_count * 4.35;
			this->cell_voltage_min = this->cell_count * 3.0;
			break;
		case batterytype_t::liion:
			this->cell_voltage_max = this->cell_count * 4.2;
			this->cell_voltage_min = this->cell_count * 3.0;
			break;
		case batterytype_t::liion_hv:
			this->cell_voltage_max = this->cell_count * 4.35;
			this->cell_voltage_min = this->cell_count * 3.0;
			break;
		case batterytype_t::nimh:
			this->cell_voltage_max = this->cell_count * 1.5;
			this->cell_voltage_min = this->cell_count * 1.0;
			break;
		default:
			this->valid = false;
			break;
		}
	}
	Battery(const uint8_t &_id, const batterytype_t & _type, const batteryinterface_t &_interface, const uint8_t & _cell_count, const float & _capacity_full, const float &_internal_resistance):
	id(_id),
	type(_type),
	interface(_interface),
	cell_count(_cell_count),
	capacity_full(_capacity_full),
	internal_resistance(_internal_resistance)
	{
		switch(_type){
		case batterytype_t::lipo:
			this->cell_voltage_max = this->cell_count * 4.2;
			this->cell_voltage_min = this->cell_count * 3.0;
			break;
		case batterytype_t::lipo_hv:
			this->cell_voltage_max = this->cell_count * 4.35;
			this->cell_voltage_min = this->cell_count * 3.0;
			break;
		case batterytype_t::liion:
			this->cell_voltage_max = this->cell_count * 4.2;
			this->cell_voltage_min = this->cell_count * 3.0;
			break;
		case batterytype_t::liion_hv:
			this->cell_voltage_max = this->cell_count * 4.35;
			this->cell_voltage_min = this->cell_count * 3.0;
			break;
		case batterytype_t::nimh:
			this->cell_voltage_max = this->cell_count * 1.5;
			this->cell_voltage_min = this->cell_count * 1.0;
			break;
		default:
			this->valid = false;
			break;
		}
	}
	~Battery(){}

	bool& 					isValid(void);
	bool					isCharging(void);
	const uint8_t& 			getId(void);
	const batterytype_t&	getType(void);
	float& 					getVoltage(void);
	float&					getCurrent(void);
	float 					getRemainingCapacity(void);
	float&					getPercentage(void);
	const uint8_t&			getCellCount(void);
	uint16_t&				getRemainingTimeMinutes(void);
	batterystate_t&			getState(void);
	batteryerror_t&			getErrors(void);
	uint8_t 				getStatus(void);
	void					configureAdcCharacteristics(const float &_adc_voltage_divider_error_factor, const float &_adc_reference_voltage, const uint32_t &_adc_levels);
	bool					update(const float & _dt);
	bool					update(const float & _dt, uint16_t *_raw_adc_values, const uint8_t &_adc_values_count);
};



#endif /* POWER_H_ */
