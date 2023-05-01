#include <Wire.h>
#include "motors.h"
#include "DFRobotIRPosition.h"
#include "misc.h"

MOTORS M(RIGHTPWM, LEFTPWM, RIGHTFORWARD, RIGHTBACKWARD, LEFTFORWARD, LEFTBACKWARD);
DFRobotIRPosition myDFRobotIRPosition;

// Store X & Y position for both cameras
int positionXR, positionYR, positionXL, positionYL;

int randomarray[] = {300, 600, 500, 900, 1023, 700, 100};

int shift = 0;
int rssi = 0;

void setup()
{
  Serial.begin(115200);
  Wire.begin();

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

  // Side US sensor I/O pins
  pinMode(LEFTTRIGGER, OUTPUT);
  pinMode(RIGHTTRIGGER, OUTPUT);
  pinMode(LEFTECHO, INPUT);
  pinMode(RIGHTECHO, INPUT);
  
  // FRONT US sensor I/O pins
  pinMode(FRONTLEFTTRIG, OUTPUT);
  pinMode(FRONTLEFTECHO, INPUT);
  pinMode(FRONTRIGHTTRIG, OUTPUT);
  pinMode(FRONTRIGHTECHO, INPUT);

  echo_init();
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
//    printResult(positionXL, 2);
//  }
//}

void loop()
{
  while(check_front_sensors())
  {
    M.STOP();
  }
  while(echo_confidence(LEFTTRIGGER, LEFTECHO, 'L') < SIDE_DISTANCE)
  {
    M.Forward_Left(255);
    Serial.println("LEFT");
    delay(150);
    M.Backward(255);
    Serial.println("Forward");
    delay(150);
  }

  while(echo_confidence(RIGHTTRIGGER, RIGHTECHO, 'R') < SIDE_DISTANCE)
  {
    M.Forward_Right(255);
    Serial.println("RIGHT");
    delay(150);
    M.Backward(255);
    Serial.println("Forward");
    delay(150);
  }

  // Right Camera
  set_pins(0);
  delay(25);
  
  myDFRobotIRPosition.requestPosition();
  delay(25);
  if (myDFRobotIRPosition.available())
  {
    positionXR = myDFRobotIRPosition.readX(0);
    positionYR = myDFRobotIRPosition.readY(0);
//    printResult(positionXR, 1);
  }
  else
  {
    Serial.println("Device not available!");
  }


  // Left Camera
  set_pins(1);
  delay(25);
  
  myDFRobotIRPosition.requestPosition();
  delay(25);
  if (myDFRobotIRPosition.available()) 
  {
    positionXL = myDFRobotIRPosition.readX(0);
    positionYL = myDFRobotIRPosition.readY(0);
//    printResult(positionXL, 2);
  }
  else
  {
    Serial.println("Device not available!");
  }

  perform_movement();

//  Serial.println("Arduino1");
  Wire.requestFrom(0x9,4);
  while(Wire.available())
  {
    rssi += Wire.read() << shift;
    shift += 8;
  }
//  Serial.println(rssi);
  rssi = 0;
  shift = 0;
//  delay(100);

//  Serial.println("Arduino2");
  Wire.requestFrom(0x91,4);
  while(Wire.available())
  {
    rssi += Wire.read() << shift;
    shift += 8;
  }
//  Serial.println(rssi);
  rssi = 0;
  shift = 0;
//  delay(100);
}

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


void perform_movement()
{
  if(positionXR > 300 && positionXR < 1000 && positionXL > 300 && positionXL < 1000)
  {
    M.Backward(255);
  }
  else if(positionXR < 300)
  {
    M.Forward_Right(255);
  }
  else if(positionXL < 300)
  {
    M.Forward_Left(255);
  }
  else
  {
    M.STOP();
  }
}
