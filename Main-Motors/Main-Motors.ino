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

#define CAMERA_SEL_A0 11
#define CAMERA_SEL_A1 12
#define CAMERA_SEL_A2 13
#define CAMERA_SEL_A3 39

#define LEFTTRIGGER 34
#define RIGHTTRIGGER 37
#define LEFTECHO 40
#define RIGHTECHO 41

#include "Libraries\motors.h"
#include "Libraries\DFRobotIRPosition.h"

int left_echo [5];
int right_echo [5];

MOTORS M(RIGHTPWM, LEFTPWM, RIGHTFORWARD, RIGHTBACKWARD, LEFTFORWARD, LEFTBACKWARD);
DFRobotIRPosition myDFRobotIRPosition;

int positionXR[4];     ///< Store the X position
int positionYR[4];     ///< Store the Y position

int positionXL[4];     ///< Store the X position
int positionYL[4];     ///< Store the Y position

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

  pinMode(CAMERA_SEL_A0, OUTPUT);
  pinMode(CAMERA_SEL_A1, OUTPUT);
  pinMode(CAMERA_SEL_A2, OUTPUT);
  pinMode(CAMERA_SEL_A3, OUTPUT);

  // Right Camera
  set_pins(0);
  myDFRobotIRPosition.begin();

  // Left Camera
  set_pins(1);
  myDFRobotIRPosition.begin();

  pinMode(LEFTTRIGGER, OUTPUT);
  pinMode(RIGHTTRIGGER, OUTPUT);
  pinMode(LEFTECHO, INPUT);
  pinMode(RIGHTECHO, INPUT);
}

//void loop()
//{
//  delay(50);
//  myDFRobotIRPosition.requestPosition();
//
//  if (myDFRobotIRPosition.available()) 
//  {
//    for (int i=0; i<4; i++) 
//    {
//      positionXL[i]=myDFRobotIRPosition.readX(i);
//      positionYL[i]=myDFRobotIRPosition.readY(i);
//    }
//
//    printResult(2);
//  }
//}

long duration;
int distance;
void loop()
{
  while(check_sides(LEFTTRIGGER, LEFTECHO, 'L'))
  {
    echo_confidence(LEFTTRIGGER, LEFTECHO, 'L');
    if(echo_avg('L') < 8)
    {
      M.Forward_Left(255);
      Serial.println("LEFT");
      delay(100);
      M.Backward(255);
      Serial.println("Forward");
      delay(100);
    }
  }

  while(check_sides(RIGHTTRIGGER, RIGHTECHO, 'R'))
  {
    echo_confidence(RIGHTTRIGGER, RIGHTECHO, 'R');
    if(echo_avg('R') < 8)
    {
      M.Forward_Right(255);
      Serial.println("RIGHT");
      delay(100);
      M.Backward(255);
      Serial.println("Forward");
      delay(100);
    }
  }

  // Right Camera
  set_pins(0);
  delay(50);
  
  myDFRobotIRPosition.requestPosition();
  if (myDFRobotIRPosition.available())
  {
    for (int i=0; i<4; i++)
    {
      positionXR[i]=myDFRobotIRPosition.readX(i);
      positionYR[i]=myDFRobotIRPosition.readY(i);
    }

    printResult(1);
  }
  else
  {
    Serial.println("Device not available!");
  }


  // Left Camera
  set_pins(1);
  delay(50);
  
  myDFRobotIRPosition.requestPosition();

  if (myDFRobotIRPosition.available()) 
  {
    for (int i=0; i<4; i++) 
    {
      positionXL[i]=myDFRobotIRPosition.readX(i);
      positionYL[i]=myDFRobotIRPosition.readY(i);
    }

    printResult(2);
  }
  else
  {
    Serial.println("Device not available!");
  }

  if(positionXR[0] > 300 && positionXR[0] < 1000 && positionXL[0] > 300 && positionXL[0] < 1000)
  {
    M.Backward(255);
  }
  else if(positionXR[0] < 300)
  {
    M.Forward_Right(255);
  }
  else if(positionXL[0] < 300)
  {
    M.Forward_Left(255);
  }
  else
  {
    M.STOP();
  }
}

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
    digitalWrite(CAMERA_SEL_A0, HIGH);
    digitalWrite(CAMERA_SEL_A1, HIGH);
    digitalWrite(CAMERA_SEL_A2, LOW);
    digitalWrite(CAMERA_SEL_A3, LOW);
  }
  else
  {
    digitalWrite(CAMERA_SEL_A0, LOW);
    digitalWrite(CAMERA_SEL_A1, LOW);
    digitalWrite(CAMERA_SEL_A2, HIGH);
    digitalWrite(CAMERA_SEL_A3, HIGH);
  }
}

void printResult(int num)
{
  if(num == 1)
  {
    Serial.print("Camera1: ");
    for (int i=0; i<4; i++) {
      Serial.print(positionXR[i]);
      Serial.print(",");
      
      Serial.print(positionYR[i]);
      Serial.print(";");
    }
  }
  else
  {
    Serial.print("Camera2: ");
    for (int i=0; i<4; i++) {
      Serial.print(positionXL[i]);
      Serial.print(",");
      
      Serial.print(positionYL[i]);
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


bool check_sides(int trig, int echo, char side)
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  distance = duration*0.034/2;
  if(distance < 8)
  {
    if(side == 'L') left_echo[0] = distance;
    else right_echo[0] = distance;
    return true;
  }
  return false;
}

void echo_confidence(int trig, int echo, char side)
{
  for(int i  = 0; i < 4; i++)
  {
    digitalWrite(trig, LOW);
    delayMicroseconds(2);

    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    duration = pulseIn(echo, HIGH);
    distance = duration*0.034/2;

    if(side == 'L') left_echo[i+1] = distance;
    else right_echo[i+1] = distance;
  }
}

int echo_avg(char side)
{
  int total = 0;
  if(side == 'L')for(int i = 0; i < 5; i++) total += left_echo[i];
  else for(int i = 0; i < 5; i++) total += right_echo[i];
  return total/5;
}




