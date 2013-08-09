
/*
 ADXL362_LoPower.ino -  Simple XYZ axis reading example
 for Analog Devices ADXL362 - Micropower 3-axis accelerometer
 go to http://www.analog.com/ADXL362 for datasheet 
 
 License: CC BY-SA 3.0: Creative Commons Share-alike 3.0. Feel free 
 to use and abuse this code however you'd like. If you find it useful
 please attribute, and SHARE-ALIKE!
 
 Created June 2012
 by Anne Mahaffey - hosted on http://annem.github.com/ADXL362

 Modified July 2013
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
4        CS
11	 MOSI
12	 MISO
13	 SCLK
 
*/ 

// PETTERI
// Note that if you re-download the ADXL362 library from GitHub the library uses
// by default the pin 10 as the slave select pin whereas for the Daysimeter v. 13
// development board it is the Pin 4 (SS-2) so remember to change this for 
// ADXL362.cpp as well

#include <SPI.h>
#include <ADXL362.h> // ADXL362 library, http://annem.github.io/ADXL362/
#include <LowPower.h> // http://www.rocketscream.com/blog/2011/07/04/lightweight-low-power-arduino-library/

// Declare the variables
ADXL362 xl;

int temp;
int XValue, YValue, ZValue, Temperature;

// Added to get the delta of acceleration (for example to flash
// the indicator LED to show the acceleration)
int XValue_prev, YValue_prev, ZValue_prev, Temperature_prev;
int XValue_delta, YValue_delta, ZValue_delta, Temperature_delta, acc_delta;
int loopCount = 0;
int ledIntensity;
int ledPin = 3;    // LED connected to digital pin 2

void setup(){
  
    Serial.begin(9600);
    xl.begin();                   // Setup SPI protocol, issue device soft reset
    xl.beginMeasure();            // Switch ADXL362 to measure mode  
    xl.checkAllControlRegs();     // Burst Read all Control Registers, to check for proper setup
    
    Serial.print("\n\nBegin Loop Function:\n");
}

void loop(){

    loopCount++;
    
    // The Low Power part added based on the short tutorial
    // by Rocket Scream
    // http://www.rocketscream.com/blog/2011/07/04/lightweight-low-power-arduino-library/
  
    // Sleep for x s with ADC module and BOD module off
    // other options, see LowPower.cpp, e.g. 15 ms to FOREVER
    LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF); // ADC_CONTROL_OFF -> ADC_OFF

    // read all three axis in burst to ensure all measurements correspond to same sample time
    // xl.readXYZTData(XValue, YValue, ZValue, Temperature);  	 
    XValue = xl.readXData();
    YValue = xl.readYData();
    ZValue = xl.readZData();
    Temperature = xl.readTemp();  
  
    if (loopCount == 1) {
      XValue_prev = XValue;
      YValue_prev = YValue;
      ZValue_prev = ZValue;
      Temperature_prev = Temperature;
    }    
    
    // Get the changes in acceleration
    XValue_delta = XValue_prev - XValue;
    YValue_delta = YValue_prev - YValue;
    ZValue_delta = ZValue_prev - ZValue;
    Temperature_delta = Temperature_prev - Temperature;
    
    // Update the previous value
    XValue_prev = XValue;
    YValue_prev = YValue;
    ZValue_prev = ZValue;
    Temperature_prev = Temperature;
    
    // Get "RMS acceleration delta"
    // computationally faster just sum the absolute values
    acc_delta = abs(XValue_delta) + abs(YValue_delta) + abs(ZValue_delta);
            
    // Quick'n'dirty manual hand shaking of the board gives a maximum acc_delta of around 6000
    // so we can remap this to led output intensity
    ledIntensity = map(acc_delta, 1, 6000, 0, 255);
    analogWrite(ledPin, ledIntensity); 
    
    // Display on Serial Monitor
    Serial.print(  "XDATA = "); Serial.print(XValue);
    Serial.print(  "\tYDATA = "); Serial.print(YValue); 
    Serial.print(  "\tZDATA = "); Serial.print(ZValue); 
    Serial.print(  "\tTemperature = "); Serial.print(Temperature);
    Serial.print(  "\tLED = "); Serial.print(ledIntensity);
    Serial.print(  "\tAccDeltaSum = "); Serial.println(acc_delta);

    
    // Arbitrary delay to make serial monitor easier to observe
    delay(150); 
}

