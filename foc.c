#include "cfg.h"
#include "math.h"

Foc motor_foc = {0};
Filt filter = {0};
extern motor_ctrl motor;

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
}

void ClarkConvT(void)
{
	motor_foc.motor_r.Ia = motor_foc.motor_c.Ialpha;
	motor_foc.motor_r.Ib = -motor_foc.motor_c.Ialpha/2+motor_foc.motor_c.Ibeta*SQRT3/2;
	motor_foc.motor_r.Ic = -motor_foc.motor_c.Ialpha/2-motor_foc.motor_c.Ibeta*SQRT3/2;
	
	motor.pulsea = (__Constrain(motor_foc.motor_r.Ia/V_SOURCE)+V_OFFSET)*(TIMARR+1);
	motor.pulseb = (__Constrain(motor_foc.motor_r.Ib/V_SOURCE)+V_OFFSET)*(TIMARR+1);
	motor.pulsec = (__Constrain(motor_foc.motor_r.Ic/V_SOURCE)+V_OFFSET)*(TIMARR+1);
}

float LowPass(float x)
{
	float out;
	filter.s_last = filter.s_now;
	filter.s_now = x;
	out = filter.alpha*filter.s_now + filter.s_last*(1-filter.alpha);
	return out;
}
