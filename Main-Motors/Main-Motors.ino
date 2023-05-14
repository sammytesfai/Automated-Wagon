#include <Wire.h>
#include "motors.h"
#include "DFRobotIRPosition.h"
#include "misc.h"

MOTORS M(RIGHTPWM, LEFTPWM, RIGHTFORWARD, RIGHTBACKWARD, LEFTFORWARD, LEFTBACKWARD);
DFRobotIRPosition myDFRobotIRPosition;

// Store X & Y position for both cameras
int positionXR, positionXL; 

int rssi = 0;
char front_sensor_side;
int run_time = 0;

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

//void loop()
//{
//  Serial.print("Right: ");
//  Serial.println(echo_confidence(FRONTRIGHTTRIG, FRONTRIGHTECHO, RIGHT));
//  Serial.print("Left: ");
//  Serial.println(echo_confidence(FRONTLEFTTRIG, FRONTLEFTECHO, LEFT));
//  delay(1000);
//}

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
  while(echo_confidence(LEFTTRIGGER, LEFTECHO, LEFT) < SIDE_DISTANCE)
  {
    M.Forward_Right(255);
    Serial.println("RIGHT");
    delay(150);
    M.Forward(255);
    Serial.println("Forward");
    delay(150);
  }

  while(echo_confidence(RIGHTTRIGGER, RIGHTECHO, RIGHT) < SIDE_DISTANCE)
  {
    M.Forward_Left(255);
    Serial.println("LEFT");
    delay(150);
    M.Forward(255);
    Serial.println("Forward");
    delay(150);
  }

  while(echo_confidence(FRONTTRIG, FRONTECHO, FRONT) < 30)
  {
    M.Backward(255);
  }
  
  // Right Camera
  set_pins(0);
  delay(25);
  get_camera_vals(RIGHT);

  // Left Camera
  set_pins(1);
  delay(25);
  get_camera_vals(LEFT);
  
  perform_movement();

  Wire.requestFrom(0x9,1);
  while(Wire.available())
  {
    rssi= Wire.read();
  }
  Serial.println("Arduino1");
  Serial.println(rssi);
//  delay(1000);
}

int get_camera_vals(char side)
{
  myDFRobotIRPosition.requestPosition();
  delay(25);
  if (myDFRobotIRPosition.available())
  {
    if(side == RIGHT)
    {
      positionXR = myDFRobotIRPosition.readX(0);
      printResult(positionXR, 1);
      return positionXR;
    }
    else
    {
      positionXL = myDFRobotIRPosition.readX(0);
      printResult(positionXL, 2);
      return positionXL;
    }
  }
  Serial.println("Device not available!");
  return 0;
}

void perform_movement()
{
  if(positionXR == 1023 && positionXL == 1023)
  {
    if(verify_lost())
      lost_mode();
    M.STOP();
    delay(300);
    if(run_time != 0 && (millis() - run_time) > 15000)
    {
      while(1){}
    }
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
  run_time = millis();
  M.STOP();
  delay(200);
  if(echo_confidence(FRONTRIGHTTRIG, FRONTRIGHTECHO, RIGHT) < echo_confidence(FRONTLEFTTRIG, FRONTLEFTECHO, LEFT))
  {
    M.Forward_Right(255);
    delay(200);
    M.Forward(255);
    delay(300);
    while(echo_confidence(RIGHTTRIGGER, RIGHTECHO, RIGHT) > 30 && echo_confidence(FRONTRIGHTTRIG,  FRONTRIGHTECHO, RIGHT) > 50 && (millis() - run_time) < WALLSEARCH){}
    while((millis() - run_time) < RUNTIME)
    {
      // Right Camera
      set_pins(0);
      delay(25);
      get_camera_vals(RIGHT);
  
      // Left Camera
      set_pins(1);
      delay(25);
      get_camera_vals(LEFT);
      
      if(positionXL != 1023 or positionXR != 1023)
      {
        run_time = 0;
        break;
      }
      else
      { 
        M.Forward(255);
        delay(500);
        if(echo_confidence(FRONTTRIG, FRONTECHO, FRONT) < 70)
        {
          M.Backward(255);
          delay(500);
          M.Forward_Left(255);
          delay(500);
        }
        else if(echo_confidence(FRONTRIGHTTRIG,  FRONTRIGHTECHO, RIGHT) < 50)
        {
          M.Forward_Left(255);
          delay(300);
        }
        else if(echo_confidence(RIGHTTRIGGER, RIGHTECHO, RIGHT) < 30)
        {
          M.Forward_Left(255);
          delay(150);
        }
        else if(echo_confidence(RIGHTTRIGGER, RIGHTECHO, RIGHT) > 40)
        {
          if(echo_confidence(FRONTRIGHTTRIG,  FRONTRIGHTECHO, RIGHT) < 50)
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
  else
  {
    M.Forward_Left(255);
    delay(200);
    M.Forward(255);
    delay(300);
    while(echo_confidence(LEFTTRIGGER, LEFTECHO, LEFT) > 30 && echo_confidence(FRONTLEFTTRIG,  FRONTLEFTECHO, LEFT) > 50 && (millis() - run_time) < WALLSEARCH){}
    while((millis() - run_time) < RUNTIME)
    {
      // Right Camera
      set_pins(0);
      delay(25);
      get_camera_vals(RIGHT);
  
      // Left Camera
      set_pins(1);
      delay(25);
      get_camera_vals(LEFT);
      
      if(positionXL != 1023 or positionXR != 1023)
      {
        run_time = 0;
        break;
      }
      else
      { 
        M.Forward(255);
        delay(500);
        if(echo_confidence(FRONTTRIG, FRONTECHO, FRONT) < 70)
        {
          M.Backward(255);
          delay(500);
          M.Forward_Right(255);
          delay(500);
        }
        else if(echo_confidence(FRONTLEFTTRIG,  FRONTLEFTECHO, LEFT) < 50)
        {
          M.Forward_Right(255);
          delay(300);
        }
        else if(echo_confidence(LEFTTRIGGER, LEFTECHO, LEFT) < 30)
        {
          M.Forward_Right(255);
          delay(150);
        }
        else if(echo_confidence(LEFTTRIGGER, LEFTECHO, LEFT) > 40)
        {
          if(echo_confidence(FRONTLEFTTRIG,  FRONTLEFTECHO, RIGHT) < 50)
          {
            M.Forward_Right(255);
            delay(300);
          }
          else
          {
            M.Forward_Left(255);
            delay(150);
          }
        }
      }
    }
  }
}

/*
 * Verify_Lost: Takes another 4 samples of the IR detector to verify whether
 * or not the user is truly lost
 * 
 * Returns: Bool, true if the user is lost false otherwise.
*/
bool verify_lost()
{
  int R_avg = 0, L_avg =0;
  for(int i = 0; i < 4; i++)
  {
    // Right Camera
    set_pins(0);
    delay(25);
    R_avg += get_camera_vals(RIGHT);

    // Left Camera
    set_pins(1);
    delay(25);
    L_avg += get_camera_vals(LEFT);
  }

  Serial.print("Bool: ");
  Serial.println((R_avg/4 == 1023) && (L_avg/4 == 1023));
  return (R_avg/4 == 1023) && (L_avg/4 == 1023);
  
}
