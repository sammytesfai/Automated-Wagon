                                                                                                                                                                                                                                                                                                              #define RIGHTPWM 5       // Set Speed of motor
#define RIGHTPWM 5       // Set Speed of motor
#define RIGHTFORWARD 31  // Move Forward
#define RIGHTBACKWARD 32 // Move Backward
#define RIGHTMOTORENCODERA 7
#define RIGHTMOTORENCODERB 8

#define LEFTPWM 3      // Set Speed of motor
#define LEFTFORWARD 27  // Move Forward
#define LEFTBACKWARD 26 // Move Backward
#define LEFTMOTORENCODERB 35
#define LEFTMOTORENCODERA 38
#define LEFTBUMPER 33

#define CAMERAMUXA 11
#define CAMERAMUXB 12
#define CAMERAMUXC 40
#define CAMERAMUXD 41

#define LEFTBACKTRIG 39
#define LEFTBACKECHO 13

#define NMOTORS 2

#include "Libraries\motors.h"
#include "Libraries\DFRobotIRPosition.h"

MOTORS M(RIGHTPWM, LEFTPWM, RIGHTFORWARD, RIGHTBACKWARD, LEFTFORWARD, LEFTBACKWARD);
DFRobotIRPosition myDFRobotIRPosition;

// Pins
const int enca[] = {RIGHTMOTORENCODERA,LEFTMOTORENCODERA};
const int encb[] = {RIGHTMOTORENCODERB,LEFTMOTORENCODERB};
const int pwm[] = {RIGHTPWM,LEFTPWM};
const int in1[] = {RIGHTFORWARD,LEFTFORWARD};
const int in2[] = {RIGHTBACKWARD,LEFTBACKWARD};

// Globals
long prevT = 0;
volatile int posi[] = {0,0};

int positionX[4];     ///< Store the X position
int positionY[4];     ///< Store the Y position

int positionX1[4];     ///< Store the X position
int positionY1[4];     ///< Store the Y position

int positionX2[4];     ///< Store the X position
int positionY2[4];     ///< Store the Y position

int randomarray[] = {300, 600, 500, 900, 1023, 700, 100};

void setup()
{
  Serial.begin(115200);
  // Right Motor
  pinMode(RIGHTMOTORENCODERA, INPUT);
  pinMode(RIGHTMOTORENCODERB, INPUT);

  // Left Motor
  pinMode(LEFTMOTORENCODERB, INPUT);
  pinMode(LEFTMOTORENCODERA, INPUT);
  pinMode(LEFTBUMPER, INPUT);

  pinMode(CAMERAMUXA, OUTPUT);
  pinMode(CAMERAMUXB, OUTPUT);
  pinMode(CAMERAMUXC, OUTPUT);
  pinMode(CAMERAMUXD, OUTPUT);
  
  set_pins(0);
  myDFRobotIRPosition.begin();

  
  set_pins(1);
  myDFRobotIRPosition.begin();

  pinMode(LEFTBACKTRIG, OUTPUT);
  pinMode(LEFTBACKECHO, INPUT);
}

long duration;
int distance;
void loop()
{
  while(check_sides(LEFTBACKTRIG, LEFTBACKECHO))
  {
    M.Forward_Right(255);
    Serial.println("RIGHT");
    delay(250);
    M.Backward(255);
    Serial.println("Forward");
    delay(250);
    
  }
  
  set_pins(0);
  delay(50);
  
  myDFRobotIRPosition.requestPosition();
  if (myDFRobotIRPosition.available()) 
  {
    for (int i=0; i<4; i++) 
    {
      positionX1[i]=myDFRobotIRPosition.readX(i);
      positionY1[i]=myDFRobotIRPosition.readY(i);
    }

    printResult(1);
  }
  else
  {
    Serial.println("Device not available!");
  }

  set_pins(1);
  delay(50);
  
  myDFRobotIRPosition.requestPosition();

  if (myDFRobotIRPosition.available()) 
  {
    for (int i=0; i<4; i++) 
    {
      positionX2[i]=myDFRobotIRPosition.readX(i);
      positionY2[i]=myDFRobotIRPosition.readY(i);
    }

    printResult(2);
  }
  else
  {
    Serial.println("Device not available!");
  }

  if(positionX1[0] > 300 && positionX1[0] < 1000 && positionX2[0] > 300 && positionX2[0] < 1000)
  {
    M.Backward(255);
  }
  else if(positionX1[0] < 300)
  {
    M.Forward_Right(255);
  }
  else if(positionX2[0] < 300)
  {
    M.Forward_Left(255);
  }
  else
  {
    M.STOP();
  }
}


//void loop() {
//
//  // set target position
//  int target[NMOTORS];
//  target[0] = 750*sin(prevT/1e6);
//  target[1] = -750*sin(prevT/1e6);
//
//  // time difference
//  long currT = micros();
//  float deltaT = ((float) (currT - prevT))/( 1.0e6 );
//  prevT = currT;
//
//  // Read the position
//  int pos[NMOTORS];
//  noInterrupts(); // disable interrupts temporarily while reading
//  for(int k = 0; k < NMOTORS; k++){
//      pos[k] = posi[k];
//    }
//  interrupts(); // turn interrupts back on
//  
//  // loop through the motors
//  for(int k = 0; k < NMOTORS; k++){
//    int pwr, dir;
//    // evaluate the control signal
//    pid[k].evalu(pos[k],target[k],deltaT,pwr,dir);
//    // signal the motor
//    setMotor(dir,pwr,pwm[k],in1[k],in2[k]);
//  }
//
//  for(int k = 0; k < NMOTORS; k++){
//    Serial.print(target[k]);
//    Serial.print(" ");
//    Serial.print(pos[k]);
//    Serial.print(" ");
//  }
//  Serial.println();
//}

// Check bumpers
//void loop() 
//{
//  Serial.println("A");
//  Serial.println(digitalRead(LEFTBUMPER));
//  Serial.println("B");
//  while(digitalRead(LEFTBUMPER))
//  {
//    Serial.println(digitalRead(LEFTBUMPER));
//    M.Backward(255);
//  }
//  Serial.println("C");
//  M.Forward(0);
//}

//int i = 0;
//// Check basic movement with 1 second delays
//void loop()
//{
//  Serial.println(i);
//  int pos = randomarray[i];
//  if(pos < 300 && pos > 0)
//  {
//    M.Forward_Left(255);
//  }
//  else if(pos > 700 && pos < 1000)
//  {
//    M.Forward_Right(255);
//  }
//  else if(pos >= 300 && pos <= 700)
//  {
//    M.Backward(255);
//  }
//  else
//  {
//    M.STOP();
//  }
//  delay(1000);
//  i = (i+1)%7;
//}

void set_pins(int flag)
{
  if(flag)
  {
    digitalWrite(CAMERAMUXA, HIGH);
    digitalWrite(CAMERAMUXB, HIGH);
    digitalWrite(CAMERAMUXC, LOW);
    digitalWrite(CAMERAMUXD, LOW);
  }
  else
  {
    digitalWrite(CAMERAMUXA, LOW);
    digitalWrite(CAMERAMUXB, LOW);
    digitalWrite(CAMERAMUXC, HIGH);
    digitalWrite(CAMERAMUXD, HIGH);
  }
}

void printResult(int num)
{
  if(num == 1)
  {
    Serial.print("Camera1: ");
    for (int i=0; i<4; i++) {
      Serial.print(positionX1[i]);
      Serial.print(",");
      
      Serial.print(positionY1[i]);
      Serial.print(";");
    }
  }
  else
  {
    Serial.print("Camera2: ");
    for (int i=0; i<4; i++) {
      Serial.print(positionX2[i]);
      Serial.print(",");
      
      Serial.print(positionY2[i]);
      Serial.print(";");
    }
  }
  Serial.println();
}

void setMotor(int dir, int pwmVal, int pwm, int in1, int in2){
  analogWrite(pwm,pwmVal);
  if(dir == 1){
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
  }
  else if(dir == -1){
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
  }
  else{
    digitalWrite(in1,LOW);
    digitalWrite(in2,LOW);
  }  
}

void readEncoderRight(){
  int b = digitalRead(encb[0]);
  if(b > 0){
    posi[0]++;
  }
  else{
    posi[0]--;
  }
}


void readEncoderLeft(){
  int b = digitalRead(encb[1]);
  if(b > 0){
    posi[1]++;
  }
  else{
    posi[1]--;
  }
}

bool check_sides(int trig, int echo)
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  distance = duration*0.034/2;
  if(distance < 4)return true;
  return false;
}




