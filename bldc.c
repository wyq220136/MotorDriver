#include "stm32f1xx_hal.h"
#include "cfg.h"

motor_ctrl motor = {0};
int32_t cnt_all = 0, last_cnt = 0;
extern TIM_HandleTypeDef TIM1_Handler, htim3;
extern Foc motor_foc;
extern float angle;
extern pulse_volt volt_out;
extern adrc Adrc1, Adrc2, Adrc3;
extern Foc motor_foc;
extern uint8_t g_adc_dma_sta;
extern uint16_t g_adc_ave[ADC_CH_NUM];
extern Filt filter;
uint8_t ctrl_cnt = 0;

pctr basic_ctrl[6] =
{
	&phase_zero, &phase_one, &phase_two,
	&phase_three, &phase_four, &phase_five
};

//鍏鎹㈠悜鍑芥暟
void phase_zero(void)
{
	TIM1_Handler.Instance->CCR4= motor.pulsea;
	TIM1_Handler.Instance->CCR2 = 0;
	TIM1_Handler.Instance->CCR3 = 0;

	UL_OFF;
	VL_ON;
	WL_ON;
}

void phase_one(void)
{
	TIM1_Handler.Instance->CCR4 = motor.pulsea;
	TIM1_Handler.Instance->CCR2 = motor.pulseb;
	TIM1_Handler.Instance->CCR3 = 0;

	UL_OFF;
	VL_OFF;
	WL_ON;
}

void phase_two(void)
{
	TIM1_Handler.Instance->CCR4 = 0;
	TIM1_Handler.Instance->CCR2 = motor.pulseb;
	TIM1_Handler.Instance->CCR3 = 0;

	UL_ON;
	VL_OFF;
	WL_ON;
}

void phase_three(void)
{
	TIM1_Handler.Instance->CCR4 = 0;
	TIM1_Handler.Instance->CCR2 = motor.pulseb;
	TIM1_Handler.Instance->CCR3 = motor.pulsec;

	UL_ON;
	VL_OFF;
	WL_OFF;
}

void phase_four(void)
{
	TIM1_Handler.Instance->CCR4 = 0;
	TIM1_Handler.Instance->CCR2 = 0;
	TIM1_Handler.Instance->CCR3 = motor.pulsec;

	UL_ON;
	VL_ON;
	WL_OFF;
}

void phase_five(void)
{
	TIM1_Handler.Instance->CCR4 = motor.pulsea;
	TIM1_Handler.Instance->CCR2 = 0;
	TIM1_Handler.Instance->CCR3 = motor.pulsec;

	UL_OFF;
	VL_ON;
	WL_OFF;
}

void Stop_motor(void)   //閫氳繃灏咵N_GATE浠庨珮缃綆鍏虫柇妗ヨ噦
{
	TIM1_Handler.Instance->CCR4 = 0;
	TIM1_Handler.Instance->CCR2 = 0;
	TIM1_Handler.Instance->CCR3 = 0;
	
	UL_ON;
	VL_ON;
	WL_ON;
}


/*******************************************************************************
定时器回调函数，进行六步换相和数据读取
*******************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2) //5us进一次
	{
		ctrl_cnt++;
		if(ctrl_cnt >= 4) //下半周期使用下一个矢量
			basic_ctrl[motor_foc.idx1+1]();
	}
	
	if(htim->Instance == TIM5)
	{
		GetPos();
		ParkConvT();
		SVPWM();
		
	}
	if(htim->Instance == TIM4)
	{
		cnt_all = __HAL_TIM_GET_COUNTER(&htim3);
		getAngle();
		cal_motor();
		
		//adc_ave_filter();
		/*if(g_adc_dma_sta == 1)
		{
		adc_ave_filter();
		g_adc_dma_sta = 0;
		volt_out.a_out = adrcCal(&Adrc1, motor_foc.motor_r.Ia+12, g_adc_ave[0]);
		volt_out.b_out = adrcCal(&Adrc2, motor_foc.motor_r.Ib+12, g_adc_ave[1]);
		volt_out.c_out = adrcCal(&Adrc3, motor_foc.motor_r.Ic+12, g_adc_ave[2]);
		adc_dma_enable(ADC_CH_NUM*ADC_MEM_NUM);
			
		//printf("adc1:%d, adc2:%d, adc3:%d\n", g_adc_ave[0], g_adc_ave[1], g_adc_ave[2]);
		//motor.pulsea = (__Constrain(volt_out.a_out/V_SOURCE)+V_OFFSET)*(TIMARR+1);
		//motor.pulseb = (__Constrain(volt_out.b_out/V_SOURCE)+V_OFFSET)*(TIMARR+1);
		//motor.pulsec = (__Constrain(volt_out.c_out/V_SOURCE)+V_OFFSET)*(TIMARR+1);
		}*/
	}
}
