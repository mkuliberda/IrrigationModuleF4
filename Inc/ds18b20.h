/*
 * ds18b20.h
 *
 *  Created on: 21.01.2020
 *      Author: Mati
 */

#ifndef DS18B20_H_
#define DS18B20_H_

#include "stm32f4xx_hal.h"
#include "main.h"
#include "gpio.h"
#include "tim.h"
#include "sensors_base.h"
#include "cmsis_os.h"


/* OneWire commands */
#define ONEWIRE_CMD_RSCRATCHPAD			0xBE
#define ONEWIRE_CMD_WSCRATCHPAD			0x4E
#define ONEWIRE_CMD_CPYSCRATCHPAD		0x48
#define ONEWIRE_CMD_RECEEPROM			0xB8
#define ONEWIRE_CMD_RPWRSUPPLY			0xB4
#define ONEWIRE_CMD_SEARCHROM			0xF0
#define ONEWIRE_CMD_READROM				0x33
#define ONEWIRE_CMD_MATCHROM			0x55
#define ONEWIRE_CMD_SKIPROM				0xCC

#define DS18B20_FAMILY_CODE				0x28
#define DS18B20_CMD_ALARMSEARCH			0xEC

/* DS18B20 read temperature command */
#define DS18B20_CMD_CONVERTTEMP			0x44 	/* Convert temperature */
#define DS18B20_DECIMAL_STEPS_12BIT		0.0625
#define DS18B20_DECIMAL_STEPS_11BIT		0.125
#define DS18B20_DECIMAL_STEPS_10BIT		0.25
#define DS18B20_DECIMAL_STEPS_9BIT		0.5

/* Bits locations for resolution */
#define DS18B20_RESOLUTION_R1					6
#define DS18B20_RESOLUTION_R0					5

/* CRC enabled */
#ifdef DS18B20_USE_CRC
#define DS18B20_DATA_LEN							9
#else
#define DS18B20_DATA_LEN							2
#endif

/***********************************/
/*! DS18B20 class implementation */
/***********************************/

class DS18B20: public TemperatureSensor{

private:

	bool 							valid;
	struct gpio_s 					gpio;
	TIM_HandleTypeDef 				*ptimer;
	double							conversiontimeSeconds;
	bool							conversionRunning;
	float							temperatureCelsius;
	uint8_t							ROM[8];

	void 							delay_us (const uint32_t & _us);
	void							conversiontimeIncrease(const double & _dt);
	double&							conversiontimeGet(void);
	void							conversiontimeReset(void);
	void							conversionStart(void);
	void							conversionflagSet(void);
	bool&							conversionflagGet(void);
	void							conversionflagReset(void);
	void 							gpioSetInput (void);
	void 							gpioSetOutput (void);
	inline uint8_t 					OneWire_ReadBit(void);
	inline void 					OneWire_WriteBit(const uint8_t & _bit);
	uint8_t							OneWire_ReadByte(void);
	void 							OneWire_WriteByte(uint8_t byte);
	inline uint8_t 					OneWire_Reset(void);
	uint8_t 						OneWire_CRC8(uint8_t *addr, uint8_t len);

public:

	DS18B20():
		valid(false),
		conversiontimeSeconds(0),
		conversionRunning(false),
		temperatureCelsius(-1000)
	{
		this->interfacetype = sensorinterfacetype_t::digital_1Wire;
		this->type = temperaturesensortype_t::ds18b20;
	};

	bool& 							init(const struct gpio_s & _gpio, TIM_HandleTypeDef* _tim_baseHandle);
	bool& 							isValid(void);
	float& 							readTemperatureCelsius(const double & _dt);

};



#endif /* DS18B20_H_ */
