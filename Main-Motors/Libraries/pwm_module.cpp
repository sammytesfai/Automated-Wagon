#include <Arduino.h>
#include "pwm_module.h"
#include "misc.h"

PWM::PWM(unsigned int pin)
{
  Pin = pin;
  PWM_Duty_Cycle = 0;
  PWM_Active = ACTIVE;
  pinMode(Pin, OUTPUT);
}

void PWM::PWM_SetDutyCycle(unsigned int Duty)
{
  if (Duty == MAX_DUTY_CYCLE) PWM_Duty_Cycle = MAX_DUTY_CYCLE;
  else PWM_Duty_Cycle = Duty % MAX_DUTY_CYCLE;
  analogWrite(Pin, PWM_Duty_Cycle);
}

unsigned int PWM::PWM_GetDutyCycle()
{
  return PWM_Duty_Cycle;
}
