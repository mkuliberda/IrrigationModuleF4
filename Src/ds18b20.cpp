/*
 * ds18b20.cpp
 *
 *  Created on: 21.01.2020
 *      Author: Mati
 */

#include <ds18b20.h>

/******************************************/
/*! DS18B20 class implementation */
/******************************************/

/* delay in microseconds */
void DS18B20::delay_us (const uint32_t & _us){
	this->ptimer->Instance->CNT = 0;
	while (this->ptimer->Instance->CNT <= _us);
}

bool& DS18B20::init(const struct gpio_s & _gpio, TIM_HandleTypeDef* _tim_baseHandle){

	this->valid = false;
	this->ptimer = _tim_baseHandle;
	this->gpio.port = _gpio.port;
	this->gpio.pin = _gpio.pin;
	uint8_t retry_count = 0;

	if(HAL_TIM_Base_Start(ptimer) != HAL_OK) this->valid = false;

	while(this->valid == false && retry_count < 20 ){
		if (this->OneWire_Reset()){
			this->OneWire_WriteByte(ONEWIRE_CMD_READROM);
			for(uint8_t i=0; i<8; i++) ROM[i] = this->OneWire_ReadByte();
			this->valid = ROM[0] == DS18B20_FAMILY_CODE ? true : false;
		}
		this->delay_us(5);
		retry_count++;
	}

	return this->valid;
}

void DS18B20::conversiontimeIncrease(const double & _dt){
	this->conversiontimeSeconds += _dt;
}

double& DS18B20::conversiontimeGet(void){
	return this->conversiontimeSeconds;
}

void DS18B20::conversiontimeReset(void){
	this->conversiontimeSeconds = 0;
}

void DS18B20::conversionStart(void){

	this->conversiontimeReset();
	this->OneWire_WriteByte(DS18B20_CMD_CONVERTTEMP);
	this->conversionflagSet();
}

void DS18B20::conversionflagSet(void){
	this->conversionRunning = true;
}

bool& DS18B20::conversionflagGet(void){
	return this->conversionRunning;
}

void DS18B20::conversionflagReset(void){
	this->conversionRunning = false;
}

void DS18B20::gpioSetInput (void){

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = this->gpio.pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(this->gpio.port, &GPIO_InitStruct);
}


void DS18B20::gpioSetOutput (void){

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = this->gpio.pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(this->gpio.port, &GPIO_InitStruct);
}

inline uint8_t DS18B20::OneWire_Reset(void)
{
	/* Line low, and wait 480us */
	HAL_GPIO_WritePin (this->gpio.port, this->gpio.pin, GPIO_PIN_RESET);
	this->gpioSetOutput();
	this->delay_us(500); //480

	/* Release line and wait for 70us */
	this->gpioSetInput();
	this->delay_us(80); //70
	/* Check bit value */
	if (!(HAL_GPIO_ReadPin (this->gpio.port, this->gpio.pin)))    // if the pin is low i.e the presence pulse is there
	{
		this->delay_us (410);  // wait for 400 us
		return true;
	}

	else
	{
		this->delay_us (410);
		return false;
	}
}

inline void DS18B20::OneWire_WriteBit(const uint8_t & _bit)
{
	if (_bit)
	{
		/* Set line low */
		HAL_GPIO_WritePin (this->gpio.port, this->gpio.pin, GPIO_PIN_RESET);
		this->gpioSetOutput();
		this->delay_us(10);

		/* Bit high */
		this->gpioSetInput();

		/* Wait for 55 us and release the line */
		this->delay_us(55);
		this->gpioSetInput();
	}
	else
	{
		/* Set line low */
		HAL_GPIO_WritePin (this->gpio.port, this->gpio.pin, GPIO_PIN_RESET);
		this->gpioSetOutput();
		this->delay_us(65);

		/* Bit high */
		this->gpioSetInput();

		/* Wait for 5 us and release the line */
		this->delay_us(5);
		this->gpioSetInput();
	}

}

inline uint8_t DS18B20::OneWire_ReadBit(void)
{
	uint8_t bit = 0;

	/* Line low */
	HAL_GPIO_WritePin (this->gpio.port, this->gpio.pin, GPIO_PIN_RESET);  // pull the pin LOW
	this->gpioSetOutput();
	this->delay_us(2);

	/* Release line */
	this->gpioSetInput();
	this->delay_us(10);

	/* Read line value */
	if (HAL_GPIO_ReadPin(this->gpio.port, this->gpio.pin)) {
		/* Bit is HIGH */
		bit = 1;
	}

	/* Wait 50us to complete 60us period */
	this->delay_us(50);

	/* Return bit value */
	return bit;
}

void DS18B20::OneWire_WriteByte(uint8_t byte) {
	uint8_t i = 8;
	/* Write 8 bits */
	while (i--) {
		/* LSB bit is first */
		this->OneWire_WriteBit(byte & 0x01);
		byte >>= 1;
	}
}

uint8_t DS18B20::OneWire_ReadByte(void) {
	uint8_t i = 8, byte = 0;
	while (i--) {
		byte >>= 1;
		byte |= (this->OneWire_ReadBit() << 7);
	}

	return byte;
}

uint8_t DS18B20::OneWire_CRC8(uint8_t *addr, uint8_t len) {
	uint8_t crc = 0, inbyte, i, mix;

	while (len--) {
		inbyte = *addr++;
		for (i = 8; i; i--) {
			mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix) {
				crc ^= 0x8C;
			}
			inbyte >>= 1;
		}
	}

	/* Return calculated CRC */
	return crc;
}

bool& DS18B20::isValid(void){
	return this->valid;
}

float& DS18B20::temperatureCelsiusRead(const double & _dt){

	uint16_t temperature;
	uint8_t resolution;
	int8_t digit, minus = 0;
	float decimal;
	uint8_t i = 0;
	uint8_t data[9];
	uint8_t crc;


	if (this->conversionflagGet() == false){
		this->OneWire_Reset();
		this->OneWire_WriteByte(ONEWIRE_CMD_SKIPROM);
		this->conversionStart();
	}
	else{
		this->conversiontimeIncrease(_dt);
	}

	if(this->conversiontimeGet() > 0.75 && this->OneWire_ReadBit()){

		this->conversionflagReset();
		this->OneWire_Reset();
		this->OneWire_WriteByte(ONEWIRE_CMD_SKIPROM);
		this->OneWire_WriteByte(ONEWIRE_CMD_RSCRATCHPAD);

		/* Get data */
		for (i = 0; i < 9; i++)
		{
			/* Read byte by byte */
			data[i] = this->OneWire_ReadByte();
		}

		/* Calculate CRC */
		crc = this->OneWire_CRC8(data, 8);

		/* Check if CRC is ok */
		if (crc == data[8]){
			/* First two bytes of scratchpad are temperature values */
			temperature = data[0] | (data[1] << 8);

			/* Reset line */
			//this->OneWire_Reset();

			/* Check if temperature is negative */
			if (temperature & 0x8000)
			{
				/* Two's complement, temperature is negative */
				temperature = ~temperature + 1;
				minus = 1;
			}


			/* Get sensor resolution */
			resolution = ((data[4] & 0x60) >> 5) + 9;


			/* Store temperature integer digits and decimal digits */
			digit = temperature >> 4;
			digit |= ((temperature >> 8) & 0x7) << 4;

			/* Store decimal digits */
			switch (resolution)
			{
				case 9:
					decimal = (temperature >> 3) & 0x01;
					decimal *= (float)DS18B20_DECIMAL_STEPS_9BIT;
				break;
				case 10:
					decimal = (temperature >> 2) & 0x03;
					decimal *= (float)DS18B20_DECIMAL_STEPS_10BIT;
				 break;
				case 11:
					decimal = (temperature >> 1) & 0x07;
					decimal *= (float)DS18B20_DECIMAL_STEPS_11BIT;
				break;
				case 12:
					decimal = temperature & 0x0F;
					decimal *= (float)DS18B20_DECIMAL_STEPS_12BIT;
				 break;
				default:
					decimal = 0xFF;
					digit = 0;
			}

			/* Check for negative part */
			decimal = digit + decimal;
			if (minus)
				decimal = 0 - decimal;

			this->temperatureCelsius = decimal;
		}

	}

	return this->temperatureCelsius;
}
