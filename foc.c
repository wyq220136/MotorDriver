#include "cfg.h"
#include "math.h"

Foc motor_foc = {0};
Filt filter = {0};
extern motor_ctrl motor;

uint8_t GetSector(float x, float y)
{
	if(x == 0 && y == 0)
		return 0;
	motor_foc.atan_now = atan2(y, x);
	if((0 <= motor_foc.atan_now)&&(PI/3 >= motor_foc.atan_now))
		return 1;
	if((PI/3 <= motor_foc.atan_now)&&(2*PI/3 >= motor_foc.atan_now))
		return 2;
	if((PI*2/3 <= motor_foc.atan_now)&&(PI >= motor_foc.atan_now))
		return 3;
	if((-PI <= motor_foc.atan_now)&&(-2*PI/3 >= motor_foc.atan_now))
		return 4;
	if((-PI*2/3 <= motor_foc.atan_now)&&(-PI/3 >= motor_foc.atan_now))
		return 5;
	return 6;
}

void ParkConv(void)
{
	motor_foc.motor_c.Ialpha = 2*(motor_foc.motor_r.Ia-0.5*(motor_foc.motor_r.Ib+motor_foc.motor_r.Ic))/3;
	motor_foc.motor_c.Ibeta = SQRT3*(motor_foc.motor_r.Ib-motor_foc.motor_r.Ic)/2;
}

void ClarkConv(void)
{
	motor_foc.motor_p.Id = motor_foc.motor_c.Ialpha*cos(motor_foc.theta)+motor_foc.motor_c.Ibeta*sin(motor_foc.theta);
	motor_foc.motor_p.Iq = -motor_foc.motor_c.Ialpha*sin(motor_foc.theta)+motor_foc.motor_c.Ibeta*cos(motor_foc.theta);
}

void ParkConvT(void)
{
	motor_foc.motor_c.Ialpha = -motor_foc.motor_p.Iq*sin(motor_foc.theta);
	motor_foc.motor_c.Ibeta = motor_foc.motor_p.Iq*cos(motor_foc.theta);
	motor_foc.sector = GetSector(motor_foc.motor_c.Ialpha, motor_foc.motor_c.Ibeta);
	motor_foc.cal_angle = (motor_foc.theta - 60*motor_foc.sector)/180*PI;
	motor_foc.motor_c.Ialpha = -motor_foc.motor_p.Iq*sin(motor_foc.cal_angle);
	motor_foc.motor_c.Ibeta = motor_foc.motor_p.Iq*cos(motor_foc.cal_angle);
}

void SVPWM(void)
{
	motor_foc.T1 = (motor_foc.motor_c.Ialpha - motor_foc.motor_c.Ibeta/SQRT3)/V_SOURCE;
	motor_foc.T2 = 2*motor_foc.motor_c.Ibeta/SQRT3/V_SOURCE;
	
	motor_foc.idx1 = motor_foc.sector-1;
	
	switch(motor_foc.idx1)
	{
		case 0:
			motor.pulsea = 0.866*(motor_foc.T2+motor_foc.T1+V_OFFSET)*(TIMARR+1);
			motor.pulseb = 0.866*(motor_foc.T2+V_OFFSET)*(TIMARR+1);
			motor.pulsec = 0.866*V_OFFSET*(TIMARR+1);
			break;
		case 1:
			motor.pulsea = 0.866*(motor_foc.T1+V_OFFSET)*(TIMARR+1);
			motor.pulseb = 0.866*(motor_foc.T2+motor_foc.T1+V_OFFSET)*(TIMARR+1);
			motor.pulsec = 0.866*V_OFFSET*(TIMARR+1);
			break;
		case 2:
			motor.pulsea = 0.866*V_OFFSET*(TIMARR+1);
			motor.pulseb = 0.866*(motor_foc.T2+motor_foc.T1+V_OFFSET)*(TIMARR+1);
			motor.pulsec = 0.866*(motor_foc.T2+V_OFFSET)*(TIMARR+1);
			break;
		case 3:
			motor.pulsea = 0.866*V_OFFSET*(TIMARR+1);
			motor.pulseb = 0.866*(motor_foc.T1+V_OFFSET)*(TIMARR+1);
			motor.pulsec = 0.866*(motor_foc.T2+motor_foc.T1+V_OFFSET)*(TIMARR+1);
			break;
		case 4:
			motor.pulsea = 0.866*(motor_foc.T2+V_OFFSET)*(TIMARR+1);
			motor.pulseb = 0.866*V_OFFSET*(TIMARR+1);
			motor.pulsec = 0.866*(motor_foc.T2+motor_foc.T1+V_OFFSET)*(TIMARR+1);
			break;
		case 5:
			motor.pulsea = 0.866*(motor_foc.T2+motor_foc.T1+V_OFFSET)*(TIMARR+1);
			motor.pulseb = 0.866*V_OFFSET*(TIMARR+1);
			motor.pulsec = 0.866*(motor_foc.T1+V_OFFSET)*(TIMARR+1);
			break;
	}
}

float LowPass(float x)
{
	float out;
	filter.s_last = filter.s_now;
	filter.s_now = x;
	out = filter.alpha*filter.s_now + filter.s_last*(1-filter.alpha);
	return out;
}

void FiltConf(void)
{
	filter.alpha = 0.5;
	filter.s_last = 0;
	filter.s_now = 0;
	filter.trig = 0;
}

