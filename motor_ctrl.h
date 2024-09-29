#ifndef MOTOR_CTRL_H
#define MOTOR_CTRL_H
#include "stm32f1xx_hal.h"

#define Kp 0.15
#define Ki  0.07
#define Kd 0.0

//这是一个PID控制器
typedef struct
{
	float ka;
	float kb;
	float kc;
	
	float error;
	float error_t;
	float error_t_prim;
	
	float limit_h;//输出限幅，PWM占空比非负
	float limit_l;
	
	float pid_out;
}pid;

typedef struct
{
	float omegac;
	float omega_zero;
	
	float ts;
	float vy;
	float vy_prim;
	float f;
	
	float v1;
	float v2;
	
	float kp;
	float kd;
	
	float u0;
	float u;
	float b0;
	float r;
}adrc;

void ctrl_Init(void);
void Enc_Rpm(void);
void Rpm_Speed(void);
void Speed_Rpm(void);
void Pid_Conf(pid*k);
void Pid_Cal(pid*k, int16_t e);
void cal_motor();
void adrcConf(adrc*k);
void adrcRSET(adrc*k);
void adrcTD(adrc*k, float x);
void adrcEso(adrc*k, float feedback);
void adrcLf(adrc*k);
float adrcCal(adrc*k, float expect, float actual);

#endif
