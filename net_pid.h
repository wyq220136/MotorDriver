#ifndef NET_PID_H
#define NET_PID_H
#include "stm32f1xx_hal.h"

#define IS_USING_NET 0		//是否启用自适应PID，1：启用，0：不启用

typedef struct
{
	int idx;
	float*data;
	float outtmp;
	int length;
}u_que;


typedef struct
{
	float i1;
	float i2;
	float i3;
	
	u_que u1;
	u_que u2;
	u_que u3;
	
	float layer1[9];
	float layer2[3];
	
	u_que out;
	u_que net;
	
	float alpha1;
	float alpha2;
	
	float limith;
	float limitl;
	
	int flag;
	uint16_t pid_out;
}net_pid;

// 使用反向传播动态更新pid,下面是优先级队列定义及实现
void u_que_init(u_que*k, int l);
void update_que(u_que*k, float x);
// pid神经元核心函数，正向传播，反向传播
void netpid_init(net_pid* k, float alpha1, float alpha2, float limit);
int pid_output(net_pid* k, float error);
void net_update(net_pid* k, u_que*error);
float calc_f_grad(float limit, float output);
int jud_grad(float a, float b);

#endif
