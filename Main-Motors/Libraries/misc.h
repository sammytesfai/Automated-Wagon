#ifndef _MISC_H__
#define _MISC_H__

// Used by PWM & Motors modules
#define MAX_FREQ 100
#define MAX_DUTY_CYCLE 255
#define ACTIVE 1
#define INACTIVE 0
#define SUCCESS 1
#define ERROR 0

#define RIGHTPWM 5       // Set Speed of motor
#define RIGHTFORWARD 31  // Move Forward
#define RIGHTBACKWARD 32 // Move Backward
#define RIGHTMOTORENCODERA 7
#define RIGHTMOTORENCODERB 8

#define LEFTPWM 3      // Set Speed of motor
#define LEFTFORWARD 27  // Move Forward
#define LEFTBACKWARD 26 // Move Backward
#define LEFTMOTORENCODERB 35
#define LEFTMOTORENCODERA 38

// IR Camera Select Pins
#define CAMERA_SEL_A0 11
#define CAMERA_SEL_A1 12
#define CAMERA_SEL_A2 13
#define CAMERA_SEL_A3 39

// I/O pins for US sensors
#define LEFTTRIGGER 34
#define RIGHTTRIGGER 37
#define RADARTRIGGER 29
#define LEFTECHO 40
#define RIGHTECHO 41
#define RADARECHO 30

// I/O Servo Pin
#define SERVO 28

// Values to define distane from US sensors and confidence samples
#define CONFIDENCE_SAMPLE 5
#define DISTANCE 8
#define RADAR_FIELD_VIEW 90
#define RADAR_INCREMENT 3

void set_pins(int flag);
void printResult(int *positionX, int num);
bool check_sides(int trig, int echo, char side);
void echo_confidence(int trig, int echo, char side);
int echo_avg(char side);

#endif