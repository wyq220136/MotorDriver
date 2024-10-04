#include "main.h"
#include "cfg.h"

extern motor_ctrl motor;
extern Foc motor_foc;
extern TIM_HandleTypeDef htim3;
extern uint32_t g_adc_ave[ADC_CH_NUM];

int main(void)
{
	sys_init();
	//motor.pulsea = (TIMARR+1)/2;
	//motor.pulseb = (TIMARR+1)/2;
	//motor.pulsec = (TIMARR+1)/2;
	
	//motor.dir = FORWARD;
	//motor.run_flag = START;
    while (1)
    {
		//printf("%f\n", g_adc_ave[1]);
		//HAL_Delay(5);
    }
}



