                                                                                                                                                                                                                                                                                                              #define RIGHTPWM 5       // Set Speed of motor
#define RIGHTFORWARD 31  // Move Forward
#define RIGHTBACKWARD 32 // Move Backward
#define RIGHTMOTORENCODER1 7
#define RIGHTMOTORENCODER2 8
#define RIGHTSENSOR 13

#define LEFTPWM 3      // Set Speed of motor
#define LEFTFORWARD 27  // Move Forward
#define LEFTBACKWARD 26 // Move Backward
#define LEFTMOTORENCODER1 35
#define LEFTMOTORENCODER2 38
#define LEFTBUMPER 33
#define LEFTSENSOR 12

#include <Wire.h>
#include "Libraries\motors.h"
#include "Libraries\DFRobotIRPosition.h"

MOTORS M(RIGHTPWM, LEFTPWM, RIGHTFORWARD, RIGHTBACKWARD, LEFTFORWARD, LEFTBACKWARD);
DFRobotIRPosition myDFRobotIRPosition;

void printResult();

int positionX[4];     ///< Store the X position
int positionY[4];     ///< Store the Y position


void setup()
{
  Serial.begin(115200);
  // Right Motor
  pinMode(RIGHTMOTORENCODER1, INPUT);
  pinMode(RIGHTMOTORENCODER2, INPUT);
  pinMode(RIGHTSENSOR, INPUT);

  // Left Motor
  pinMode(LEFTMOTORENCODER1, INPUT);
  pinMode(LEFTMOTORENCODER2, INPUT);
  pinMode(LEFTBUMPER, INPUT);
  pinMode(LEFTSENSOR, INPUT);

  myDFRobotIRPosition.begin();
}

// Check Camera IR Sensor
void loop()
{
  myDFRobotIRPosition.requestPosition();

  if (myDFRobotIRPosition.available()) {
    for (int i=0; i<4; i++) {
      positionX[i]=myDFRobotIRPosition.readX(i);
      positionY[i]=myDFRobotIRPosition.readY(i);
    }

    printResult();
  }
  else{
    Serial.println("Device not available!");
  }
  
  delay(50);
}

// Simulate Turkey Lab with serial inputs
//void loop()
//{
//  if(digitalRead(LEFTSENSOR) && !digitalRead(RIGHTSENSOR))
//  {
//    M.Forward_Left(255);
//  }
//  else if(!digitalRead(LEFTSENSOR) && digitalRead(RIGHTSENSOR))
//  {
//    M.Forward_Right(255);
//  }
//  else if(!digitalRead(LEFTSENSOR) && !digitalRead(RIGHTSENSOR))
//  {
//    M.Backward(255);
//  }
//  else
//  {
//    M.STOP();
//  }
//}

// Check bumpers
//void loop() 
//{
//  Serial.println("A");
//  Serial.println(digitalRead(LEFTBUMPER));
//  Serial.println("B");
//  while(digitalRead(LEFTBUMPER))
//  {
//    Serial.println(digitalRead(LEFTBUMPER));
//    M.Backward(255);
//  }
//  Serial.println("C");
//  M.Forward(0);
//}

// Check basic movement with 1 second delays
//void loop()
//{
//  M.Forward(255);
//  delay(1000);
//  M.Backward(200);
//  delay(1000);
//  M.Forward_Right(255);
//  delay(1000);
//  M.Forward_Left(255);
//  delay(1000);
//}

void printResult()
{
  for (int i=0; i<4; i++) {
    Serial.print(positionX[i]);
    Serial.print(",");
    
    Serial.print(positionY[i]);
    Serial.print(";");
  }
  Serial.println();
}




