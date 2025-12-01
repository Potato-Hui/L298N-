#ifndef __STEPMOTOR_H
#define	__STEPMOTOR_H
#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"


void Motor_GPIO_Init(void);
void StepMotor(int8_t direction);
void RotateToAngle(float angle);
static void Motor_Step(int8_t direction);
void Motor_Rotate(float angle, int dir);
void Motor_RotateTo(float targetAngle);


#endif



