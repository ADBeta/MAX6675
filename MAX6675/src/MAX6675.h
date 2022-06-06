/*
 *	Simple MAX6675 Thermocouple IC library. Contains Fahrenheit conversion as 
 *	well as a fast function that doesn't work with floats. ideal for low power
 *	ICs like ATTinys'. 
 *
 *	Version 3.5.0
 *	ADBeta 06 Jun 2022
 
	Notes:
		CS > LOW to start read
		Bit 15 is already present at SO but is a dummy, always 0
		16 clock pulses are required to get a full transmission 
		SCK > Read SO at falling edge
		D14 - D3 converted data
		D2 Open state of thermocouple - 1 = open 0 = closed
		D1 is LOW
		D0 is tri-state 
*/
#include "Arduino.h"

#ifndef MAX6675_h
#define MAX6675_h

class MAX6675 {
	public:
	
	bool SAFETY_DELAY = false;
	
	/* Functions */
	//Initilaise the MAX6675. SCK CS SO
	MAX6675(uint8_t, uint8_t, uint8_t);	
	
	uint16_t getData(); //Gets raw 16bit data from the MAX6675
	
	float getTemp(); //Uses data and returns temp in C, including decimal

	uint16_t getTempFast(); //Uses data and returns temp in C as int 
	
	float getTempF(); //Gets temperature in Celcius, converts to Fahrenheit
		
	private:
	uint8_t _SCK, _CS, _SO; //Arduino pinout
	//Convert arduino pin number to asm
	uint8_t hw_SCKasm, hw_CSasm, hw_SOasm;
	//Allocate ports for each pin
	volatile uint8_t *hw_SCKport, *hw_CSport, *hw_SOport;
};
#endif
