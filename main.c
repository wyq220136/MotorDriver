#include "main.h"
#include "cfg.h"

extern pid Pid;
extern motor_ctrl motor;
extern Foc motor_foc;
extern TIM_HandleTypeDef htim3;
extern uint32_t g_adc_ave[ADC_CH_NUM];
extern uint32_t target_rpm;
extern float target_angle;
extern pulse_volt volt_out;
extern Filt filter;
extern uint8_t rxdat[5];
extern uint8_t roundcnt;


uint8_t rxflag = 0;
uint8_t tmp_test = 0;
extern int32_t cnt_tmp;
extern int32_t cnt_all;
extern int32_t last_cnt;
//
extern float errr;

int main(void)
{
	sys_init();
	//target_rpm = 0;
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
	
	motor.dir = FORWARD;
	motor.run_flag = START;
	HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t*)&rxdat, 5);
    while (1)
    {
		//printf("%f, %f, %f\n", target_angle, motor_foc.T1, Pid.pid_out);
			printf("%f, %f, %f, %f, %f, %d\n", motor_foc.theta, motor_foc.cal_angle, motor_foc.motor_p.Iq, motor_foc.T1, motor_foc.T2, motor_foc.sector);
			//target_angle    °/ms   
		HAL_Delay(5);
    }
}








//MotorDriver
