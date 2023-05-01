#include <Arduino.h>
#include "misc.h"

static long duration;
static int distance;
static int left_echo [CONFIDENCE_SAMPLE];
static int right_echo [CONFIDENCE_SAMPLE];
static int front_left_echo [CONFIDENCE_SAMPLE];
static int front_right_echo [CONFIDENCE_SAMPLE];

// Selector Functions for Cameras
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

void printResult(int *positionX, int num)
{
  if(num == 1) Serial.print("Camera1: ");
  else Serial.print("Camera2: ");
  for (int i=0; i<4; i++) 
  {
    Serial.print(positionX[i]);
    Serial.print(",");
            
    Serial.print(positionX[i]);
    Serial.print(";");
  }
  Serial.println();
}

// Get new US value from sensor
int check_US(int trig, int echo, char side)
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  distance = duration*0.034/2;
  if(side == 'L') 
  {
    if(trig == FRONTLEFTTRIG)
      front_left_echo[0] = distance;
    else
      left_echo[0] = distance;
  }
  else if(side == 'R') 
  {
    if(trig == FRONTRIGHTTRIG)
      front_right_echo[0] = distance;
    else
      right_echo[0] = distance;
  }
  return distance;
}

// Retain state of previous US values and returnn avg
int echo_confidence(int trig, int echo, char side)
{
  for(int i = 1; i < CONFIDENCE_SAMPLE; i++)
  {
    if(side == 'L')
    {
      if(trig == FRONTLEFTTRIG)
        front_left_echo[CONFIDENCE_SAMPLE-i] = front_left_echo[CONFIDENCE_SAMPLE-i-1];
      else
        left_echo[CONFIDENCE_SAMPLE-i] = left_echo[CONFIDENCE_SAMPLE-i-1];
    }
    else if(side == 'R')
    {
      if(trig == FRONTRIGHTTRIG)
        front_right_echo[CONFIDENCE_SAMPLE-i] = front_right_echo[CONFIDENCE_SAMPLE-i-1];
      else
        right_echo[CONFIDENCE_SAMPLE-i] = right_echo[CONFIDENCE_SAMPLE-i-1];
    }
  }

  check_US(trig, echo, side);
  Serial.println(echo_avg(trig, side));
  return echo_avg(trig, side);
}

// Calcualtes avg of a specific US sensor
int echo_avg(int trig, char side)
{
  int total = 0;
  if(side == 'L')
  {
    if(trig == FRONTLEFTTRIG)
      for(int i = 0; i < CONFIDENCE_SAMPLE; i++) total += front_left_echo[i];
    else
      for(int i = 0; i < CONFIDENCE_SAMPLE; i++) total += left_echo[i];
  }
  else 
  {
    if(trig == FRONTRIGHTTRIG)
      for(int i = 0; i < CONFIDENCE_SAMPLE; i++) total += front_right_echo[i];
    else
      for(int i = 0; i < CONFIDENCE_SAMPLE; i++) total += right_echo[i];
  }
  return total/CONFIDENCE_SAMPLE;
}

// Get intial sample of robots surroundings
void echo_init()
{
  echo_confidence(LEFTTRIGGER, LEFTECHO, 'L');
  echo_confidence(RIGHTTRIGGER, RIGHTECHO, 'R');
  echo_confidence(LEFTTRIGGER, LEFTECHO, 'L');
  echo_confidence(RIGHTTRIGGER, RIGHTECHO, 'R');
  echo_confidence(LEFTTRIGGER, LEFTECHO, 'L');
  echo_confidence(RIGHTTRIGGER, RIGHTECHO, 'R');
  echo_confidence(LEFTTRIGGER, LEFTECHO, 'L');
  echo_confidence(RIGHTTRIGGER, RIGHTECHO, 'R');

  echo_confidence(FRONTLEFTTRIG, FRONTLEFTECHO, 'L');
  echo_confidence(FRONTRIGHTTRIG, FRONTRIGHTECHO, 'R');
  echo_confidence(FRONTLEFTTRIG, FRONTLEFTECHO, 'L');
  echo_confidence(FRONTRIGHTTRIG, FRONTRIGHTECHO, 'R');
  echo_confidence(FRONTLEFTTRIG, FRONTLEFTECHO, 'L');
  echo_confidence(FRONTRIGHTTRIG, FRONTRIGHTECHO, 'R');
  echo_confidence(FRONTLEFTTRIG, FRONTLEFTECHO, 'L');
  echo_confidence(FRONTRIGHTTRIG, FRONTRIGHTECHO, 'R');
}

// Checks to see if object is detected in front of Robot
bool check_front_sensors()
{
  if(echo_confidence(FRONTLEFTTRIG, FRONTLEFTECHO, 'L') < FRONT_DISTANCE)
    return true;
  if(echo_confidence(FRONTRIGHTTRIG, FRONTRIGHTECHO, 'R') < FRONT_DISTANCE)
    return true;
  return false;
}
