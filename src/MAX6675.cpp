/*
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
#include "MAX6675.h"

void MAX6675::init(byte SCK, byte CS, byte SO) { //Req. SCK CS SO
	//Set global pinouts
	_SCK = SCK;
	_CS = CS;
	_SO = SO;
	
	pinMode(_SCK, OUTPUT);
	pinMode(_CS, OUTPUT);
	pinMode(_SO, INPUT);
	
	digitalWrite(_SCK, LOW);
	//Put chip into sample & wait mode
	digitalWrite(_CS, HIGH);
}

uint16_t MAX6675::getData() {
	uint16_t datastream; //raw 16 bit data
	
	/* Read data from MAX6675 */
	digitalWrite(_CS, LOW); //Start communication
	
	for(uint8_t cbit = 0; cbit < 16; cbit++) {
		digitalWrite(_SCK, LOW); //Falling edge of clock		
		datastream = digitalRead(_SO) + (datastream << 1);
		digitalWrite(_SCK, HIGH); //Rising edgo of clock
		delay(1); 
	}
	
	//Reset SCK pin for next read cycle
	digitalWrite(_SCK, LOW);
	
	digitalWrite(_CS, HIGH); //Stop communication
	
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
