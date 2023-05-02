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

#define LEFTPWM 3      // Set Speed of motor
#define LEFTFORWARD 27  // Move Forward
#define LEFTBACKWARD 26 // Move Backward

// IR Camera Select Pins
#define CAMERA_SEL_A0 11
#define CAMERA_SEL_A1 12
#define CAMERA_SEL_A2 13
#define CAMERA_SEL_A3 39

// I/O pins for US sensors
#define RIGHTTRIGGER 37
#define RIGHTECHO 41
#define LEFTTRIGGER 34
#define LEFTECHO 40

#define FRONTLEFTTRIG 7
#define FRONTLEFTECHO 8
#define FRONTRIGHTTRIG 29
#define FRONTRIGHTECHO 28

// Values to define distane from US sensors and confidence samples
#define CONFIDENCE_SAMPLE 5
#define SIDE_DISTANCE 8
#define FRONT_DISTANCE 30


void set_pins(int flag);
void printResult(int *positionX, int num);
int check_US(int trig, int echo, char side);
int echo_confidence(int trig, int echo, char side);
int echo_avg(int trig, char side);
void echo_init();
char check_front_sensors();
#endif