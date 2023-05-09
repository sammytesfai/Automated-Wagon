#include <Wire.h>
#include "motors.h"
#include "DFRobotIRPosition.h"
#include "misc.h"

MOTORS M(RIGHTPWM, LEFTPWM, RIGHTFORWARD, RIGHTBACKWARD, LEFTFORWARD, LEFTBACKWARD);
DFRobotIRPosition myDFRobotIRPosition;

// Store X & Y position for both cameras
int positionXR, positionXL;
int oldpositionXR, oldpositionXL;


int rssi_distance1 = 0, rssi_distance2 = 0, rssi_distance3 = 0;
bool left_camera, right_camera;
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
  
  // Front Side US sensor I/O pins
  pinMode(FRONTLEFTTRIG, OUTPUT);
  pinMode(FRONTLEFTECHO, INPUT);
  pinMode(FRONTRIGHTTRIG, OUTPUT);
  pinMode(FRONTRIGHTECHO, INPUT);

  // Front US sensor I/O pins
  pinMode(FRONTTRIG, OUTPUT);
  pinMode(FRONTECHO, INPUT);

  echo_sample_refresh();
}

//void loop()
//{
//  Wire.requestFrom(0x92,1);
//  while(Wire.available())
//  {
//    Serial.println("Available");
//    rssi_distance1 = Wire.read();
//  }
//  Serial.print("Arduino1: ");
//  Serial.println(rssi_distance1);
//}

void loop()
{
 /*
  * Front Object Avoidance:
  * Checks if objects are detected using two front sensors,
  * if object is detect then performs static movement to move around 
  * obstruction
 */ 
 while((front_sensor_side = check_front_sensors()) != NULL)
 {
   Serial.println(echo_avg(FRONTTRIG, 'F'));
   if(front_sensor_side == 'R')
   {
     avoid_right(500, 300);
   }
   else if(front_sensor_side == 'L')
   {
     avoid_left(500, 300);
   }
   else
   {
     if(echo_avg(LEFTTRIGGER, 'L') > echo_avg(RIGHTTRIGGER, 'R') && echo_avg(LEFTTRIGGER, 'L') > FRONT_DISTANCE*2)
     {
       avoid_left(1000, 600);
     }
     else if(echo_avg(RIGHTTRIGGER, 'L') > echo_avg(LEFTTRIGGER, 'R') && echo_avg(RIGHTTRIGGER, 'L') > FRONT_DISTANCE*2)
     {
       avoid_right(1000, 600);
     }
     // Lost Mode as value of camera sensor will output 1023 signifying lost emitter
     else
       M.STOP();
   }
 }
 
 /*
  * Side Sensor Avoidance:
  * Two sensors located on both sides of the robot to detect objects
  * Upon reaching the threshold of SIDE_DISTANCE performs static movement.
 */

 // Left Side Sensor
 while(echo_confidence(LEFTTRIGGER, LEFTECHO, 'L') < SIDE_DISTANCE)
 {
   M.Forward_Left(255);
   Serial.println("LEFT");
   delay(150);
   M.Backward(255);
   Serial.println("Forward");
   delay(150);
   M.STOP();
 }

 // Right Side Sensor
 while(echo_confidence(RIGHTTRIGGER, RIGHTECHO, 'R') < SIDE_DISTANCE)
 {
   M.Forward_Right(255);
   Serial.println("RIGHT");
   delay(150);
   M.Backward(255);
   Serial.println("Forward");
   delay(150);
   M.STOP();
 }

  /*
  * Turkey Lab State:
  * Get input from IR Camera Sensors and perform tracking of users
  * IR emitter
 */
 // Right Camera
 set_pins(0);
// delay(25);
 get_camera_vals('R');

 // Left Camera
 set_pins(1);
// delay(25);
 get_camera_vals('L');

 // Perform tracking of user based on current position of IR emitter relative to
 // both cameras X-axis, if perform_movement() returns false then goes into lost mode
 if(!perform_movement())
 {
  M.STOP();
//  lost_mode();
 }
 
 Wire.requestFrom(0x9,1);
 while(Wire.available())
 {
   rssi_distance1 = Wire.read();
 }
  Serial.print("Arduino1: ");
  Serial.println(rssi_distance1);

 Wire.requestFrom(0x91,1);
 while(Wire.available())
 {
   rssi_distance2 = Wire.read();
 }
  Serial.print("Arduino2: ");
  Serial.println(rssi_distance2);

 Wire.requestFrom(0x92,1);
 while(Wire.available())
 {
   rssi_distance3 = Wire.read();
 }
  Serial.print("Arduino3: ");
  Serial.println(rssi_distance3);
}

bool get_camera_vals(char side)
{
 myDFRobotIRPosition.requestPosition();
 delay(25);
 if (myDFRobotIRPosition.available())
 {
   if(side == 'R')
   {
      positionXR = myDFRobotIRPosition.readX(0);
      printResult(positionXR, 1);
   }
   else
   {
      positionXL = myDFRobotIRPosition.readX(0);
      printResult(positionXL, 2);
   }
    Serial.println();
   return true;
 }
 Serial.println("Device not available!");
 return false;
}

/*
 * Performs movement based on current x-axis position
 * Returns true if movement can be done and emitter can be seen
 * Returns false if emitter can not be found
*/
bool perform_movement()
{
  if(positionXR > 300 && positionXR < 1000 && positionXL > 300 && positionXL < 1000)
  {
    Serial.println("User Acquired");
    M.Backward(255);
  }
  else if(positionXR < 300)
  {
    Serial.println("User Acquired");
    M.Forward_Right(255);
  }
  else if(positionXL < 300)
  {
    Serial.println("User Acquired");
    M.Forward_Left(255);
  }
  else
  {
    Serial.println("User Lost");
    oldpositionXR = positionXR;
    oldpositionXL = positionXL;
    return false;
  }
  return true;
}

void avoid_left(int forward_duration, int turn_duration)
{
  M.Forward_Left(255);
  delay(turn_duration);
  M.Backward(255);
  delay(forward_duration);
  M.Forward_Right(255);
  delay(turn_duration);
  M.STOP();
}

void avoid_right(int forward_duration, int turn_duration)
{
  M.Forward_Right(255);
  delay(turn_duration);
  M.Backward(255);
  delay(forward_duration);
  M.Forward_Left(255);
  delay(turn_duration);
  M.STOP();
}

void lost_mode()
{
  int run_time = millis();
  int dir_time = millis();
  while((millis() - run_time) < 15000)
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
      if((millis() - dir_time) >= 5000)
        dir_time = millis();

      if((millis() - dir_time) < 2000)
        M.Backward(200);
      else if((millis() - dir_time) < 3000)
        M.Forward_Right(200);
      else if((millis() - dir_time) < 5000)
        M.Forward_Left(200);

      while((front_sensor_side = check_front_sensors()) != NULL)
      {
        if(front_sensor_side == 'R')
        {
          avoid_right(500, 300);
          dir_time += 800;
        }
        else if(front_sensor_side == 'L')
        {
          avoid_left(500, 300);
          dir_time += 800;
        }
        else
        {
          if(echo_avg(LEFTTRIGGER, 'L') > echo_avg(RIGHTTRIGGER, 'R') && echo_avg(LEFTTRIGGER, 'L') > FRONT_DISTANCE*2)
          {
            avoid_left(1000, 600);
            dir_time += 1600;
          }
          else if(echo_avg(RIGHTTRIGGER, 'L') > echo_avg(LEFTTRIGGER, 'R') && echo_avg(RIGHTTRIGGER, 'L') > FRONT_DISTANCE*2)
          {
            avoid_right(1000, 600);
            dir_time += 1600;
          }
          else
            M.STOP();
        }
      }

      // Left Side Sensor
      while(echo_confidence(LEFTTRIGGER, LEFTECHO, 'L') < SIDE_DISTANCE)
      {
        M.Forward_Left(255);
        Serial.println("LEFT");
        delay(150);
        M.Backward(255);
        Serial.println("Forward");
        delay(150);
        M.STOP();
        dir_time += 300;
      }

      // Right Side Sensor
      while(echo_confidence(RIGHTTRIGGER, RIGHTECHO, 'R') < SIDE_DISTANCE)
      {
        M.Forward_Right(255);
        Serial.println("RIGHT");
        delay(150);
        M.Backward(255);
        Serial.println("Forward");
        delay(150);
        M.STOP();
        dir_time += 300;
      }
    }
  }
}
