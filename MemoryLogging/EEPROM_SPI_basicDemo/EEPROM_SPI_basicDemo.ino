// From http://arduino.cc/en/Tutorial/SPIEEPROM

// Now we will write the code to enable SPI communication between the EEPROM and the Arduino. 
// In the setup routine this program fills 128 bytes, or one page of the EEPROM with data. 
// In the main loop it reads that data back out, one byte at a time and prints that byte out the built in serial port. 
// We will walk through the code in small sections.

// The first step is setting up our pre-processor directives. 
// Pre-processor directives are processed before the actual compilation begins. 
// They start with a "#" and do not end with semi-colons.

// We define the pins we will be using for our SPI connection, DATAOUT, DATAIN, SPICLOCK and SLAVESELECT. 
// Then we define our opcodes for the EEPROM. Opcodes are control commands:
#define DATAOUT 11//MOSI
#define DATAIN  12//MISO 
#define SPICLOCK  13//sck
#define SLAVESELECT 2//ss (SS-1 in Daysimeter)

//opcodes
#define WREN  6
#define WRDI  4
#define RDSR  5
#define WRSR  1
#define READ  3
#define WRITE 2 

// Here we allocate the global variables we will be using later in the program. 
// Note char buffer [128];. 
// this is a 128 byte array we will be using to store the data for the EEPROM write:
byte eeprom_output_data;
byte eeprom_input_data=0;
byte clr;
int address=0;

//data buffer
char buffer [128]; 

// First we initialize our serial connection, 
// set our input and output pin modes and set the SLAVESELECT line high to start. 
// This deselects the device and avoids any false transmission messages due to line noise:
void setup()
{
  Serial.begin(9600);

  pinMode(DATAOUT, OUTPUT);
  pinMode(DATAIN, INPUT);
  pinMode(SPICLOCK,OUTPUT);
  pinMode(SLAVESELECT,OUTPUT);
  digitalWrite(SLAVESELECT,HIGH); //disable device 

// Now we set the SPI Control register (SPCR) to the binary value 01010000. 
// In the control register each bit sets a different functionality. 
// The eighth bit disables the SPI interrupt, the seventh bit enables the SPI, 
// the sixth bit chooses transmission with the most significant bit going first, 
// the fifth bit puts the Arduino in Master mode, the fourth bit sets the data clock idle when it is low, 
// the third bit sets the SPI to sample data on the rising edge of the data clock, 
// and the second and first bits set the speed of the SPI to system speed / 4 (the fastest). 

// After setting our control register up we read the SPI status register (SPSR) 
// and data register (SPDR) in to the junk clr variable to clear out any spurious data from past runs:
 
  // SPCR = 01010000
  //interrupt disabled,spi enabled,msb 1st,master,clk low when idle,
  //sample on leading edge of clk,system clock/4 rate (fastest)
  SPCR = (1<<SPE)|(1<<MSTR);
  clr=SPSR;
  clr=SPDR;
  delay(10); 

  // Here we fill our data array with numbers and send a write enable instruction to the EEPROM. 
  // The EEPROM MUST be write enabled before every write instruction. To send the instruction we pull the SLAVESELECT line low, 
  // enabling the device, and then send the instruction using the spi_transfer function. 
  // Note that we use the WREN opcode we defined at the beginning of the program. 
  // Finally we pull the SLAVESELECT line high again to release it:

  //fill buffer with data
  fill_buffer();
  //fill eeprom w/ buffer
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(WREN); //write enable, ~WP tied to VCC in Daysimeter
  digitalWrite(SLAVESELECT,HIGH);
  
  // Now we pull the SLAVESELECT line low to select the device again after a brief delay. 
  // We send a WRITE instruction to tell the EEPROM we will be sending data to record into memory. 
  // We send the 16 bit address to begin writing at in two bytes, Most Significant Bit first. 
  // Next we send our 128 bytes of data from our buffer array, one byte after another without pause. 
  // Finally we set the SLAVESELECT pin high to release the device and pause to allow the EEPROM to write the data:

  delay(10);
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(WRITE); //write instruction
  address=0;
  spi_transfer((char)(address>>8));   //send MSByte address first
  spi_transfer((char)(address));      //send LSByte address
  //write 128 bytes
  for (int I=0;I<128;I++)
  {
    spi_transfer(buffer[I]); //write data byte
  }
  digitalWrite(SLAVESELECT,HIGH); //release chip
  //wait for eeprom to finish writing
  delay(3000); 
  
  // We end the setup function by sending the word "hi" plus a line feed out the built in 
  // serial port for debugging purposes. 
  // This way if our data comes out looking funny later on we can tell it isn't just the serial port acting up:
  Serial.write( byte('h') ); // Serial.print('h',BYTE);
  Serial.write( byte('i') ); // Serial.print('i',BYTE);
  Serial.write( byte('\n') ); // Serial.print('\n',BYTE);//debug
  delay(1000);
} // END OF SETUP

// In our main loop we just read one byte at a time from the EEPROM and print it out the serial port. 
// We add a line feed and a pause for readability. Each time through the loop we increment the eeprom address to read. 
// When the address increments to 128 we turn it back to 0 because we have only filled 128 addresses in the EEPROM with data:

void loop()
{
  eeprom_output_data = read_eeprom(address);
  Serial.print(address);
  Serial.print('\t');
  Serial.print(eeprom_output_data,DEC);
  Serial.write( byte('\n') ); // Serial.print('\n',BYTE);//debug
  address++;
  delay(500); //pause for readability
} 

// The fill_buffer function simply fills our data array with numbers 0 - 127 for each index in the array. 
// This function could easily be changed to fill the array with data relevant to your application:
void fill_buffer()
{
  for (int I=0;I<128;I++)
  {
    buffer[I]=I;
  }
} 

// The spi_transfer function loads the output data into the data transmission register, 
// thus starting the SPI transmission. It polls a bit to the SPI Status register (SPSR) 
// to detect when the transmission is complete using a bit mask, SPIF. 
// An explanation of bit masks can be found here (http://www.arduino.cc/en/Tutorial/BitMask). 
// It then returns any data that has been shifted in to the data register by the EEPROM:
char spi_transfer(volatile char data)
{
  SPDR = data;                    // Start the transmission
  while (!(SPSR & (1<<SPIF)))     // Wait for the end of the transmission
  {
  };
  return SPDR;                    // return the received byte
}

// The read_eeprom function allows us to read data back out of the EEPROM. 
// First we set the SLAVESELECT line low to enable the device. 
// Then we transmit a READ instruction, followed by the 16-bit address we wish to read from, 
// Most Significant Bit first. Next we send a dummy byte to the EEPROM for the purpose 
// of shifting the data out. 

// Finally we pull the SLAVESELECT line high again to release the device after reading one byte, 
// and return the data. If we wanted to read multiple bytes at a time we could hold the SLAVESELECT 
// line low while we repeated the data = spi_transfer(0xFF); up to 128 times for a full page of data:
byte read_eeprom(int EEPROM_address)
{
  //READ EEPROM
  int data;
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(READ); //transmit read opcode
  spi_transfer((char)(EEPROM_address>>8));   //send MSByte address first
  spi_transfer((char)(EEPROM_address));      //send LSByte address
  data = spi_transfer(0xFF); //get data byte
  digitalWrite(SLAVESELECT,HIGH); //release chip, signal end transfer
  return data;
}



