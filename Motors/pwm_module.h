#ifndef _PWM_MODULE_H__
#define _PWM_MODULE_H__


class PWM
{
  private:
    unsigned int Pin;
    unsigned int PWM_Active;
    unsigned int PWM_Duty_Cycle;
  public:
    PWM(unsigned int pin);
    void PWM_SetDutyCycle(unsigned int Duty);
    unsigned int PWM_GetDutyCycle();
};


#endif


