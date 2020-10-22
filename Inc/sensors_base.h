#ifndef SENSORS_BASE_H_
#define SENSORS_BASE_H_


enum class temperaturesensortype_t{
	generic,
	ds18b20
};

enum class fixedwaterlevelsensorstate_t{
	undetermined,
	wet,
	dry
};

enum class waterlevelsensorsubtype_t{
	unknown,
	fixed,
	floating
};

enum class waterlevelsensortype_t{
	unknown,
	optical,
	capacitive,
	resistive
};

enum class sensorinterfacetype_t {
	gpio,
	analog,
	digital_I2C,
	digital_SPI,
	digital_1Wire,
	digital_UART,
	digital_CAN
};

enum class moisturesensortype_t{
	generic,
	capacitive_noshield
};

class Sensor{
protected:

	sensorinterfacetype_t			interfacetype;

	sensorinterfacetype_t& 			getInterface(void);

public:
	Sensor(){};

	virtual ~Sensor(){};

};

class TemperatureSensor: protected Sensor{

protected:

	temperaturesensortype_t 		type;
	temperaturesensortype_t& 		getType(void);

public:

	TemperatureSensor(){};

	virtual ~TemperatureSensor(){};

};



class MoistureSensor: protected Sensor{

protected:

	moisturesensortype_t 			type;
	float 							moisturePercent;
	bool							valid;

	moisturesensortype_t& 			getType(void);
	virtual void 					updatePercent(void) = 0;

public:

	MoistureSensor():
	moisturePercent(0),
	valid(false)
	{
		this->type = moisturesensortype_t::generic;
	};

	virtual ~MoistureSensor(){};

	virtual float		 			read(void) = 0;
	virtual bool&					isValid(void) = 0;
	float&							getMoisturePercent(void);

};


class WaterLevelSensor: protected Sensor{

protected:

	waterlevelsensortype_t 				type;
	waterlevelsensorsubtype_t 			subtype;

	virtual waterlevelsensorsubtype_t& 	getSubtype(void);
	virtual waterlevelsensortype_t&		getType(void);

public:

	WaterLevelSensor(){};

	virtual ~WaterLevelSensor(){};

};


#endif /* SENSORS_BASE_H_ */
