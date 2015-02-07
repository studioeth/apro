
// STATE
enum UserState {
  stand,
  sit
}userState;

// HARDWARE PIN SETTING ///
int SPEAKER = 7;
int DIST_TRIG = 8;
int DIST_ECHO = 9;

// PARAMS
int distance; //[mm]

void setup() {
  userState = stand;

//  Serial.begin(9600);
  pinMode(DIST_TRIG,OUTPUT);
  pinMode(DIST_ECHO,INPUT);
}

void loop() {
  distance = getDistance();
  // TODO distance to vol and fq
  int freq = 36;
  int vol = 100;
  int duration = 1000;
  tone(SPEAKER, duration,freq);
}

int getDistance(){
  int duration;//[us]
  int distance = 0;
  digitalWrite(DIST_TRIG,LOW);
  delayMicroseconds(1);
  digitalWrite(DIST_TRIG,HIGH);
  delayMicroseconds(1);
  digitalWrite(DIST_TRIG,LOW);
  duration = pulseIn(DIST_ECHO,HIGH);
  if (duration>0) {
    distance = duration*340*100/100000/2; // ultrasonic speed is 340m/s = 34000cm/s = 0.034cm/us 
    Serial.print(duration);
    Serial.print(" us ");
    Serial.print(distance);
    Serial.println(" mm");
  }
  return distance;
}
