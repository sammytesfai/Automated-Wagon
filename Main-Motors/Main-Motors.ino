#include <Wire.h>
#include "motors.h"
#include "DFRobotIRPosition.h"
#include "misc.h"

MOTORS M(RIGHTPWM, LEFTPWM, RIGHTFORWARD, RIGHTBACKWARD, LEFTFORWARD, LEFTBACKWARD);
DFRobotIRPosition myDFRobotIRPosition;

// Store X & Y position for both cameras
int positionXR, positionXL;
int rssi = 0;
int run_time = 0;
int state = 0;

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
  run_time = millis();
}

void loop()
{
  Serial.print("STATE: ");
  Serial.println(state);
  switch(state)
  {
    case SEARCH:
      // Transition to SOS state if can not detect within timer
      // If can detect user transition to TURKEY state
      if(!check_cameras())
      {
        if((millis() - run_time) > 15000)
        {
          M.STOP();
          state = SOS;
        }
        else
          M.Forward_Right(250);
      }
      else
        state = TURKEY;
      break;
    case TURKEY:
      // Avoid objects on left side of robot
      while(echo_confidence(LEFTTRIGGER, LEFTECHO, LEFT) < SIDE_DISTANCE)
      {
        M.Forward_Right(255);
        Serial.println("RIGHT");
        delay(150);
        M.Forward(255);
        Serial.println("Forward");
        delay(150);
      }
    
      // Avoid objects on right side of robot
      while(echo_confidence(RIGHTTRIGGER, RIGHTECHO, RIGHT) < SIDE_DISTANCE)
      {
        M.Forward_Left(255);
        Serial.println("LEFT");
        delay(150);
        M.Forward(255);
        Serial.println("Forward");
        delay(150);
      }
    
      // Backup until object in front of robot is far enough away
      while(echo_confidence(FRONTTRIG, FRONTECHO, FRONT) < 30)
      {
        M.Backward(255);
      }

      // Obtain RSSI value from arduino 33 ble and use it to determine distance from user
      Wire.requestFrom(0x9,1);
      while(Wire.available())
      {
        rssi= Wire.read();
      }
    
      // Obtain camera detector values and perform movement based on provided input
      set_pins(0);
      delay(25);
      get_camera_vals(RIGHT);
    
      set_pins(1);
      delay(25);
      get_camera_vals(LEFT);
      
      perform_movement();

      // Transition to OUT_OF_RANGE state if too far from user
      if(state == TURKEY && rssi ==1)
        state = OUT_OF_RANGE;
      break;
    case LOST:
      lost_mode();
      M.STOP();
      delay(300);

      // Transition to SEARCH state if timer runs out
      if(run_time != 0 && (millis() - run_time) > 15000)
      {
        run_time = millis();
        state = SEARCH;
      }
      break;
    case OUT_OF_RANGE:
      M.Forward(255);
      // Obtain RSSI value from arduino 33 ble and use it to determine distance from user
      Wire.requestFrom(0x9,1);
      while(Wire.available())
      {
        rssi= Wire.read();
      }

      // Transition to TURKEY state if within range and detect emitter
      // If can not detect emitter transition to LOST state
      if(rssi == 0 && check_cameras())
        state = TURKEY;
      else if(!check_cameras())
        state = LOST;
    case SOS:
      // Inifinite Loop needs to be rebooted to operate
      while(1){}
      break;
    default:
      break;
  }
}

/*
 * Get_Camera_Vals: Helper function to obtain Camera IR values for either the
 * Right or Left camera. Stores these values globally.
 * 
 * Return: Returns the last recorded value from the cameras
*/ 
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

/*
 * Perform_Movement: Uses IR detector values to determine where user is
 * and performs movements accordingly. If user can not be detected robot
 * will go into lost mode.
*/
void perform_movement()
{
  if(positionXR == 1023 && positionXL == 1023)
  {
    if(verify_lost())
      state = LOST;
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

/*
 * Lost_Mode: Uses wall hugging algorithm to identify closest wall and use it as
 * a life line to follow in the direction of where the user was last seen. Lost
 * will continue searching for the user until they are found or until the timer
 * expires.
*/
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
    while(echo_confidence(RIGHTTRIGGER, RIGHTECHO, RIGHT) > 30 && echo_confidence(FRONTRIGHTTRIG,  FRONTRIGHTECHO, RIGHT) > 50 && (millis() - run_time) < WALLSEARCH)
    {
      if(check_cameras())
      {
        state = TURKEY;
        break;
      }
    }
    while((millis() - run_time) < RUNTIME)
    {
      if(check_cameras())
      {
        run_time = 0;
        state = TURKEY;
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
    while(echo_confidence(LEFTTRIGGER, LEFTECHO, LEFT) > 30 && echo_confidence(FRONTLEFTTRIG,  FRONTLEFTECHO, LEFT) > 50 && (millis() - run_time) < WALLSEARCH)
    {
      if(check_cameras())
      {
        state = TURKEY;
        break;
      }
    }
    
    while((millis() - run_time) < RUNTIME)
    { 
      if(check_cameras())
      {
        run_time = 0;
        state = TURKEY;
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

/*
 * Check_Cameras: Helper function to check if cameras can detect a
 * users IR emitter
 * 
 * Returns: True if it can be detected, otherwise false
*/
bool check_cameras()
{
  // Right Camera
  set_pins(0);
  delay(25);
  get_camera_vals(RIGHT);
  
  // Left Camera
  set_pins(1);
  delay(25);
  get_camera_vals(LEFT);

  return positionXL != 1023 || positionXR != 1023;
}
