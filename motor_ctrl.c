#include "cfg.h"
#include "net_pid.h"
#include <stdlib.h>
#include "motor_ctrl.h"
#include "math.h"
/************************************************************************
这个文件用于电机闭环控制
日期：2024.8.17，负责人wyq

*************************************************************************/
extern motor_ctrl motor;
extern Foc motor_foc;
uint16_t counter = 0;
uint8_t start_cnt = 0;//上升沿捕获标志位，捕获到上升沿置1，下降沿置0
uint32_t normal_cnt = 0;
uint32_t speed_actual, target_speed;

long long encoder_num = 0;//用于记录霍尔编码器码数
uint32_t rpm, target_rpm;

adrc Adrc;
pid Pid;

void ctrl_Init(void)
{
	adrcConf(&Adrc);
	Pid_Conf(&Pid);
}

//编码速度转换为转速
void Enc_Rpm(void)
{
	rpm = encoder_num / POLE_NUM;
}

//转速转换为实际速度，单位m/s
void Rpm_Speed(void)
{
	speed_actual = 2*rpm*PI*RADIUS;
}

//实际速度转换为转速，用于目标转速计算
void Speed_Rpm(void)
{
	target_rpm = target_speed / (2*rpm*PI);
}

//PID控制器初始化函数
void Pid_Conf(pid*k)
{
	k->ka = Kp+Ki+Kd;
	k->kb = -Kp-2*Kd;
	k->kc = Kd;
	
	k->limit_h = TIMARR*V_MAX/V_SOURCE;
	k->limit_l = 0;
	
	k->error = 0;
	k->error_t = 0;
	k->error_t_prim = 0;
	
	k->pid_out = 0;
}

//PID控制器输出计算
void Pid_Cal(pid*k, int16_t e)
{
	k->error_t_prim = k->error_t;
	k->error_t = k->error;
	k->error = e;
	
	int16_t pid_out;
	pid_out = k->ka*k->error+k->kb*k->error_t+k->kc*k->error_t_prim;
	
	k->pid_out += pid_out;
	k->pid_out = k->pid_out>k->limit_h?k->limit_h:(k->pid_out<k->limit_l?k->limit_l:k->pid_out);
}

void cal_motor(pid*k)
{
	Speed_Rpm();
	Enc_Rpm();
	int16_t err = target_rpm - rpm;
	Pid_Cal(&Pid, err);
	motor_foc.motor_p.Iq = k->pid_out;
}

void adrcConf(adrc*k)
{
	k->omegac = OMEGAC;
	k->omega_zero = 4*k->omegac;
	k->ts = SAMPLETIME;
	
	k->vy = 0;
	k->vy_prim = 0;
	k->f = 0;
	
	k->kp = KPADRC;
	k->kd = KDADRC;
	
	k->b0 = BADRC;
	k->r = RADRC;
	
	k->u0 = 0;
	k->u = 0;
	
	k->v1 = 0;
	k->v2 = 0;
}

void adrcRSET(adrc*k)
{
	k->vy = 0;
	k->vy_prim = 0;
	k->f = 0;
	
	k->v1 = 0;
	k->v2 = 0;
}

void adrcTD(adrc*k, float x)
{
	float fh = k->r*k->r*(x-k->v1)-2*k->r*k->v2;
	k->v1 += k->v2*k->ts+fh*k->ts*k->ts-x;
	k->v2 += fh*k->ts;
}

void adrcEso(adrc*k, float feedback)
{
	float beta1 = 3*k->omega_zero;
	float beta2 = 3*k->omega_zero*k->omega_zero;
	float beta3 = k->omega_zero*k->omega_zero*k->omega_zero;
	
	float e = k->vy - feedback;
	k->vy += (k->vy_prim - beta1*e)*k->ts;
	k->vy_prim += (k->f - beta2*e + k->b0*k->u)*k->ts;
	k->f += -beta3*e*k->ts;
}

void adrcLf(adrc*k)
{
	float e1 = k->v1-k->vy;
	float e2 = k->v2-k->vy_prim;
	k->u = k->kp*e1+k->kd*e2;
	
	k->u0 = (k->u-k->f)/k->b0;
	if(k->u0>ADRCLIMIT)
		k->u0 = ADRCLIMIT;
	if(k->u0< -ADRCLIMIT)
		k->u0 = -ADRCLIMIT;
}

float adrcCal(adrc*k, float expect, float actual)
{
	adrcTD(k, expect);
	adrcEso(k, actual);
	adrcLf(k);
	return k->u0;
}
