#include "net_pid.h"
#include "cfg.h"
#include "math.h"


void u_que_init(u_que*k, int l)
{
	int i = 0;
	k->idx = 0;
	k->length = l;
	for(i = 0; i < l; i++)
		k->data[i] = 0;
	k->outtmp = 0;
}

void update_que(u_que*k, float x)
{
	k->outtmp = k->data[k->idx];
	k->data[k->idx] = x;
	k->idx = (k->idx+1)%4;
}

void netpid_init(net_pid* k, float alpha1, float alpha2, float limit)
{
	int i = 4;
	k->alpha1 = alpha1;
	k->alpha2 = alpha2;
	
	u_que_init(&k->u1, 4);
	u_que_init(&k->u2, 4);
	u_que_init(&k->u3, 4);
	
	u_que_init(&k->out, 4);
	u_que_init(&k->net, 4);
	
	k->i1 = 0;
	k->i2 = 0;
	k->i3 = 0;
	
	k->limith = TIMARR*V_MAX/V_SOURCE;
	k->flag = 0;
	
	k->layer1[0] = 0.09;
	k->layer1[1] = 0.0005;
	k->layer1[2] = 0;
	
	k->layer1[3] = -0.09;
	for(i = 4; i<9; i++)
		k->layer1[i] = 0;
	for(i = 0; i<3; i++)
		k->layer2[i] = 1;
	k->pid_out = 0;
}

// 计算pid输出
int pid_output(net_pid* k, float error)
{
	float u1, u2, u3;
	
	k->i3 = k->i2;
	k->i2 = k->i1;
	k->i1 = error;
	
	u1 = k->layer1[0] * k->i1 + k->layer1[3] * k->i2 + k->layer1[6] * k->i3;
	u2 = k->layer1[1] * k->i1 + k->layer1[4] * k->i2 + k->layer1[7] * k->i3;
	u3 = k->layer1[2] * k->i1 + k->layer1[5] * k->i2 + k->layer1[8] * k->i3;
	
	update_que(&k->u1, u1);
	update_que(&k->u2, u2);
	update_que(&k->u3, u3);
	
	k->pid_out += k->layer2[0] * u1 + k->layer2[1] * u2 + k->layer2[2] * u3;
	
	update_que(&k->net, k->pid_out);
	if(k->pid_out > k->limith)
		k->pid_out = k->limith;
	if(k->pid_out < k->limitl)
		k->pid_out = k->limitl;
	update_que(&k->out, k->pid_out);
	
	return k->pid_out;
}

// 激活函数导数计算
float calc_f_grad(float limit, float output)
{
	if(output >= 0)
		return output/(2*limit);
	else
		return -output/(2*limit);
}

// 判断梯度下降方向，避免差分出现0，用符号函数激活
int jud_grad(float a, float b)
{
	int seg;
	if(a*b > 0) seg = 1;
	if(0 == a*b) seg =  0;
	if(a*b < 0) seg = -1;
	return seg;
}

// 反向传播更新网络权值 wyq 2024.5
void net_update(net_pid* k, u_que*error)
{
	
	float delta[3] = {0}, tmp, tmp_layer1[9] = {0};
	int i = 0, j = 0, idx[7];
	
	idx[0] = k->u1.idx, idx[1] = k->u2.idx, idx[2] = k->u3.idx, idx[3] = k->out.idx, 
	idx[4] = error->idx, idx[5] = k->net.idx, idx[6] = (error->idx+4)%6; 
	
	for(i = 0; i < 4; i++)
	{
		tmp = (error->data[idx[4]] * calc_f_grad(k->limith, k->out.data[idx[3]]) * 
		jud_grad(fabs(error->data[idx[4]]) -  fabs(error->data[(idx[4]+5)%6]), 
		fabs(k->out.data[idx[3]]) - fabs(k->out.data[(idx[3]+3)%4])))/4;
		delta[0] += tmp*k->u1.data[idx[0]];
		delta[1] += tmp*k->u2.data[idx[1]];
		delta[2] += tmp*k->u3.data[idx[2]];
		
		tmp_layer1[0] += tmp*k->layer2[0]*error->data[idx[6]];
		tmp_layer1[1] += tmp*k->layer2[1]*error->data[idx[6]];
		tmp_layer1[2] += tmp*k->layer2[2]*error->data[idx[6]];
		
		tmp_layer1[3] += tmp*k->layer2[0]*error->data[(idx[6]+5)%6];
		tmp_layer1[4] += tmp*k->layer2[1]*error->data[(idx[6]+5)%6];
		tmp_layer1[5] += tmp*k->layer2[2]*error->data[(idx[6]+5)%6];
		
		tmp_layer1[6] += tmp*k->layer2[0]*error->data[(idx[6]+4)%6];
		tmp_layer1[7] += tmp*k->layer2[1]*error->data[(idx[6]+4)%6];
		tmp_layer1[8] += tmp*k->layer2[2]*error->data[(idx[6]+4)%6];
		
		for(j = 0; j<6; j++) 
			idx[j] = (idx[j]+3)%4;
		idx[6] = (idx[6]+5)%6;
	}
	
	for(i = 0; i<3; i++) k->layer2[i] -= k->alpha1*delta[i];
	for(j = 0; j<9; j++) k->layer1[j] -= k->alpha2*tmp_layer1[j];
}
