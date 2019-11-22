#ifndef FRICTION_H__
#define FRICTION_H__

const int SINE = 0x2000;                    // Define AD9833's waveform register value FOR SINE WAVE.
const float refFreq = 20000000.0;           // On-board crystal reference frequency in HZ
const int FSYNC = 20;                       // Standard SPI pins for the AD9833 waveform generator.
const int CS3 = 21;                         //CS pin for modulation DAC
unsigned long freq = 20000;               // Set initial frequency.
SPISettings mysetting(10000000, MSBFIRST, SPI_MODE0); //for ADC & DAC
SPISettings mysetting2(1000000, MSBFIRST, SPI_MODE2); //for DDS chip

//functions
void AD9833setFrequency(long frequency);
void WriteRegister(int dat);
void setDac2 (char channel, int voltage);





// Set the frequency and waveform registers in the AD9833.
void AD9833setFrequency(long frequency) {

  long FreqWord = (frequency * pow(2, 28)) / refFreq;

  int MSB = (((int) FreqWord) & 0xFFFC000) >> 14;    //Only lower 14 bits are used for data
  int LSB = (((int) FreqWord) & 0x3FFF);

  //Set control bits 15 ande 14 to 0 and 1, respectively, for frequency register 0
  LSB |= 0x4000;
  MSB |= 0x4000;
  WriteRegister(0x2100);
  WriteRegister(LSB);                  // Write lower 16 bits to AD9833 registers
  WriteRegister(MSB);                  // Write upper 16 bits to AD9833 registers.
  WriteRegister(0xC000);               // Phase register
  WriteRegister(0x2000);
}

void WriteRegister(int dat) {

  //AD9833 use different SPI MODE so it has to be set for the AD9833 here.
  SPI.beginTransaction(mysetting2);
  noInterrupts();
  digitalWrite(FSYNC, LOW);           // Set FSYNC low before writing to AD9833 registers
  delay(1);
  SPI.transfer(highByte(dat));        // Each AD9833 register is 32 bits wide and each 16
  SPI.transfer(lowByte(dat));         // bits has to be transferred as 2 x 8-bit bytes.
  delay(1);
  digitalWrite(FSYNC, HIGH);          //Write done. Set FSYNC high
  delay(1);
  interrupts();
  SPI.endTransaction();
}

void setDac2 (char channel, int voltage) {  //DAC modulation for sin wave
  unsigned short t = 0;

  if (voltage <= 1) {
    voltage = 1;
  }
  t = channel << 15; //a is at the very end of the data transfer
  t = t | 0b0111000000000000;   //set first a couple settings
  t = t | (voltage & 0b0111111111111);  //get rid of bits

  //rejecting excessive bits (above 10)
  SPI.beginTransaction(mysetting);
  noInterrupts();

  digitalWrite(CS3, LOW);
  SPI.transfer(t >> 8);
  SPI.transfer(t & 0xff);
  digitalWrite(CS3, HIGH);
  interrupts();
  SPI.endTransaction();
}

#endif
