#include "motor.h"
#include "stdio.h"
#include "stdarg.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim1;
extern UART_HandleTypeDef huart6;

int16_t encode_L=0;			
int16_t encode_R=0;	

struct MG310_State State_L = {0,0.0,0.0,0,0};
struct MG310_State State_R = {0,0.0,0.0,0,0};

struct MG310_PID pid_L_v = {0.5,0.1,0.1};
struct MG310_PID pid_R_v = {0.5,0.1,0.1};
struct MG310_PID pid_L_l = {20,0.0,0.0};
struct MG310_PID pid_R_l = {20,0.0,0.0};

struct MG310_PID_ERROR error_L_v = {0,0,0};
struct MG310_PID_ERROR error_R_v = {0,0,0};
struct MG310_PID_ERROR error_L_l = {0,0,0};
struct MG310_PID_ERROR error_R_l = {0,0,0};

void MOTOR_Forward(int16_t Loutput,int16_t Routput)
{
		if(Loutput >= 0)
		{
			HAL_GPIO_WritePin(LIN1_GPIO_Port, LIN1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LIN2_GPIO_Port, LIN2_Pin, GPIO_PIN_SET);
			if(State_L.output > 256-1)
			{
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 256-1);
			}
			else 
			{
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, State_L.output);
			}
		}
		else if(Loutput < 0)
		{
			HAL_GPIO_WritePin(LIN1_GPIO_Port, LIN1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(LIN2_GPIO_Port, LIN2_Pin, GPIO_PIN_RESET);
			if(State_L.output < -256+1)
			{
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 256-1);
			}
			else 
			{
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, Loutput);
			}
		}
		
		if(Routput >= 0)
		{
			HAL_GPIO_WritePin(RIN1_GPIO_Port, RIN1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(RIN2_GPIO_Port, RIN2_Pin, GPIO_PIN_SET);
			if(Routput > 256-1)
			{
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 256-1);
			}
			else 
			{
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, Routput);
			}
		}
		else if(Routput < 0)
		{
			HAL_GPIO_WritePin(RIN1_GPIO_Port, RIN1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(RIN2_GPIO_Port, RIN2_Pin, GPIO_PIN_RESET);
			if(Routput < -256+1)
			{
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 256-1);
			}
			else 
			{
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, -Routput);
			}
		}
	 	uart_printf(",A=%.2f,B=%.2f,C=%.1f,D=%.1f,", State_L.location_act, State_L.location_exp,State_L.speed_act,State_L.speed_exp);
		
}
// 左轮满载330转速 右轮满载305转速
void pid_measure(void)
{
	// 编码为正时向前,为负时向后
	// 处理左轮编码值
	encode_L = __HAL_TIM_GetCounter(&htim2); 
	__HAL_TIM_SetCounter(&htim2, 0);
	// 处理右轮编码值
	encode_R = __HAL_TIM_GetCounter(&htim1); 
	__HAL_TIM_SetCounter(&htim1, 0);
	encode_R = -encode_R;
	
	// 计算两轮子行走路程, 圈数,(1060编码值视为走过22cm),
	State_L.location_act = encode_L/1060.0 + State_L.location_act ;
	State_R.location_act = encode_R/1060.0 + State_L.location_act ;
	// uart_printf("A=%lf,B=%lf\n", State_L.speed_act, State_R.speed_act);
	
	// 记录两轮子路程的误差值
	error_L_l.last = error_L_l.now;
	error_L_l.now  = State_L.location_exp-State_L.location_act;
	error_L_l.all += error_L_l.now;
	
	error_R_l.last = error_R_l.now;
	error_R_l.now  = State_R.location_exp-State_R.location_act;
	error_R_l.all += error_R_l.now;
	
	// 这个要结合实际去设置
	if(error_L_l.all >= 5)
	{
		error_L_l.all = 5;
	}
	if(error_R_l.all >= 5)
	{
		error_R_l.all = 5;
	}
	
	//State_L.speed_exp = pid_L_l.P*error_L_l.now + pid_L_l.I*error_L_l.all;// + pid_L_v.D*(error_L_v.now-error_L_v.last);
	//State_R.speed_exp = pid_R_l.P*error_R_l.now + pid_R_l.I*error_R_l.all;// + pid_R_v.D*(error_R_v.now-error_R_v.last);
	
	if(State_L.speed_exp>=275)
	{
		State_L.speed_exp = 275;
	}
	if(State_R.speed_exp>=275)
	{
		State_R.speed_exp = 275;
	}
	
	// 计算两轮子转速,每分钟圈数,
	State_L.speed_act = (float)encode_L*100.0*60/1060.0 ;
	State_R.speed_act = (float)encode_R*100.0*60/1060.0 ;
	
	// 记录两轮子速度的误差值
	error_L_v.last = error_L_v.now;
	error_L_v.now  = State_L.speed_exp-State_L.speed_act;
	error_L_v.all += error_L_v.now;
	error_R_v.last = error_R_v.now;
	error_R_v.now  = State_R.speed_exp-State_R.speed_act;
	error_R_v.all += error_R_v.now;
	
	State_L.output = pid_L_v.P*error_L_v.now + pid_L_v.I*error_L_v.all;// + pid_L_v.D*(error_L_v.now-error_L_v.last);
	State_R.output = pid_R_v.P*error_R_v.now + pid_R_v.I*error_R_v.all;// + pid_R_v.D*(error_R_v.now-error_R_v.last);
	MOTOR_Forward(State_L.output,State_R.output);
}

unsigned char printf_temp[32];

void uart_printf(const char *format,...)
{
	unsigned short len;
	va_list args;
	va_start(args, format);
	len= vsnprintf((char*)printf_temp, sizeof(printf_temp)+1,(char*)format,args);
	va_end(args);
	HAL_UART_Transmit_IT(&huart6, printf_temp, len);
}
