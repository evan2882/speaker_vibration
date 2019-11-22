#ifndef INPUT_H__
#define INPUT_H__
#define BUF_SIZE 1000
//for serial setup kp ki kd   input: b <kp,ki,kd>
const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];
boolean newData = false;

float ki = 30;     // for left speaker 0
float kp = 50;
float kd = 1000;

float ki2 = 50;  //for right speaker 1
float kp2 = 150;
float kd2 = 2500;

//constant to change amplitude and frequency

float sin_Max = 0.5; //amplitude max =1
float sin_fq=50; //frequency
char buff[BUF_SIZE]; //for input detection

// the following functions parse input from terminal 
// input example c<0.1,50>
void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }

  }
}

void parseData() {      // split the data into its parts

  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ",");     // get the first part - the string
  kp = atof(strtokIndx); // copy it to messageFromPC

  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  ki = atof(strtokIndx);  // convert this part to an integer

  strtokIndx = strtok(NULL, ",");
  kd = atof(strtokIndx);   // convert this part to a float



}

void parseData2() {      // split the data into its parts

  char * strtokIndx; // this is used by strtok() as an index
  strtokIndx = strtok(tempChars, ","); // this continues where the previous call left off
  kp2 = atof(strtokIndx);  // convert this part to an integer

  strtokIndx = strtok(NULL, ",");
  ki2 = atof(strtokIndx);   // convert this part to a float

  strtokIndx = strtok(NULL, ",");
  kd2 = atof(strtokIndx);   // convert this part to a float

}

void parseSine() {      // split the data into its parts

  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ","); // this continues where the
  sin_Max = atof(strtokIndx);  // convert this part to an integer
  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  sin_fq = atof(strtokIndx);  // convert this part to an integer

  Serial.print("sin_max ");
  Serial.println(sin_Max);
  Serial.print("freq");
  Serial.println(sin_fq);



}

#endif
