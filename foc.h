#ifndef FOC_H
#define FOC_H

#include "cfg.h"
typedef struct
{
	float Iq;
	float Id;
}Park;

typedef struct
{
	float Ialpha;
	float Ibeta;
}Clark;

typedef struct
{
	float Ia;
	float Ib;
	float Ic;
}Raw;

typedef struct
{
	float theta;
	Park motor_p;
	Clark motor_c;
	Raw motor_r;	
}Foc;

typedef struct
{
	float alpha;
	float s_last;
	float s_now;
	uint8_t trig;
}Filt;

void ParkConv(void);
void ClarkConv(void);
void ParkConvT(void);
void ClarkConvT(void);
float LowPass(float x);
void FiltConf(void);

#endif
