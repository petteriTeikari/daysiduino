#include <Wire.h> 
#include "RTClib.h" // https://github.com/adafruit/RTClib
#include <LowPower.h> // https://github.com/rocketscream/Low-Power

// RTC_Millis RTC; // Software RTC
RTC_DS1307 RTC; // With external DS1307

  // For a tutorial see:
  // http://bildr.org/2011/03/ds1307-arduino/

void setup(void){  
  
  Serial.begin(115200);
  Wire.begin(); 
  RTC.begin();
 
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  
}

void loop(){
  
    // Sleep for x s with ADC module and BOD module off
    // other options, see LowPower.cpp, e.g. 15 ms to FOREVER
    // LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); // ADC_CONTROL_OFF -> ADC_OFF
            
    // Get the time using the "software RTC"
    DateTime now = RTC.now();  
    
    // Print on the Serial Monitor / Console the time (debugging)
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
      // If garbage like this is coming out
      // 2165/165/165 165:165:85
      // since midnight 1/1/1970 = 1398728785s = 16188d
      // now + 7d + 30s: 2014/5/5 23:46:55
      // Check out, e.g. :
      // http://www.instructables.com/answers/Why-doesnt-DS1307-work-on-my-arduino/
      // http://forums.adafruit.com/viewtopic.php?f=24&t=28256
      // http://adafruit.com/forums/viewtopic.php?f=41&t=33277

    
    // Integer that could be saved to the EEPROM
    // Could be then converted to human-readable format when reading
    // in the data in Python, Matlab, Java, whatever
    Serial.print(" seconds since 1970: ");
    Serial.println(now.unixtime());
    
    delay(100);
}

