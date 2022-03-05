#include "Arduino.h"

#ifndef MAX6675_h
#define MAX6675_h

class MAX6675 {
	public:
	
	/* Functions */
	void init(byte, byte, byte); //Initialises the sensor. req. SCK CS SO	
	
	uint16_t getData(); //Gets raw 16bit data from the MAX6675
	
	float getTemp(); //Uses 16Bit data and returns temp in C, including decimal

	uint16_t getTempFast(); //Uses 16Bit data and returns temp in C as int 
	
	float getTempF(); //Gets temperature in Celcius then converts it to Fahrenheit
		
	private:
	byte _SCK, _CS, _SO; //Pinout for the sensor
	
};
#endif
