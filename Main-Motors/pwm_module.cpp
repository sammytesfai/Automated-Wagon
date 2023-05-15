#include <Arduino.h>
#include "pwm_module.h"
#include "misc.h"

/*
 * PWM Constructor: Initializes values and sets pin mode of pwm.
*/
PWM::PWM(unsigned int pin)
{
  Pin = pin;
  PWM_Duty_Cycle = 0;
  PWM_Active = ACTIVE;
  pinMode(Pin, OUTPUT);
}

/*
 * PWM_SetDutyCycle: Sets duty cycle of pwm if it has not already been done.
*/
void PWM::PWM_SetDutyCycle(unsigned int Duty)
{
  if (Duty == MAX_DUTY_CYCLE) PWM_Duty_Cycle = MAX_DUTY_CYCLE;
  else PWM_Duty_Cycle = Duty % MAX_DUTY_CYCLE;
  analogWrite(Pin, PWM_Duty_Cycle);
}

/*
 * PWM_GetDutyCycle: Returns current duty cycle being used in PWM.
*/
unsigned int PWM::PWM_GetDutyCycle()
{
  return PWM_Duty_Cycle;
}
