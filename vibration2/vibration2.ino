#include<SPI.h>
#include "friction.h"
#include "input.h"
#include "speaker.h"


float pi = 3.1415;
int x =0;
float Pc=0;  //The height of plate
int Pc2=0 //DAC modulation of the sine wave from dds chip. value ranges from 0 to 900. 
int dac_left, dac_right=0; //DAC value for left/right speaker. Range from 0 to 4196.

enum mode {TRACK, RTEST, ETEST, COMB}; 
enum mode current_mode;
IntervalTimer PID; // timer based interrupt. 1k hz

void controller () {

  if (x > 1000) {
    x = 1;
  }
  x++;

  switch (current_mode) {
    case TRACK: //actuating speaker only. 

      Pc = sin_Max * sin(x * 2 * 3.1415 / 1000 * sin_fq); //Pc is the amplitude. Max=1.
      //sin_fq determine frequency 50 for 50hz


      dac_left = Pc  * 1800 + 1800; //convert to DAC value
      dac_right = Pc * 1870 + 1870;
      
      setDac(1, dac_right); 
      setDac(0, dac_left);

      break;

    case ETEST: //electro-adhesion only. 


      Pc2 = 450 + 450 * sin(x * 2 * 3.1415 / 1000 * sin_fq); //sin_fq determine frequency 50 for 50hz

      setDac2(0, Pc2);


      break;


    case COMB: //combine both vibration and electro-adhesion

      Pc = sin_Max * sin(x * 2 * 3.1415 / 1000 * sin_fq);
      Pc2 = 450 + 450 * sin(x * 2 * 3.1415 / 1000 * sin_fq);


      dac_left = Pc / 0.5 * 1800 + 1800;
      dac_right = Pc / 0.5 * 1870 + 1870;

      setDac(1, dac_right);
      setDac(0, dac_left);

      setDac2(0, Pc2);



      break;

    


  }
}

void setup() {
  Serial.begin(9600);
  pinMode(CS, OUTPUT);
  pinMode(CS2, OUTPUT);
  pinMode(CS3, OUTPUT);
  pinMode(FSYNC, OUTPUT);
  pinMode(DataOUT, OUTPUT);
  pinMode(SCLK, OUTPUT);
  pinMode(CNVST, OUTPUT);
  digitalWrite(CS, HIGH);
  digitalWrite(CS2, HIGH);
  digitalWrite(CS3, HIGH);
  digitalWrite(CNVST, HIGH);
  digitalWrite(FSYNC, HIGH);
  SPI.begin();

  delay(20);
  AD9833setFrequency(freq);  //start DDS to get 20khz sin wave
  PID.begin(controller, 1000); //1000= 1k hz interrupt

  setDac(0, 1800);
  setDac(1, 1870);
  setDac2(0, 0);
  current_mode = ETEST;

}

void loop() {




  if (Serial.available()) {
    buff[0] = Serial.read();

  }

  // input from terminal: t for speaker only, e for electro-adhesion only, m for combined
  // c changes the amplitude and frequency
  // input example:  c<0.5,50>

  switch (buff[0]) {
    case 't':
      current_mode = TRACK;
      Serial.println(current_mode);
      break;
    case 'e':
      current_mode = ETEST;
      Serial.println(current_mode);

      break;
    case 'm':
      current_mode = COMB;
      Serial.println(current_mode);
      break;




    case'c':
      recvWithStartEndMarkers();
      if (newData == true) {
        strcpy(tempChars, receivedChars);

        parseSine();

        newData = false;
      }



      break;
  }

}
