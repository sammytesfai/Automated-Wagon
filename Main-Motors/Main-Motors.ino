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
char front_sensor_side;

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

//  echo_init();
}

void loop()
{
  // Right Camera
  set_pins(0);
  delay(25);
  get_camera_vals('R');

  // Left Camera
  set_pins(1);
  delay(25);
  get_camera_vals('L');
  
  perform_movement();
  delay(1000);
}


//void loop()
//{
//  while((front_sensor_side = check_front_sensors()) != NULL)
//  {
//    if(front_sensor_side == 'R')
//    {
//      M.Forward_Left(255);
//      delay(600);
//      M.Forward(255);
//      delay(1000);
//      M.Forward_Right(255);
//      delay(600);
//    }
//    else
//    {
//      M.Forward_Right(255);
//      delay(600);
//      M.Forward(255);
//      delay(1000);
//      M.Forward_Left(255);
//      delay(600);
//    }
//  }
//  
//  while(echo_confidence(LEFTTRIGGER, LEFTECHO, 'L') < SIDE_DISTANCE)
//  {
//    M.Forward_Right(255);
//    Serial.println("RIGHT");
//    delay(150);
//    M.Forward(255);
//    Serial.println("Forward");
//    delay(150);
//  }
//
//  while(echo_confidence(RIGHTTRIGGER, RIGHTECHO, 'R') < SIDE_DISTANCE)
//  {
//    M.Forward_Left(255);
//    Serial.println("LEFT");
//    delay(150);
//    M.Forward(255);
//    Serial.println("Forward");
//    delay(150);
//  }
//
//  // Right Camera
//  set_pins(0);
//  delay(25);
//  get_camera_vals('R');
//
//  // Left Camera
//  set_pins(1);
//  delay(25);
//  get_camera_vals('L');
//
//  perform_movement();
//
////  Serial.println("Arduino1");
//  Wire.requestFrom(0x9,4);
//  while(Wire.available())
//  {
//    rssi += Wire.read() << shift;
//    shift += 8;
//  }
////  Serial.println(rssi);
//  rssi = 0;
//  shift = 0;
////  delay(100);
//
////  Serial.println("Arduino2");
//  Wire.requestFrom(0x91,4);
//  while(Wire.available())
//  {
//    rssi += Wire.read() << shift;
//    shift += 8;
//  }
////  Serial.println(rssi);
//  rssi = 0;
//  shift = 0;
////  delay(100);
//}

bool get_camera_vals(char side)
{
  myDFRobotIRPosition.requestPosition();
  delay(25);
  if (myDFRobotIRPosition.available())
  {
    if(side == 'R')
    {
      positionXR = myDFRobotIRPosition.readX(0);
      positionYR = myDFRobotIRPosition.readY(0);
      printResult(positionXR, 1);
    }
    else
    {
      positionXL = myDFRobotIRPosition.readX(0);
      positionYL = myDFRobotIRPosition.readY(0);
      printResult(positionXL, 2);
    }
    return true;
  }
  Serial.println("Device not available!");
  return false;
}

void perform_movement()
{
  Serial.print("YR");
  Serial.println(positionYR);
  Serial.print("YL");
  Serial.println(positionYL);

//  if ((positionXR <500 && positionXL > 400)){
//    Serial.println("Forward");
//    M.Forward(255);
//  }
//  else if(positionXR > 500 && positionXR != 1023)
//  {
//    Serial.println("Right");
//    M.Forward_Right(255);
//  }
//  else if(positionXL < 400 && positionXL !=1023)
//  {
//    Serial.println("Left");
//    M.Forward_Left(255);
//  }
//  else
//  {
//    M.STOP();
//  }


  
  if(positionXR > 200 && positionXR < 1000 && positionXL > 200 && positionXL < 1000)
  {
    M.Forward(255);
  }
  else if(positionXR < 400)
  {
    M.Forward_Right(200);
  }
  else if(positionXL < 400)
  {
    M.Forward_Left(200);
  }
  else
  {
    M.STOP();
  }
}
