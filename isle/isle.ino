
#include "toneAC.h"

// STATE
enum UserState {
  STAND,
  SIT
}userState;

// HARDWARE PIN SETTING ///
int DIST_TRIG = 8;
int DIST_ECHO = 7;

// PARAMS ///
int MAX_DISTANCE = 3000; //[mm]

// sound
int MIN_FREQ = 5;
int MAX_FREQ = 35;
int MAX_VOL = 100;

void setup() {
  userState = STAND;

  Serial.begin(9600);
  pinMode(DIST_TRIG,OUTPUT);
  pinMode(DIST_ECHO,INPUT);
}

void loop() {
  
  switch(userState){
  case STAND:
    distanceToTone(getDistance());
    break; // end STAND
    
  case SIT:
    break; // end SIT
    
  }
}

void distanceToTone(long distance){
    Serial.print(distance);
    Serial.print(" mm  ");
    
    if(distance < MAX_DISTANCE){
      int freqRange = MAX_FREQ - MIN_FREQ;
      int freq = MAX_FREQ - distance * freqRange / MAX_DISTANCE;
      int vol = MAX_VOL - distance * MAX_VOL / MAX_DISTANCE;
      int duration = 1000;
      toneAC(freq, vol, duration);
      
      Serial.print(freq);
      Serial.print(" freq ");
      Serial.print(vol);
      Serial.print(" vol");
    }  
    Serial.println("");
}

long getDistance(){
  long duration;//[us]
  long distance = 0;
  digitalWrite(DIST_TRIG,LOW);
  delayMicroseconds(1);
  digitalWrite(DIST_TRIG,HIGH);
  delayMicroseconds(1);
  digitalWrite(DIST_TRIG,LOW);
  duration = pulseIn(DIST_ECHO,HIGH);
  if (duration>0) {
    distance = duration * 17 / 100; // ultrasonic speed is 340m/s = 340000mm/s = 0.34mm/us 
    //Serial.print(duration);
    //Serial.print(" us ");
  }
  return distance;
}
