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


class TemperatureSensor{

protected:

	temperaturesensortype_t 		type;
	sensorinterfacetype_t			interfacetype;

	temperaturesensortype_t& 		typeGet(void);
	sensorinterfacetype_t& 			interfacetypeGet(void);

public:

	TemperatureSensor(){};

	virtual ~TemperatureSensor(){};

};



class MoistureSensor{

protected:

	moisturesensortype_t 			type;
	sensorinterfacetype_t			interfacetype;
	float 							moisturePercent;
	bool							valid;

	moisturesensortype_t& 			typeGet(void);
	sensorinterfacetype_t& 			interfacetypeGet(void);
	virtual void 					percentUpdate(void) = 0;

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
	float&							percentGet(void);

};


class WaterLevelSensor{

protected:

	waterlevelsensortype_t 				type;
	waterlevelsensorsubtype_t 			subtype;
	sensorinterfacetype_t				interfacetype;

	virtual waterlevelsensorsubtype_t& 	subtypeGet(void);
	virtual waterlevelsensortype_t&		typeGet(void);
	virtual sensorinterfacetype_t& 		interfacetypeGet(void);

public:

	WaterLevelSensor(){};

	virtual ~WaterLevelSensor(){};

};


#endif /* SENSORS_BASE_H_ */
