#include "stm32f1xx_hal.h"
#include "cfg.h"
#include "encode.h"

/******************************************************************************
2024.8.17 负责人：wyq
这个文件完成系统初始化
******************************************************************************/

TIM_HandleTypeDef TIM1_Handler, TIM2_Handler;
TIM_OC_InitTypeDef TIM1_CH1Handler;
TIM_OC_InitTypeDef TIM1_CH2Handler;
TIM_OC_InitTypeDef TIM1_CH3Handler;
TIM_HandleTypeDef htim3, htim4;
ADC_HandleTypeDef    AdcHandle;
extern motor_ctrl motor;
extern uint16_t g_adc_raw[ADC_CH_NUM*ADC_MEM_NUM];


void h_bridge_init(void)//上桥臂初始化，控制方式采用h_pwm_l_on
{
	uint16_t psc = TIMPSC;
	uint16_t arr = TIMARR;
	TIM1_Handler.Instance=TIM1;          	
	TIM1_Handler.Init.Prescaler=psc;       
	TIM1_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;
	TIM1_Handler.Init.Period=arr;          
	TIM1_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&TIM1_Handler);      
    
	TIM1_CH1Handler.OCMode=TIM_OCMODE_PWM1; 
	TIM1_CH1Handler.Pulse=0;
	TIM1_CH1Handler.OCPolarity=TIM_OCPOLARITY_HIGH;  
	HAL_TIM_PWM_ConfigChannel(&TIM1_Handler,&TIM1_CH1Handler,TIM_CHANNEL_4);
	
	HAL_TIM_PWM_Start(&TIM1_Handler,TIM_CHANNEL_4);
	
	TIM1_CH2Handler.OCMode=TIM_OCMODE_PWM1;
	TIM1_CH2Handler.Pulse=0;
	TIM1_CH2Handler.OCPolarity=TIM_OCPOLARITY_HIGH; 
	HAL_TIM_PWM_ConfigChannel(&TIM1_Handler,&TIM1_CH2Handler,TIM_CHANNEL_2);
	
	HAL_TIM_PWM_Start(&TIM1_Handler,TIM_CHANNEL_2);
	
	
	TIM1_CH3Handler.OCMode=TIM_OCMODE_PWM1; 
	TIM1_CH3Handler.Pulse=0;            
	TIM1_CH3Handler.OCPolarity=TIM_OCPOLARITY_HIGH; 
	HAL_TIM_PWM_ConfigChannel(&TIM1_Handler,&TIM1_CH3Handler,TIM_CHANNEL_3);
	
	HAL_TIM_PWM_Start(&TIM1_Handler,TIM_CHANNEL_3);
	
}

void l_bridge_init(void)
{
	GPIO_InitTypeDef gpio_init_struct;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	gpio_init_struct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
	gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_init_struct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIO_LBridge, &gpio_init_struct);
}

void H_sensor_init(void)				//霍尔信号采集引脚初始化
{
	GPIO_InitTypeDef gpio_init_struct;
	//__HAL_RCC_GPIOA_CLK_ENABLE();
	
	gpio_init_struct.Pin = GPIO_PIN_14|GPIO_PIN_13|GPIO_PIN_12;
	gpio_init_struct.Mode = GPIO_MODE_INPUT;
	gpio_init_struct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &gpio_init_struct);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)   //PWM回调函数，用于IO复用配置
{
	if(htim->Instance == TIM1)
	{
		GPIO_InitTypeDef gpio_init_struct;
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_TIM1_CLK_ENABLE();
		
		gpio_init_struct.Pin = GPIO_PIN_11 | GPIO_PIN_9 | GPIO_PIN_10;
		gpio_init_struct.Mode = GPIO_MODE_AF_PP;
		gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
		gpio_init_struct.Pull = GPIO_PULLUP;
		
		HAL_GPIO_Init(GPIOA, &gpio_init_struct);
		__HAL_AFIO_REMAP_TIM1_PARTIAL();

	}
}

void Stop_Start_Init(void)
{
	GPIO_InitTypeDef gpio_init_struct;
	__HAL_RCC_GPIOF_CLK_ENABLE();
	
	gpio_init_struct.Pin = GPIO_PIN_5;
	gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_init_struct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIO_LBridge, &gpio_init_struct);
}

void TIM_Init(void)
{
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_TIM4_CLK_ENABLE();
	
	TIM2_Handler.Instance = TIM2;
	TIM2_Handler.Init.Prescaler = 3600-1;
	TIM2_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	
	TIM2_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM2_Handler.Init.Period = 10-1; //1ms
	
	
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 3600-1;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 20-1; //2ms采样一次转速
	
	HAL_TIM_Base_Init(&TIM2_Handler); 
	HAL_TIM_Base_Init(&htim4); 
	
	HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	HAL_NVIC_SetPriority(TIM4_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);
	
	HAL_TIM_Base_Start_IT(&TIM2_Handler); 
	HAL_TIM_Base_Start_IT(&htim4); 
}


/**************************************************************************************

adc初始化函数
日期：2024.8.19

**************************************************************************************/
void adc_init(void)
{
	ADC_ChannelConfTypeDef Sconfig = {0};
	AdcHandle.Instance = ADC_MEASURE;
	AdcHandle.Init.ContinuousConvMode = ENABLE;
	AdcHandle.Init.NbrOfConversion = ADC_CH_NUM;
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;
	
	AdcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	AdcHandle.Init.ScanConvMode = ADC_SCAN_ENABLE;
	HAL_ADC_Init(&AdcHandle);
	
	Sconfig.Channel = ADC_CHANNEL_0;
	Sconfig.Rank = ADC_REGULAR_RANK_1;
	Sconfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	HAL_ADC_ConfigChannel(&AdcHandle, &Sconfig);
	
	Sconfig.Channel = ADC_CHANNEL_3;
	Sconfig.Rank = ADC_REGULAR_RANK_2;
	HAL_ADC_ConfigChannel(&AdcHandle, &Sconfig);
	
	Sconfig.Channel = ADC_CHANNEL_5;
	Sconfig.Rank = ADC_REGULAR_RANK_3;
	HAL_ADC_ConfigChannel(&AdcHandle, &Sconfig);
	
	Sconfig.Channel = ADC_CHANNEL_7;
	Sconfig.Rank = ADC_REGULAR_RANK_4;
	HAL_ADC_ConfigChannel(&AdcHandle, &Sconfig);
	
	Sconfig.Channel = ADC_CHANNEL_9;
	Sconfig.Rank = ADC_REGULAR_RANK_5;
	HAL_ADC_ConfigChannel(&AdcHandle, &Sconfig);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
	GPIO_InitTypeDef gpio_init_struct;
	DMA_HandleTypeDef DMAHandle;
	__HAL_RCC_ADC1_CLK_ENABLE();
	
	gpio_init_struct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8;
	gpio_init_struct.Mode = GPIO_MODE_ANALOG;
	gpio_init_struct.Pull = GPIO_NOPULL;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &gpio_init_struct);
	//HAL_ADC_Start(hadc);
	
	__HAL_RCC_DMA1_CLK_ENABLE();
	DMAHandle.Init.Direction = DMA_PERIPH_TO_MEMORY;
	DMAHandle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	DMAHandle.Init.MemInc = DMA_MINC_ENABLE;
	DMAHandle.Init.PeriphInc = DMA_PINC_DISABLE;
	DMAHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	DMAHandle.Init.Mode = DMA_CIRCULAR;
	DMAHandle.Init.Priority = DMA_PRIORITY_MEDIUM;
	HAL_DMA_Init(&DMAHandle);
	
	__HAL_LINKDMA(&AdcHandle,DMA_Handle,DMAHandle);
	
	HAL_NVIC_SetPriority(ADC1_2_IRQn, 2, 1);
	HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

	HAL_ADC_Start_DMA(&AdcHandle, (uint32_t *)g_adc_raw, ADC_CH_NUM*ADC_MEM_NUM);
}

void capture_init(void)
{
	Cap_Tim_Init();

}

void Cap_Tim_Init(void)     //引脚映射到PA6
{
	//TIM_SlaveConfigTypeDef tim_slave_config = {0};
	TIM_Encoder_InitTypeDef sConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	__HAL_RCC_TIM3_CLK_ENABLE(); 
	
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 0;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 65535;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
	sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 0;
	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 0;
	
	HAL_TIM_Encoder_Init(&htim3, &sConfig);
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	
	HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);
}

void sys_init(void)
{
	HAL_Init();
	h_bridge_init();
	l_bridge_init();
	H_sensor_init();
	Stop_Start_Init();
	TIM_Init();
	capture_init();
	sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);
    usart_init(115200);
	gtimRestart();
	MX_SPI1_Init();
}
