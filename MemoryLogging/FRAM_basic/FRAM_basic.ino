// Demo for using SPI FRAM Memory with Ardino
// by Kerry D. Wong
// http://www.kerrywong.com/2012/01/15/using-fram-as-nonvolatile-memory-with-arduino/

// In this blog post I will give an example on interfacing two Ramtron FM25C160 FRAM chips 
// with Arduino. Since the basic operations on an FRAM are very similar to those on a 
// standard EEPROM, I would recommend reading this SPI EEPROM tutorial first 
// if you are not familiar with the protocol.

// The main difference between the operation modes of an FM25C160 and a standard EEPROM 
// is that FM25C160 allows multi-byte sequential reads and writes whereas EEPROM typically 
// has a limited write buffer and long writes beyond the buffer length must be broken 
// into multiple writes. We will take advantage of this in our code later.
#include <SPI.h>

  // See a good intro of SPI by Nick Gammon
  // http://www.gammon.com.au/forum/?id=10892
 
// For further information, see the SPI Guide for F-RAM
// http://www.cypress.com/?docID=44520
const byte CMD_WREN = 0x06; //0000 0110 Set Write Enable Latch
const byte CMD_WRITE = 0x02; //0000 0010 Write Memory Data
const byte CMD_READ = 0x03; //0000 0011 Read Memory Data
const byte CMD_RDSR = 0x05; //0000 0101 Read Status Register
const byte CMD_WRSR = 0x01; //0000 0001 Write Status Register
const byte CMD_WRDI = 0x04; //0000 0100 Write Disable
const byte CMD_SLEEP = 0xB9; // 1011 1001 Enter Sleep Mode
const byte CMD_FASTREAD = 0x0A; // 0000 1011 Fast Read Memory Data
const byte CMD_RDID = 0x9F ; // 1001 1111 Read Device ID
const byte CMD_SNR= 0xC3 ; // 1100 0011 Read Device S/N
 
const int FRAM_CS1 = 2; //chip select for memory (2 in Daysimeter v.13)

// ------------------------------------------------------
// WRITE
// ------------------------------------------------------
 
  /**
   * Write to FRAM (original code with FM25C160)
                    - FM25C160 is a 16bit memory with 2 address bytes
                    - and 11 address bits, whereas the corresponding values
                    - for FM25V20 (2 MBit) are 3 and 18
                    - for FM25V02 (256 kbit) are 2 and 15
   * addr: starting address
   * buf: pointer to data
   * count: data length. 
   *        If this parameter is omitted, it is defaulted to one byte.
   * returns: 0 operation is successful
   *          -1 address out of range
   */
  int FRAMWrite(int addr, byte *buf, int count=1)
  {
    int cs = 0;
    cs = FRAM_CS1;
     
    if (addr > 0x7ff) // correct this for 256 kb and 2048 kB chips
      {
      return -1;
      }
     
    byte addrMSB = (addr >> 8) & 0xff;
    byte addrLSB = addr & 0xff;
     
    digitalWrite(cs, LOW);   
    SPI.transfer(CMD_WREN); //write enable 
    digitalWrite(cs, HIGH); //after WREN the /CS goes high  
   
    // For EEPROMs, a delay here is suggested while the FRAM should be
    // considerably faster, e.g. http://www.microchip.com/forums/m709262-print.aspx
    // delay(35);
    
    digitalWrite(cs, LOW); // and back LOW before op-code
                           // see Fig. 6 of SPI Guide for F-RAM
                           // http://www.cypress.com/?docID=44520
    //delay(35);
       
    // Serial.print("Write1 RDSR: "); Serial.println(String(SPI.transfer(CMD_RDSR),BIN));                           
    SPI.transfer(CMD_WRITE); //write command
    // Serial.print("Write2 RDSR: "); Serial.println(String(SPI.transfer(CMD_RDSR),BIN));
    SPI.transfer(addrMSB);
    // SPI.transfer(secondByte); // for the 2 MBit chip
    SPI.transfer(addrLSB);
     
    for (int i = 0;i < count;i++) 
      {
      SPI.transfer(buf[i]);
      // Serial.println(buf[i]); // At this point, the value is correct
      }
   
    digitalWrite(cs, HIGH);   
    delay(10);
    return 0;
  }
 
 
// ------------------------------------------------------
// READ
// ------------------------------------------------------
 
  /**
   * Read from FRAM (original code with FM25C160)
                    - FM25C160 is a 16bit memory with 2 address bytes
                    - and 11 address bits, whereas the corresponding values
                    - for FM25V20 (2 MBit) are 3 and 18
                    - for FM25V02 (256 kbit) are 2 and 15
   * addr: starting address
   * buf: pointer to data
   * count: data length. 
   *        If this parameter is omitted, it is defaulted to one byte.
   * returns: 0 operation is successful
   *          1 address out of range
   */
  int FRAMRead(int addr, byte *buf, int count=1)
  {
    int cs = 0;
    cs = FRAM_CS1;  
     
    if (addr > 0x7ff) 
      {
      return -1;
      }
   
    byte addrMSB = (addr >> 8) & 0xff;
    byte addrLSB = addr & 0xff;
     
    digitalWrite(cs, LOW);
    // delay(35);
    
    // Serial.print("Read RDSR: "); Serial.println(String(SPI.transfer(CMD_RDSR),BIN));
     
    SPI.transfer(CMD_READ);
    SPI.transfer(addrMSB);
   // SPI.transfer(secondByte); // for the 2 MBit chip
    SPI.transfer(addrLSB);  
     
    for (int i=0; i < count; i++) 
      {
      buf[i] = SPI.transfer(0xFF); // What is the actual purpose of the 0x00?
                                   // "Don't Care" in Fig. 5 of f SPI Guide for F-RAM
                                   // http://www.cypress.com/?docID=44520
                                   // And apparently one can get different results
                                   // with 0x00 compared to 0xFF
                                   // http://forum.arduino.cc/index.php?topic=162242.15
      // Serial.println(buf[i]); 
      // delay(0.1);
      }
   
    digitalWrite(cs, HIGH);    
    return 0;
  }


// ------------------------------------------------------
// SETUP
// ------------------------------------------------------
 
  void setup()
  {  
  
    pinMode(FRAM_CS1, OUTPUT);
    digitalWrite(FRAM_CS1, HIGH);
    delay(10);

   
    // Setting up the SPI bus
    SPI.begin();
    SPI.setDataMode(SPI_MODE0); // FRAMs support 0 and 3
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV64); // 8 MHz of system clock
                                          // SPI Clock was 1 MHz for
                                          // the Bluetooth in the
                                          // Red Bear demo
                                          
    Serial.begin(115200);  
    delay(100);

    // Try to get the Product ID and S/N of the chip    
    digitalWrite(FRAM_CS1, LOW);  
    Serial.print("Product ID: "); Serial.println(String(SPI.transfer(CMD_RDID),BIN));
    digitalWrite(FRAM_CS1, HIGH);
    delay(10);
    
    /*
    digitalWrite(FRAM_CS1, LOW);  
    Serial.print("S/N: "); Serial.println(String(SPI.transfer(CMD_SNR),BIN));
    digitalWrite(FRAM_CS1, HIGH);
    
    
    // DEBUG
    digitalWrite(FRAM_CS1, LOW);
    Serial.print("Setup 1 RDSR: "); Serial.println(String(SPI.transfer(CMD_RDSR),BIN));
    digitalWrite(FRAM_CS1, HIGH);
    
    digitalWrite(FRAM_CS1, LOW);
    SPI.transfer(CMD_WRDI);
    digitalWrite(FRAM_CS1, HIGH);
    
    digitalWrite(FRAM_CS1, LOW);
    Serial.print("Setup 2 RDSR: "); Serial.println(String(SPI.transfer(CMD_RDSR),BIN));
    digitalWrite(FRAM_CS1, HIGH);  
    */
    
    
  }
 
 
// ------------------------------------------------------
// LOOP
// ------------------------------------------------------ 
 
  void loop()
  {
    
    //Test
    char buf[] ="Test";
    char buf2[] ="PROB";   
    int bufByte;
    int bufByte2;
       
    // Serial.println(buf);
    // Serial.println(buf2);
     
    // WRITE 
    int writeFlag = FRAMWrite(1, (byte*) buf, 4);
      // delay(20); // delay here suggested, http://www.microchip.com/forums/m709262-print.aspx
    
    // Check the STATUS REGISTER at this point
    //digitalWrite(FRAM_CS1, LOW);
    // Serial.print("After Write RDSR: "); Serial.println(String(SPI.transfer(CMD_RDSR),BIN));
      // 7: WPEN, 3: BP1, 2: BP0, 1: WEL
    //digitalWrite(FRAM_CS1, HIGH);
    
    // READ
    int readFlag = FRAMRead(1, (byte*) buf, 4);   
  
    /*
    Serial.println("CHAR looped");   
    for (int i = 0; i < 4; i++) { 
      Serial.print(i); 
      Serial.print("\t"); 
      Serial.print(writeFlag);    
      Serial.print("\t");
      Serial.print(readFlag);    
      Serial.print("\t");
      Serial.print(buf[i]);
      Serial.print("\t");
      Serial.println(buf2[i]); // Problem now that only zeroes are returned
                                // empty (space?) as character  
  
    }
    */
    
    
    
    
    /**
    Serial.println("INT in a loop");
    for (int i = 0; i < 5; i++) { 
      bufByte = i*5;
      FRAMWrite(i, (byte*) bufByte);
      bufByte = 1;
      FRAMRead(i, (byte*) bufByte2);
      Serial.print(i);    
      Serial.print("\t");
      Serial.print(bufByte);
      Serial.print("\t");
      Serial.println(bufByte2); // Problem now that only zeroes are returned
    }
    */
    //Serial.println(" ");
    
    //delay(2000);
    
  }
