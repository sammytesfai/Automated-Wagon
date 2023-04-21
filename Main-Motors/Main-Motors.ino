#include <Servo.h>
#include "Libraries\motors.h"
#include "Libraries\DFRobotIRPosition.h"
#include "Libraries\misc.h"

MOTORS M(RIGHTPWM, LEFTPWM, RIGHTFORWARD, RIGHTBACKWARD, LEFTFORWARD, LEFTBACKWARD);
DFRobotIRPosition myDFRobotIRPosition;
Servo myServo;

int positionXR[4];     ///< Store the X position
int positionYR[4];     ///< Store the Y position

int positionXL[4];     ///< Store the X position
int positionYL[4];     ///< Store the Y position

int randomarray[] = {300, 600, 500, 900, 1023, 700, 100};

int degree = 0;
bool dir;

void setup()
{
  Serial.begin(115200);
  // Right Motor
  pinMode(RIGHTMOTORENCODERA, INPUT);
  pinMode(RIGHTMOTORENCODERB, INPUT);

  // Left Motor
  pinMode(LEFTMOTORENCODERB, INPUT);
  pinMode(LEFTMOTORENCODERA, INPUT);

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

  // Radar US sensor I/O pins
  pinMode(RADARTRIGGER, OUTPUT);
  pinMode(RADARECHO, INPUT);

  myServo.attach(SERVO);
}

//void loop()
//{
//  delay(50);
//  myDFRobotIRPosition.requestPosition();
//
//  if (myDFRobotIRPosition.available()) 
//  {
//    for (int i=0; i<4; i++) 
//    {
//      positionXL[i]=myDFRobotIRPosition.readX(i);
//      positionYL[i]=myDFRobotIRPosition.readY(i);
//    }
//
//    printResult(positionXL, 2);
//  }
//}

void loop()
{
  myServo.write(degree);
  
  while(check_sides(LEFTTRIGGER, LEFTECHO, 'L'))
  {
    echo_confidence(LEFTTRIGGER, LEFTECHO, 'L');
    if(echo_avg('L') < DISTANCE)
    {
      M.Forward_Left(255);
      Serial.println("LEFT");
      delay(100);
      M.Backward(255);
      Serial.println("Forward");
      delay(100);
    }
  }

  while(check_sides(RIGHTTRIGGER, RIGHTECHO, 'R'))
  {
    echo_confidence(RIGHTTRIGGER, RIGHTECHO, 'R');
    if(echo_avg('R') < DISTANCE)
    {
      M.Forward_Right(255);
      Serial.println("RIGHT");
      delay(100);
      M.Backward(255);
      Serial.println("Forward");
      delay(100);
    }
  }

  // Right Camera
  set_pins(0);
  
  myDFRobotIRPosition.requestPosition();
  if (myDFRobotIRPosition.available())
  {
    for (int i=0; i<4; i++)
    {
      positionXR[i]=myDFRobotIRPosition.readX(i);
      positionYR[i]=myDFRobotIRPosition.readY(i);
    }

//    printResult(positionXR, 1);
  }
  else
  {
    Serial.println("Device not available!");
  }


  // Left Camera
  set_pins(1);
  
  myDFRobotIRPosition.requestPosition();

  if (myDFRobotIRPosition.available()) 
  {
    for (int i=0; i<4; i++) 
    {
      positionXL[i]=myDFRobotIRPosition.readX(i);
      positionYL[i]=myDFRobotIRPosition.readY(i);
    }

//    printResult(positionXL, 2);
  }
  else
  {
    Serial.println("Device not available!");
  }

  if(positionXR[0] > 300 && positionXR[0] < 1000 && positionXL[0] > 300 && positionXL[0] < 1000)
  {
    M.Backward(255);
  }
  else if(positionXR[0] < 300)
  {
    M.Forward_Right(255);
  }
  else if(positionXL[0] < 300)
  {
    M.Forward_Left(255);
  }
  else
  {
    M.STOP();
  }

  if(degree == 0) dir = true;
  else if(degree == RADAR_FIELD_VIEW) dir = false;

  if(dir) degree += RADAR_INCREMENT;
  else degree -= RADAR_INCREMENT;
}

//int i = 0;
//// Check basic movement with 1 second delays
//void loop()
//{
//  Serial.println(i);
//  int pos = randomarray[i];
//  if(pos < 300 && pos > 0)
//  {
//    M.Forward_Left(255);
//  }
//  else if(pos > 700 && pos < 1000)
//  {
//    M.Forward_Right(255);
//  }
//  else if(pos >= 300 && pos <= 700)
//  {
//    M.Backward(255);
//  }
//  else
//  {
//    M.STOP();
//  }
//  delay(1000);
//  i = (i+1)%7;
//}




