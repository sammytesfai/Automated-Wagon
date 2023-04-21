#include <Arduino.h>
#include "misc.h"

static long duration;
static int distance;
static int left_echo [CONFIDENCE_SAMPLE];
static int right_echo [CONFIDENCE_SAMPLE];

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

bool check_sides(int trig, int echo, char side)
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  distance = duration*0.034/2;
  if(distance < DISTANCE)
  {
    if(side == 'L') left_echo[0] = distance;
    else right_echo[0] = distance;
    return true;
  }
  return false;
}

void echo_confidence(int trig, int echo, char side)
{
  for(int i  = 0; i < CONFIDENCE_SAMPLE-1; i++)
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
  if(side == 'L')for(int i = 0; i < CONFIDENCE_SAMPLE; i++) total += left_echo[i];
  else for(int i = 0; i < CONFIDENCE_SAMPLE; i++) total += right_echo[i];
  return total/5;
}