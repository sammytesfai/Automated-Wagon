#include <Wire.h>
#include "motors.h"
#include "DFRobotIRPosition.h"
#include "misc.h"

MOTORS M(RIGHTPWM, LEFTPWM, RIGHTFORWARD, RIGHTBACKWARD, LEFTFORWARD, LEFTBACKWARD);
DFRobotIRPosition myDFRobotIRPosition;

// Store X & Y position for both cameras
int positionXR, positionYR, positionXL, positionYL;

int shift = 0;
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

  pinMode(FRONTTRIG, OUTPUT);
  pinMode(FRONTECHO, INPUT);

  echo_init();
}
//
//void loop()
//{
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
//  Wire.requestFrom(0x9,1);
//  while(Wire.available())
//  {
//    rssi= Wire.read();
//  }
//  Serial.println("Arduino1");
//  Serial.println(rssi);
//}

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
  Serial.print("YR");
  Serial.println(positionYR);
  Serial.print("YL");
  Serial.println(positionYL);
  
//  perform_movement();
  delay(1000);
}


//void loop()
//{
////  while((front_sensor_side = check_front_sensors()) != NULL)
////  {
////    if(front_sensor_side == 'R')
////    {
////      M.Forward_Left(255);
////      delay(600);
////      M.Forward(255);
////      delay(1000);
////      M.Forward_Right(255);
////      delay(600);
////    }
////    else
////    {
////      M.Forward_Right(255);
////      delay(600);
////      M.Forward(255);
////      delay(1000);
////      M.Forward_Left(255);
////      delay(600);
////    }
////  }
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
//
////  perform_movement();
//
//  
//  Wire.requestFrom(0x9,1);
//  while(Wire.available())
//  {
//    rssi= Wire.read();
//  }
//  Serial.println("Arduino1");
//  Serial.println(rssi);
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
  if(positionXR == 1023 && positionXL == 1023)
  {
    lost_mode();
    M.STOP();
    delay(300);
  }
  else if(positionXR > 700)
  {
    M.Forward_Left(240);
  }
  else if(positionXL == 1023)
  {
    M.Forward_Right(240);
  }
  else
  {
    M.Forward(255);
  }
}

void lost_mode()
{
  int run_time = millis();
  int dir_time = millis();
  M.Forward_Right(255);
  delay(200);
  M.Forward(255);
  delay(300);
  while(echo_confidence(RIGHTTRIGGER, RIGHTECHO, 'R') > 30 && echo_confidence(FRONTRIGHTTRIG,  FRONTRIGHTECHO, 'R') > 50){}
  while(1)
  {
    // Right Camera
    set_pins(0);
    delay(25);
    get_camera_vals('R');

    // Left Camera
    set_pins(1);
    delay(25);
    get_camera_vals('L');
    
    if(positionXL != 1023 or positionXR != 1023)
      break;
    else
    { 
      M.Forward(255);
      delay(500);
      if(echo_confidence(FRONTRIGHTTRIG,  FRONTRIGHTECHO, 'R') < 50)
      {
        M.Forward_Left(255);
        delay(300);
      }
      else if(echo_confidence(RIGHTTRIGGER, RIGHTECHO, 'R') < 30)
      {
        M.Forward_Left(255);
        delay(150);
      }
      else if(echo_confidence(RIGHTTRIGGER, RIGHTECHO, 'R') > 40)
      {
        if(echo_confidence(FRONTRIGHTTRIG,  FRONTRIGHTECHO, 'R') < 50)
        {
          M.Forward_Left(255);
          delay(300);
        }
        else
        {
          M.Forward_Right(255);
          delay(150);
        }
      }
    }
  }
}
