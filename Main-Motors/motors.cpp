#include <Arduino.h>
#include "motors.h"

MOTORS::MOTORS(unsigned int PWM_Right, unsigned int PWM_Left, unsigned int Right_F, unsigned int Right_B, unsigned int Left_F, unsigned int Left_B): Right(PWM_Right), Left(PWM_Left)
{
  RIGHT_FORWARD_PIN = Right_F;
  RIGHT_BACKWARD_PIN = Right_B;
  LEFT_FORWARD_PIN = Left_F;
  LEFT_BACKWARD_PIN = Left_B;
  pinMode(RIGHT_FORWARD_PIN, OUTPUT);
  pinMode(RIGHT_BACKWARD_PIN, OUTPUT);
  pinMode(LEFT_FORWARD_PIN, OUTPUT);
  pinMode(LEFT_BACKWARD_PIN, OUTPUT);
  digitalWrite(RIGHT_FORWARD_PIN, LOW);
  digitalWrite(RIGHT_BACKWARD_PIN, LOW);
  digitalWrite(LEFT_FORWARD_PIN, LOW);
  digitalWrite(LEFT_BACKWARD_PIN, LOW);
  SetLeftMotorSpeed(0);
  SetRightMotorSpeed(0);
}

void MOTORS::SetLeftMotorSpeed(unsigned int speed)
{
  if(speed == Left.PWM_GetDutyCycle()) return;
  Left.PWM_SetDutyCycle(speed);
}

void MOTORS::SetRightMotorSpeed(unsigned int speed)
{
  if(speed == Right.PWM_GetDutyCycle()) return;
  Right.PWM_SetDutyCycle(speed);
}

void MOTORS::Forward(unsigned int speed)
{
  digitalWrite(LEFT_BACKWARD_PIN, LOW);
  digitalWrite(LEFT_FORWARD_PIN, HIGH);
  digitalWrite(RIGHT_BACKWARD_PIN, HIGH);
  digitalWrite(RIGHT_FORWARD_PIN, LOW);
  
  SetLeftMotorSpeed(speed);
  SetRightMotorSpeed(speed);
}

void MOTORS::Backward(unsigned int speed)
{
  digitalWrite(LEFT_BACKWARD_PIN, HIGH);
  digitalWrite(LEFT_FORWARD_PIN, LOW);
  digitalWrite(RIGHT_BACKWARD_PIN, LOW);
  digitalWrite(RIGHT_FORWARD_PIN, HIGH);
  
  SetLeftMotorSpeed(speed);
  SetRightMotorSpeed(speed);
}

void MOTORS::Forward_Right(unsigned int speed)
{
  digitalWrite(LEFT_BACKWARD_PIN, LOW);
  digitalWrite(LEFT_FORWARD_PIN, HIGH);
  digitalWrite(RIGHT_BACKWARD_PIN, LOW);
  digitalWrite(RIGHT_FORWARD_PIN, HIGH);

  SetLeftMotorSpeed(speed);
  SetRightMotorSpeed(speed);
}

void MOTORS::Forward_Left(unsigned int speed)
{
  digitalWrite(LEFT_BACKWARD_PIN, HIGH);
  digitalWrite(LEFT_FORWARD_PIN, LOW);
  digitalWrite(RIGHT_BACKWARD_PIN, HIGH);
  digitalWrite(RIGHT_FORWARD_PIN, LOW);

  SetLeftMotorSpeed(speed);
  SetRightMotorSpeed(speed);
}

void MOTORS::Backward_Right(unsigned int speed)
{
  digitalWrite(LEFT_BACKWARD_PIN, HIGH);
  digitalWrite(LEFT_FORWARD_PIN, LOW);
  digitalWrite(RIGHT_BACKWARD_PIN, LOW);
  digitalWrite(RIGHT_FORWARD_PIN, HIGH);

  SetLeftMotorSpeed(speed);
  SetRightMotorSpeed(speed);
}

void MOTORS::Backward_Left(unsigned int speed)
{
  digitalWrite(LEFT_BACKWARD_PIN, LOW);
  digitalWrite(LEFT_FORWARD_PIN, HIGH);
  digitalWrite(RIGHT_BACKWARD_PIN, HIGH);
  digitalWrite(RIGHT_FORWARD_PIN, LOW);

  SetLeftMotorSpeed(speed);
  SetRightMotorSpeed(speed);
}

void MOTORS::STOP()
{
  digitalWrite(LEFT_BACKWARD_PIN, LOW);
  digitalWrite(LEFT_FORWARD_PIN, LOW);
  digitalWrite(RIGHT_BACKWARD_PIN, LOW);
  digitalWrite(RIGHT_FORWARD_PIN, LOW);

  SetLeftMotorSpeed(255);
  SetRightMotorSpeed(255);
}
