#include "main.h"
#include "cfg.h"

extern pid Pid;
extern motor_ctrl motor;
extern Foc motor_foc;
extern TIM_HandleTypeDef htim3;
extern uint32_t g_adc_ave[ADC_CH_NUM];
extern uint32_t target_rpm;
extern float target_angle;
extern float angle, last_angle;
extern pulse_volt volt_out;
extern Filt filter;
extern uint8_t rxdat[5];

uint8_t rxflag = 0;
uint8_t tmp_test = 0;

int main(void)
{
	sys_init();
	//target_rpm = 0;
	//motor.pulsea = (TIMARR+1)/2;
	//motor.pulseb = (TIMARR+1)/2;
	//motor.pulsec = (TIMARR+1)/2;
	
	
	motor.dir = FORWARD;
	motor.run_flag = START;
	HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t*)&rxdat, 5);
    while (1)
    {
		if(filter.trig)
		{
			filter.trig = 0;
			getAngle();
		}
		printf("last:%f, angle:%f, theta:%f\n",last_angle, angle, motor_foc.theta);
		HAL_Delay(5);
    }
}



