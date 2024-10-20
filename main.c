#include "main.h"
#include "cfg.h"

extern motor_ctrl motor;
extern Foc motor_foc;
extern TIM_HandleTypeDef htim3;
extern uint32_t g_adc_ave[ADC_CH_NUM];
extern uint32_t target_rpm;
extern float target_angle;
extern float angle;
extern pulse_volt volt_out;

uint8_t rxflag = 0;

int main(void)
{
	sys_init();
	/*motor.pulsea = (TIMARR+1)/2;
	motor.pulseb = (TIMARR+1)/2;
	motor.pulsec = (TIMARR+1)/2;
	
	motor.dir = FORWARD;
	motor.run_flag = START;*/
    while (1)
    {
		angle=AS5047_read(ANGLEUNC);
		//HAL_Delay(5);
		/*if(rxflag)
		{
			rxflag = 0;
			printf("rxdata:%d\n", target_rpm);
			HAL_Delay(5);
		}
		printf("tar_angle:%f, angle:%f\n", target_angle, angle);
		HAL_Delay(5);*/
		printf("angle:%f\n", angle);
		HAL_Delay(5);
    }
}



