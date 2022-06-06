#include "Arduino.h"
#include "MAX6675.h"

//Initilaise the MAX6675. SCK CS SO
MAX6675::MAX6675(uint8_t v_SCK, uint8_t v_CS, uint8_t v_SO) {
	//Set global pinouts
	_SCK = v_SCK;
	_CS = v_CS;
	_SO = v_SO;
	
	//Convert arduino pin number to asm
	hw_SCKasm = digitalPinToBitMask(_SCK);
	hw_CSasm = digitalPinToBitMask(_CS);
	hw_SOasm = digitalPinToBitMask(_SO);
	
	//Allocate ports for each pin
	hw_SCKport = portOutputRegister(digitalPinToPort(_SCK));
	hw_CSport = portOutputRegister(digitalPinToPort(_CS));
	hw_SOport = portOutputRegister(digitalPinToPort(_SO));
	
	pinMode(_SCK, OUTPUT);
	pinMode(_CS, OUTPUT);
	pinMode(_SO, INPUT);
	
	//Put chip into sample & wait mode
	digitalWrite(_SCK, LOW);
	digitalWrite(_CS, HIGH);
}

/*
	digitalRead, shift original value in one operation: 220 us
	shift data each step, then or in SOBit: 140us
	shift SOBit but cbit then or with data: 180us
	
*/
uint16_t MAX6675::getData() {
	//Converted from arduino to port maniuplation
	uint16_t datastream = 0; //raw 16 bit data
	bool SOBit; //Bit detected from input line.
	
	/* Read data from MAX6675 */
	*hw_CSport &= ~hw_CSasm; //CS LOW - Start communication
	
	for(uint8_t cbit = 0; cbit < 16; cbit++) {
		*hw_SCKport &= ~hw_SCKasm; //Falling edge of clock
		
		//Shift datastream by 1 to get next bit
		datastream = datastream << 1;
		//Get state of SO pin
		SOBit = digitalRead(_SO);
		//OR in the read bit.
		datastream |= SOBit;
		
		*hw_SCKport |= hw_SCKasm; //Rising edge of clock
		
		//Delay for a short time if flag is set. Mostly not needed.
		if(SAFETY_DELAY == true) {
			delayMicroseconds(500); 
		}
	}
	
	*hw_SCKport &= ~hw_SCKasm; //SCK LOW - Reset SCK pin for next read cycle
	
	*hw_CSport |= hw_CSasm; //CS HIGH - Stop communication
	
	/* Error checking */
	//If bit 15 (dummy 0 bit) is 1, error
	if(datastream >> 15) {
		return 0;
	}
	
	//If the thermocouple sense bit is 1 TODO
	if(datastream & 0x04) {
		return 0;
	}
	
	return datastream;
}

float MAX6675::getTemp() {
	uint16_t rawData = getData();
	
	if(rawData != 0) {
		/* Construct temp from data */
		//Decimal is 0.25 * the first usable 2 bits from data - shifted by 3
		float decimal = 0.25 * ((rawData >> 3) & 0x03);
		//temp is bits 14 to 5, 10bits - datastream shifted by 5 - plus decimal
		float temperature = (rawData >> 5) + decimal;
		
		return temperature;
	}
	
	//error condition
	return -9999.99;
}

uint16_t MAX6675::getTempFast() {
	uint16_t rawData = getData();
	
	if(rawData != 0) {
		/* Construct temp from data */
		//temp is bits 14 to 5, 10bits - datastream shifted by 5
		uint16_t temperature = (rawData >> 5);
		
		return temperature;
	}
	
	//error condition
	return 9999;
}

float MAX6675::getTempF() {
	float temperature = getTemp(); //Get temp in C
	
	//If getTemp had errored, error again
	if(temperature == -9999.99) {
		return temperature;
	}
	
	//The most common method of of C to F is divide by 5, multiply by 9, add 32
	//To speed this up into one calculation, multiply by 1.8, add 32
	temperature = (temperature * 1.8) + 32;
	
	return temperature;
}
