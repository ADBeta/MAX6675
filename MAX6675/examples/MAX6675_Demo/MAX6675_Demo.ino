/* 
 * Demonstration of the capabilities of this very basic but fast MAX6675 Library
 * 
 * The speed of each function has been measured and will be shown next to each
 * function call. This library was designed to be easily fast enough to work on
 * a ATTiny85/45/25 at even 1MHz.
 *
 * ADBeta 03 Jun 2022
*/

#include <MAX6675.h>

#define thrmSCK 8
#define thrmCS 9
#define thrmSO 10

MAX6675 Thermo(thrmSCK, thrmCS, thrmSO);

void setup() {
	Serial.begin(9600);

}

void loop() {

	//Time to execute: 120 microseconds
	Serial.print("Temp normal: ");
	Serial.println(Thermo.getTemp());
	
	//Time to execute: 100 microseconds
	Serial.print("Temp fast: ");
	Serial.println(Thermo.getTempFast());
	
	//Time to execute: 150 microseconds
	Serial.print("Temp farenheit: ");
	Serial.println(Thermo.getTempF());
	
	Serial.println();

  delay(2000);
}


