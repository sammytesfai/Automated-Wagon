#ifndef MOTORS_H
#define	MOTORS_H

#include "pwm_module.h"

class MOTORS
{
  private:
    PWM Right;
    PWM Left;
    unsigned int LEFT_FORWARD_PIN;
    unsigned int LEFT_BACKWARD_PIN;
    unsigned int RIGHT_FORWARD_PIN;
    unsigned int RIGHT_BACKWARD_PIN;
  public:
    MOTORS(unsigned int PWM_Right, unsigned int PWM_Left, unsigned int Right_F, unsigned int Right_B, unsigned int Left_F, unsigned int Left_B);
    void SetLeftMotorSpeed(unsigned int speed);
    void SetRightMotorSpeed(unsigned int speed);
    void Forward(unsigned int speed);
    void Backward(unsigned int speed);
    void Forward_Right(unsigned int speed);
    void Forward_Left(unsigned int speed);
    void Backward_Right(unsigned int speed);
    void Backward_Left(unsigned int speed);
};

#endif
