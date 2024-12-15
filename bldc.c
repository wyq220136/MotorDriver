#include "stm32f1xx_hal.h"
#include "cfg.h"

motor_ctrl motor = {0};
int32_t cnt_all = 0, last_cnt = 0;
extern TIM_HandleTypeDef TIM1_Handler, htim3;
extern int16_t counter;
extern uint8_t start_cnt; //娑撳﹤宕屽▽鑳箯閸欐牗鐖ｈ箛妞剧秴
extern long long encoder_num;
extern uint32_t normal_cnt; //閼惧嘲褰囨潏鎾冲弳閹规洝骞忕拋鈩冩殶閸婏拷
extern Foc motor_foc;
extern float angle;
extern pulse_volt volt_out;
extern adrc Adrc1, Adrc2, Adrc3;
extern Foc motor_foc;
extern uint8_t g_adc_dma_sta;
extern uint16_t g_adc_ave[ADC_CH_NUM];
extern Filt filter;
float last_angle = 0.0;
uint8_t last_sta = 0;


pctr basic_ctrl[6] =
{
	&uhwl, &vhul, &vhwl,
	&whvl, &uhvl, &whul
};


uint8_t forward[6] = {5, 1, 3, 2, 6, 4};
uint8_t backward[6] = {4, 6, 2, 3, 1, 5};

uint8_t idx_f[6] = {1, 3, 2, 5, 0, 4};

//璇诲彇闇嶅皵浼犳劅鍣�
uint8_t hallsensor(void) 
{
	uint8_t state = 0x00;
	if(HAL_GPIO_ReadPin(GPIO_HALLSENSOR, GPIO_PIN_14) != RESET)
	{
		state |= 0x01;
	}
	if(HAL_GPIO_ReadPin(GPIO_HALLSENSOR, GPIO_PIN_13) != RESET)
	{
		state |= 0x02;
	}
	if(HAL_GPIO_ReadPin(GPIO_HALLSENSOR, GPIO_PIN_12) != RESET)
	{
		state |= 0x04;
	}
	
	return state;
}

//鍏鎹㈠悜鍑芥暟
void uhvl(void)
{
	TIM1_Handler.Instance->CCR4= motor.pulsea;
	TIM1_Handler.Instance->CCR2 = 0;
	TIM1_Handler.Instance->CCR3 = 0;

	UL_OFF;
	VL_ON;
	WL_OFF;
}

void uhwl(void)
{
	TIM1_Handler.Instance->CCR4 = motor.pulsea;
	TIM1_Handler.Instance->CCR2 = 0;
	TIM1_Handler.Instance->CCR3 = 0;

	UL_OFF;
	VL_OFF;
	WL_ON;
}

void vhul(void)
{
	TIM1_Handler.Instance->CCR4 = 0;
	TIM1_Handler.Instance->CCR2 = motor.pulseb;
	TIM1_Handler.Instance->CCR3 = 0;

	UL_ON;
	VL_OFF;
	WL_OFF;
}

void vhwl(void)
{
	TIM1_Handler.Instance->CCR4 = 0;
	TIM1_Handler.Instance->CCR2 = motor.pulseb;
	TIM1_Handler.Instance->CCR3 = 0;

	UL_OFF;
	VL_OFF;
	WL_ON;
}

void whul(void)
{
	TIM1_Handler.Instance->CCR4 = 0;
	TIM1_Handler.Instance->CCR2 = 0;
	TIM1_Handler.Instance->CCR3 = motor.pulsec;

	UL_ON;
	VL_OFF;
	WL_OFF;
}

void whvl(void)
{
	TIM1_Handler.Instance->CCR4 = 0;
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
	if(htim->Instance == TIM2)
	{
		if(motor.run_flag == START)    //只有电机运行状态下才读取
		{
			motor.step_last = motor.step_now;
			motor.step_now = hallsensor();
			if(motor.step_now >= 1 && motor.step_now <= 6)
			{
				if(motor.dir == FORWARD) //鐢垫満姝ｈ浆
				{
					basic_ctrl[motor.step_now-1]();			
				}
				else
				{
					basic_ctrl[6 - motor.step_now]();	
				}
			}
		}
		else
			Stop_motor();
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
