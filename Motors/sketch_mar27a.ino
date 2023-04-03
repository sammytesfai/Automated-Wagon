#define RIGHTPWM 5       // Set Speed of motor
#define RIGHTFORWARD 31  // Move Forward
#define RIGHTBACKWARD 32 // Move Backward
#define RIGHTMOTORENCODER1 33
#define RIGHTMOTORENCODER2 7

#define LEFTPWM 3      // Set Speed of motor
#define LEFTFORWARD 27  // Move Forward
#define LEFTBACKWARD 26 // Move Backward
#define LEFTMOTORENCODER1 28
#define LEFTMOTORENCODER2 29
#define LEFTBUMPER 34

#include "motors.h"

MOTORS M(RIGHTPWM, LEFTPWM, RIGHTFORWARD, RIGHTBACKWARD, LEFTFORWARD, LEFTBACKWARD);

void setup()
{
  Serial.begin(115200);
  // Right Motor
  pinMode(RIGHTMOTORENCODER1, INPUT);
  pinMode(RIGHTMOTORENCODER2, INPUT);

  // Left Motor
  pinMode(LEFTMOTORENCODER1, INPUT);
  pinMode(LEFTMOTORENCODER2, INPUT);
  pinMode(LEFTBUMPER, INPUT);
}

void loop() 
{
  Serial.println(digitalRead(LEFTBUMPER));
//  while(digitalRead(LEFTBUMPER) == 0)
//  {
//    M.Backward(255);
//  }
//  M.Forward(0);
//  Serial.print("BUMPER: ");
//  Serial.println(digitalRead(LEFTBUMPER));
//  M.Forward(255);
//  delay(1000);
//  M.Backward(200);
//  delay(1000);
//  M.Forward_Right(255);
//  delay(1000);
//  M.Forward_Left(255);
//  delay(1000);
}




