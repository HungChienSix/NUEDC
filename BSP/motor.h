/*
	PA0 Encode_LB TIM2_Encode模式
	PA1 Encode_LA TIM2_Encode模式

	PA8 Encode_RB TIM1_Encode模式
	PA9 Encode_RA TIM1_Encode模式

	PA7 RPWM TIM3CH2_PWM模式
	PB0 LPWM TIM3CH3_PWM模式

	PB8 LIN1 接L298N控制引脚左一
	PB7 LIN2 接L298N控制引脚左二
	PA3 RIN1 接L298N控制引脚右一
	PA4 RIN2 接L298N控制引脚右二

	电机LM+接L298N的OUT2
	电机LM-接L298N的OUT1
	电机RM+接L298N的OUT4
	电机RM-接L298N的OUT3

	PA3 RIN1 GPIO_OUTPUT 
	PA4 RIN2 GPIO_OUTPUT
	PB8 LIN1 GPIO_OUTPUT
	PB7 LIN2 GPIO_OUTPUT

	使用UART6串口调试
*/

#ifndef MOTOR_H
#define MOTOR_H

#include "main.h"

struct MG310_State //电机的状态
{
	int16_t output;				
	float speed_exp;
	float speed_act; 
	float location_exp;
	float location_act; 
};

struct MG310_PID //PID的参数
{
	float P ;
	float I ;
	float D ;
};
 
struct MG310_PID_ERROR //PID的误差
{
	float last;
	float now ;
	float all ;
};

void pid_measure(void);
void uart_printf(const char *format,...);
	
#endif