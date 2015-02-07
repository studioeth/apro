
#include "toneAC.h"
#include <Wire.h>
#include <HMC5883L.h>

// STATE
enum UserState {
  STAND,
  SIT,
  UNKNOWN
}userState;

UserState lastUserState;
// HARDWARE PIN SETTING ///
int PRESSURE = 0; // ANALOG
int DIST_TRIG = 8;
int DIST_ECHO = 7;
// int SOUND1 = 9;[used by toneAC]
// int SOUND2 = 10;[used by toneAC]

// PARAMS ///
int MAX_DISTANCE = 3000; //[mm]
int PRESSURE_TH = 200;
// sound
int MIN_FREQ = 5;
int MAX_FREQ = 35;
int MAX_VOL = 100;

// compass
HMC5883L compass;
int error = 0;
double defaultAngle;
double angle;
double MIN_ANGLE = 1.0;
double MAX_ANGLE = 7.0;

const int PRESS_ARRAY_SIZE = 10;
int lastPress[PRESS_ARRAY_SIZE];
const int DEGREE_ARRAY_SIZE = 10;
double lastDegree[DEGREE_ARRAY_SIZE];

void setup() {
  userState = UNKNOWN;

  Serial.begin(9600);
  pinMode(DIST_TRIG,OUTPUT);
  pinMode(DIST_ECHO,INPUT);
  
  //compass
  Serial.println("Starting the I2C interface.");
  Wire.begin(); // Start the I2C interface.
 
  Serial.println("Constructing new HMC5883L");
  compass = HMC5883L(); // Construct a new HMC5883 compass.
    
  Serial.println("Setting scale to +/- 1.3 Ga");
  error = compass.SetScale(1.3); // Set the scale of the compass.   スケールを設定
  if(error != 0) // If there is an error, print it out.
    Serial.println(compass.GetErrorText(error));
  
  Serial.println("Setting measurement mode to continous.");
  error = compass.SetMeasurementMode(Measurement_Continuous); // Set the measurement mode to Continuous
  if(error != 0) // If there is an error, print it out.
    Serial.println(compass.GetErrorText(error));

}

void loop() {
  angle = getAngle();
  userState = getPressure() > PRESSURE_TH ? SIT:STAND; 
  
  if(lastUserState != userState){
    defaultAngle = angle;
  }
  
  switch(userState){
  case STAND:
    distanceToTone(getDistance());
    break; // end STAND
    
  case SIT:
    angleToTone(defaultAngle - angle);
    break; // end SIT
  }
  
  delay( 50 ); // TODO
  lastUserState = userState;
    Serial.println("");
}

double getAngle(){
  // Retrive the raw values from the compass (not scaled).
  MagnetometerRaw raw = compass.ReadRawAxis();
  // Retrived the scaled values from the compass (scaled to the configured scale).
  MagnetometerScaled scaled = compass.ReadScaledAxis();
  
  float heading = atan2(scaled.YAxis, scaled.XAxis);
  double headingDegrees = heading * 180/M_PI; 
  double angle;
   int aveRange = 10;   
   double aveDeg = 0.0;
   for(int i = aveRange - 1; i >= 0; i-- ){
     if(i == 0){
       lastDegree[i] = headingDegrees;
     }else{
       lastDegree[i] = lastDegree[i-1];
     }
   }
   
   for(int i = 0; i < aveRange; i++ ){
     aveDeg += lastDegree[i];
   }
   aveDeg /= aveRange;
  return aveDeg;
}

void angleToTone(double angle){
  Serial.print(" DegDiff  \t");
  Serial.print(angle);
  
  if(abs(angle) > MIN_ANGLE){
    if( angle > MAX_ANGLE ){
      angle = MAX_ANGLE;
    }else if( angle < MAX_ANGLE * -1){
      angle = MAX_ANGLE * -1;
    }
    
    //TODO
    if(angle < 0){
      angle *= -1;
    }
    
    double freqRange = MAX_FREQ - MIN_FREQ;
    int freq = (angle - MIN_ANGLE) * freqRange / (MAX_ANGLE - MIN_ANGLE);
    int vol = (angle - MIN_ANGLE) * MAX_VOL / (MAX_ANGLE - MIN_ANGLE);
    int duration = 200;
    toneAC(freq, vol, duration);
    
    Serial.print(" freq[Hz]: ");
    Serial.print(freq);
    Serial.print(" vol: ");
    Serial.print(vol);
    Serial.print(" angle: ");
    Serial.print(angle);

  }
}

int getPressure(){
  int avePress = 0;
  
  for(int i = PRESS_ARRAY_SIZE - 1; i >= 0; i--){
    if(i == 0){
      lastPress[i] = analogRead(PRESSURE); 
    }else{
      lastPress[i] = lastPress[i-1];
    }
  }
  
  for(int i = 0; i < PRESS_ARRAY_SIZE; i++){
    avePress += lastPress[i];
  }
  avePress /= PRESS_ARRAY_SIZE;
  
  Serial.print(" avePressure: ");
  Serial.print(avePress);
  Serial.print("\t");

  return avePress;
}

void distanceToTone(long distance){
    Serial.print(" Dist[mm]: ");
    Serial.print(distance);
    
    if(distance < MAX_DISTANCE){
      int freqRange = MAX_FREQ - MIN_FREQ;
      int freq = MAX_FREQ - distance * freqRange / MAX_DISTANCE;
      int vol = MAX_VOL - distance * MAX_VOL / MAX_DISTANCE;
      int duration = 200;
      toneAC(freq, vol, duration);
      
      Serial.print(" freq[Hz]: ");
      Serial.print(freq);
      Serial.print(" vol: ");
      Serial.print(vol);
    }  
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
  if (duration > 0) {
    distance = duration * 17 / 100; // ultrasonic speed is 340m/s = 340000mm/s = 0.34mm/us 
    //Serial.print(duration);
    //Serial.print(" us ");
  }
  return distance;
}
