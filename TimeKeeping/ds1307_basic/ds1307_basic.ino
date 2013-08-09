// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

void setup () {
  Serial.begin(57600);
#ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif
  rtc.begin();
  

  
  // Now adjusts the time every time this Sketch is uploaded 
  // to the Daysimeter, the time is the time when this sketch
  // is compiled, for details see:
  // http://learn.adafruit.com/ds1307-real-time-clock-breakout-board-kit/understanding-the-code
  rtc.adjust(DateTime(__DATE__, __TIME__));
  
  delay(2000) // for opening Serial Monitor
  Serial.print(DateTime.Hour,DEC);  
  Serial.print(':');
  Serial.print(printDigits(DateTime.Minute,DEC);  
  Serial.print(':');
  Serial.println(printDigits(DateTime.Second,DEC);  
  
  if (! rtc.isrunning()) {
    
    // Petteri: What should this return, or how to test if the clock
    // is set optimally, as "from the box" this if is not entered
    // and the time displayed is the default time from 2000    
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
}

void loop () {
    DateTime now = rtc.now();
    
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
    
    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");
    
    // calculate a date which is 7 days and 30 seconds into the future
    DateTime future (now.unixtime() + 7 * 86400L + 30);
    
    Serial.print(" now + 7d + 30s: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();
    
    Serial.println();
    delay(3000); // Display the value only every 3 seconds
}
