#include "encode.h"
#include "cfg.h"

SPI_HandleTypeDef hspi1;
extern float last_angle;
extern int32_t cnt_all, last_cnt;
int32_t cnt_tmp = 0;
extern Foc motor_foc;

void getAngle(void)
{
	if(((cnt_all > last_cnt)&&(cnt_all - last_cnt < 20000))||(last_cnt - cnt_all>50000))
	{
		if(last_cnt - cnt_all > 50000)
			cnt_tmp = cnt_all+65535-last_cnt;
		else
			cnt_tmp = cnt_all - last_cnt;
	}
	else
	{
		if(cnt_all - last_cnt > 50000)
			cnt_tmp = cnt_all - 65535 - last_cnt;
		else
			cnt_tmp = cnt_all - last_cnt;
	}
	motor_foc.theta = (cnt_tmp/4096.0)*360.0;
	last_cnt = cnt_all;
}
