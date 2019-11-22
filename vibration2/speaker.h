#ifndef SPEAKER_H__
#define SPEAKER_H__

int CS = 9; //CS
int DataOUT = 11; //MOSI for DAC
int DataIn = 12;   //for ADC
int SCLK = 13; //clock pin
int CS2 = 10; //ADC CS
int BUSY = 15;  //ADC
int CNVST = 16; //ADC
uint8_t byte_0, byte_1, byte_2, byte_3; //FOR ADC byte shift


//IIR filter
float adc1, adc1_last = 0;
float adc2, adc2_last = 0;
float xv[3] = {0, 0, 0};
float yv[3] = {0, 0, 0};
float xv2[3] = {0, 0, 0};
float yv2[3] = {0, 0, 0};
float GAIN = 3.414213562e+00;  // gain number for IIR filter 








void setDac (char channel, int voltage);
int twocompconv(int s);
void wait(int n);
void iir_filter(float *adc1, float *adc2);
void adc_read(float *adc1, float *adc2);

void setDac (char channel, int voltage) {  // Set voltage for the speaker. 0=-10V, 4196=10V 12bits
  unsigned short t = 0;
  t = channel << 15; //a is at the very end of the data transfer
  t = t | 0b0111000000000000;   //set first a couple settings
  t = t | (voltage & 0b0111111111111);  //get rid of bits

  //rejecting excessive bits (above 10)
  SPI.beginTransaction(mysetting);
  noInterrupts();

  digitalWrite(CS, LOW);
  SPI.transfer(t >> 8); //transfer bits
  SPI.transfer(t & 0xff);
  digitalWrite(CS, HIGH);
  interrupts();
  SPI.endTransaction();
}

int twocompconv(int s) { //two's complement
  if (s >= 8192) {
    s = s - 8192;
  }
  else {
    s = s + 8191;
  }
  return s;
}

void wait(int n) {  //delay
  char a = 0;
  for (int i = 0; i < n; i++) {
    a << 1;
  }
}

void iir_filter(float *adc1, float *adc2) { //corner fq 500hz

  xv[0] = xv[1];
  xv[1] = xv[2];
  xv[2] = *adc1 / GAIN;
  yv[0] = yv[1];
  yv[1] = yv[2];
  yv[2] =   (xv[0] + xv[2]) + 2 * xv[1]
            + ( -0.1715728753 * yv[0]) + ( 0 * yv[1]);
  *adc1 = yv[2];

  xv2[0] = xv2[1];
  xv2[1] = xv2[2];
  xv2[2] = *adc2 / GAIN;
  yv2[0] = yv2[1];
  yv2[1] = yv2[2];
  yv2[2] =   (xv2[0] + xv2[2]) + 2 * xv2[1]
             + ( -0.1715728753 * yv2[0]) + (  0 * yv2[1]);
  *adc2 = yv2[2];

}

void adc_read(float *adc1, float *adc2) { //read hall effect sensor using ADC 14 bits
  short adcvalue1 = 0;
  short adcvalue2 = 0;

  digitalWrite(CNVST, LOW);
  wait(1);
  digitalWrite(CNVST, HIGH);
  while (digitalRead(BUSY) == HIGH) {
    ;
  }
  SPI.beginTransaction(mysetting);
  noInterrupts();
  digitalWrite(CS2, LOW);
  byte_0 = SPI.transfer(0); // read firt 8 bits
  byte_1 = SPI.transfer(0); // read second 8 bits
  byte_2 = SPI.transfer(0);
  byte_3 = SPI.transfer(0);
  digitalWrite(CS2, HIGH);

  interrupts();
  SPI.endTransaction();
  adcvalue1 = (byte_0 << 6) | (byte_1 >> 2);
  adcvalue2 = (((byte_1 & 0b11) << 12) | (byte_2 << 4)) | (byte_3 >> 4);


  *adc1 = (twocompconv(adcvalue1) - 8192) * 0.00061;
  *adc2 = (twocompconv(adcvalue2) - 8192) * 0.00061 ;

}
#endif
