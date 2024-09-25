#ifndef FOC_H
#define FOC_H

#include "cfg.h"
typedef struct
{
	int16_t Iq;
	int16_t Id;
}Park;

typedef struct
{
	int16_t Ialpha;
	int16_t Ibeta;
}Clark;

typedef struct
{
	int16_t Ia;
	int16_t Ib;
	int16_t Ic;
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
}Filt;

#endif
