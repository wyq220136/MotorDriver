#include "main.h"
#include "cfg.h"

extern motor_ctrl motor;
extern Foc motor_foc;
extern TIM_HandleTypeDef htim3;
volatile uint16_t ecnt = 0;
int main(void)
{
	sys_init();
	motor.pulsea = (TIMARR+1)/2;
	motor.pulseb = (TIMARR+1)/2;
	motor.pulsec = (TIMARR+1)/2;
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
	
	
	motor.dir = FORWARD;
	motor.run_flag = START;
    while (1)
    {

    }
}



