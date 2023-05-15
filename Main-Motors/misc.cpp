#include <Arduino.h>
#include "misc.h"

static long duration;
static int distance;
static int left_echo [CONFIDENCE_SAMPLE];
static int right_echo [CONFIDENCE_SAMPLE];
static int front_left_echo [CONFIDENCE_SAMPLE];
static int front_right_echo [CONFIDENCE_SAMPLE];
static int front_echo [CONFIDENCE_SAMPLE];

/* 
 *  SET_PINS: set select pins for MUX to access either 
 *  Left or Right camera. 0 for Right camera, 1 for Left
 *  camera
*/
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

/*
 * PrintResult: Printing function to print out detected 
 * IR emitter values within each cameras axis.
*/
void printResult(int positionX, int num)
{
  if(num == 1) Serial.print("CameraR: ");
  else Serial.print("CameraL: ");
  Serial.print(positionX);
  Serial.println();
}

/*
 * Check_US: obtains new US value for the specified sensor.
 * Stores it in global array.
*/
int check_US(int trig, int echo, char side)
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  distance = duration*0.034/2;
  if(side == LEFT)
  {
    if(trig == FRONTLEFTTRIG)
      front_left_echo[0] = distance;
    else
      left_echo[0] = distance;
  }
  else if(side == RIGHT) 
  {
    if(trig == FRONTRIGHTTRIG)
      front_right_echo[0] = distance;
    else
      right_echo[0] = distance;
  }
  else if(side == FRONT)
    front_echo[0] = distance;
  return distance;
}

/*
 * Echo_Confidence: Takes averages of past and most current
 * US value to ensure there isnt too much fluxuation.
 * 
 * Returns: Returns avgerage of a specific US sensor
*/
int echo_confidence(int trig, int echo, char side)
{
  for(int i = 1; i < CONFIDENCE_SAMPLE; i++)
  {
    if(side == LEFT)
    {
      if(trig == FRONTLEFTTRIG)
        front_left_echo[CONFIDENCE_SAMPLE-i] = front_left_echo[CONFIDENCE_SAMPLE-i-1];
      else
        left_echo[CONFIDENCE_SAMPLE-i] = left_echo[CONFIDENCE_SAMPLE-i-1];
    }
    else if(side == RIGHT)
    {
      if(trig == FRONTRIGHTTRIG)
        front_right_echo[CONFIDENCE_SAMPLE-i] = front_right_echo[CONFIDENCE_SAMPLE-i-1];
      else
        right_echo[CONFIDENCE_SAMPLE-i] = right_echo[CONFIDENCE_SAMPLE-i-1];
    }
    else if(side == FRONT)
      front_echo[CONFIDENCE_SAMPLE-i] = front_echo[CONFIDENCE_SAMPLE-i-1];
  }

  check_US(trig, echo, side);
  return echo_avg(trig, side);
}

/*
 * Echo_Avg: Takes the average of a US sensors data.
 * 
 * Return: Average of US values stored globally.
*/
int echo_avg(int trig, char side)
{
  int total = 0;
  if(side == LEFT)
  {
    if(trig == FRONTLEFTTRIG)
      for(int i = 0; i < CONFIDENCE_SAMPLE; i++) total += front_left_echo[i];
    else
      for(int i = 0; i < CONFIDENCE_SAMPLE; i++) total += left_echo[i];
  }
  else if(side == RIGHT)
  {
    if(trig == FRONTRIGHTTRIG)
      for(int i = 0; i < CONFIDENCE_SAMPLE; i++) total += front_right_echo[i];
    else
      for(int i = 0; i < CONFIDENCE_SAMPLE; i++) total += right_echo[i];
  }
  else if(side == FRONT)
    for(int i = 0; i < CONFIDENCE_SAMPLE; i++) total += front_echo[i];
  return total/CONFIDENCE_SAMPLE;
}

/*
 * Echo_Init: Initializes all US sensors on robot to ensure
 * when robot is intially booted up averages produce most
 * accurate values.
*/
void echo_init()
{
  echo_confidence(LEFTTRIGGER, LEFTECHO, LEFT);
  echo_confidence(RIGHTTRIGGER, RIGHTECHO, RIGHT);
  echo_confidence(LEFTTRIGGER, LEFTECHO, LEFT);
  echo_confidence(RIGHTTRIGGER, RIGHTECHO, RIGHT);
  echo_confidence(LEFTTRIGGER, LEFTECHO, LEFT);
  echo_confidence(RIGHTTRIGGER, RIGHTECHO, RIGHT);
  echo_confidence(LEFTTRIGGER, LEFTECHO, LEFT);
  echo_confidence(RIGHTTRIGGER, RIGHTECHO, RIGHT);

  echo_confidence(FRONTTRIG, FRONTECHO, FRONT);
  echo_confidence(FRONTTRIG, FRONTECHO, FRONT);
  echo_confidence(FRONTTRIG, FRONTECHO, FRONT);
  echo_confidence(FRONTTRIG, FRONTECHO, FRONT);

  echo_confidence(FRONTLEFTTRIG, FRONTLEFTECHO, LEFT);
  echo_confidence(FRONTRIGHTTRIG, FRONTRIGHTECHO, RIGHT);
  echo_confidence(FRONTLEFTTRIG, FRONTLEFTECHO, LEFT);
  echo_confidence(FRONTRIGHTTRIG, FRONTRIGHTECHO, RIGHT);
  echo_confidence(FRONTLEFTTRIG, FRONTLEFTECHO, LEFT);
  echo_confidence(FRONTRIGHTTRIG, FRONTRIGHTECHO, RIGHT);
  echo_confidence(FRONTLEFTTRIG, FRONTLEFTECHO, LEFT);
  echo_confidence(FRONTRIGHTTRIG, FRONTRIGHTECHO, RIGHT);
}
