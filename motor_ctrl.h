#ifndef MOTOR_CTRL_H
#define MOTOR_CTRL_H
#include "stm32f1xx_hal.h"

#define Kp 0.095
#define Ki  0.007
#define Kd 0.001

//这是一个PID控制器
typedef struct
{
	int16_t ka;
	int16_t kb;
	int16_t kc;
	
	int16_t error;
	int16_t error_t;
	int16_t error_t_prim;
	
	uint16_t limit_h;//输出限幅，PWM占空比非负
	uint16_t limit_l;
	
	uint16_t pid_out;
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
void cal_motor(pid*k);
void adrcConf(adrc*k);
void adrcRSET(adrc*k);
void adrcTD(adrc*k, float x);
void adrcEso(adrc*k, float feedback);
void adrcLf(adrc*k);
float adrcCal(adrc*k, float expect, float actual);

#endif
