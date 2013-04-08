
/*
 ADXL362_LoPower.ino -  Simple XYZ axis reading example
 for Analog Devices ADXL362 - Micropower 3-axis accelerometer
 go to http://www.analog.com/ADXL362 for datasheet 
 
 License: CC BY-SA 3.0: Creative Commons Share-alike 3.0. Feel free 
 to use and abuse this code however you'd like. If you find it useful
 please attribute, and SHARE-ALIKE!
 
 Created June 2012
 by Anne Mahaffey - hosted on http://annem.github.com/ADXL362

 Modified April 2013
 by Petteri Teikari, to work with the low power library
 for Mini Ultra+ 8 MHz
 
Connect SCLK, MISO, MOSI, and CSB of ADXL362 to
SCLK, MISO, MOSI, and DP 10 of Arduino 
(check http://arduino.cc/en/Reference/SPI for details)
(also can check the wiring from: http://ez.analog.com/docs/DOC-2222)

Arduino  ADXL362
+++++++++++++++++++
VCC	 VS and VIO
GND	 GND
10       CS
11	 MOSI
12	 MISO
13	 SCLK
 
*/ 

#include <SPI.h>
#include <ADXL362.h> // ADXL362 library, http://annem.github.io/ADXL362/
#include <LowPower.h> // http://www.rocketscream.com/blog/2011/07/04/lightweight-low-power-arduino-library/

// Declare the variables
ADXL362 xl;

int temp;
int XValue, YValue, ZValue, Temperature;

void setup(){
  
    Serial.begin(9600);
    xl.begin();                   // Setup SPI protocol, issue device soft reset
    xl.beginMeasure();            // Switch ADXL362 to measure mode  
    xl.checkAllControlRegs();     // Burst Read all Control Registers, to check for proper setup
	
    Serial.print("\n\nBegin Loop Function:\n");
}

void loop(){
    
    // The Low Power part added based on the short tutorial
    // by Rocket Scream
    // http://www.rocketscream.com/blog/2011/07/04/lightweight-low-power-arduino-library/
  
    // Sleep for x s with ADC module and BOD module off
    // other options, see LowPower.cpp, e.g. 15 ms to FOREVER
    LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF); // ADC_CONTROL_OFF -> ADC_OFF

    // read all three axis in burst to ensure all measurements correspond to same sample time
    xl.readXYZTData(XValue, YValue, ZValue, Temperature);  	 
    delay(100); // Arbitrary delay to make serial monitor easier to observe
    
}

