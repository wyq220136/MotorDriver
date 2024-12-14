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
TIM_HandleTypeDef htim3, htim4, htim5;
ADC_HandleTypeDef    AdcHandle;
DMA_HandleTypeDef    DMAHandle;
extern motor_ctrl motor;
extern uint16_t g_adc_raw[ADC_CH_NUM*ADC_MEM_NUM];


void h_bridge_init(void)//上桥臂初始化，控制方式采用h_pwm_l_on
{
	uint16_t psc = TIMPSC;
	uint16_t arr = TIMARR;
	TIM1_Handler.Instance=PWMTIM;          	
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
	LBPIN_CLKON;
	
	gpio_init_struct.Pin = LBU|LBV|LBW;
	gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_init_struct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIO_LBridge, &gpio_init_struct);
}

void H_sensor_init(void)				//霍尔信号采集引脚初始化
{
	GPIO_InitTypeDef gpio_init_struct;
	
	gpio_init_struct.Pin = PINHALLU|PINHALLV|PINHALLW;
	gpio_init_struct.Mode = GPIO_MODE_INPUT;
	gpio_init_struct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIO_HALLSENSOR, &gpio_init_struct);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)   //PWM回调函数，用于IO复用配置
{
	if(htim->Instance == PWMTIM)
	{
		GPIO_InitTypeDef gpio_init_struct;
		PWMPIN_CLKON;
		PWMTIM_CLKON;
		
		gpio_init_struct.Pin = PINPWMU | PINPWMV | PINPWMW;
		gpio_init_struct.Mode = GPIO_MODE_AF_PP;
		gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
		gpio_init_struct.Pull = GPIO_PULLUP;
		
		HAL_GPIO_Init(GPIO_HBridge, &gpio_init_struct);
		PWM_REMAP;

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
	__HAL_RCC_TIM5_CLK_ENABLE();
	
	TIM2_Handler.Instance = TIM2;
	TIM2_Handler.Init.Prescaler = 36-1;
	TIM2_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TIM2_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM2_Handler.Init.Period = 10-1; //5us为最小计数单位
	
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 720-1;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 100-1; //1ms计算一次PID和ADRC
	
	htim5.Instance = TIM5;
	htim5.Init.Prescaler = 72-1;
	htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim5.Init.Period = 40-1; //40us采样一次位置
	
	HAL_TIM_Base_Init(&TIM2_Handler); 
	HAL_TIM_Base_Init(&htim4);
	HAL_TIM_Base_Init(&htim5);
	
	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	HAL_NVIC_SetPriority(TIM4_IRQn, 3, 1);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);
	HAL_NVIC_SetPriority(TIM5_IRQn, 2, 1);
	HAL_NVIC_EnableIRQ(TIM5_IRQn);
	
	HAL_TIM_Base_Start_IT(&TIM2_Handler); 
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_Base_Start_IT(&htim5);
}


/**************************************************************************************

adc初始化函数
日期：2024.8.19

**************************************************************************************/
void adc_dma_init(uint32_t mar)
{
    GPIO_InitTypeDef gpio_init_struct;
    RCC_PeriphCLKInitTypeDef adc_clk_init = {0};
    ADC_ChannelConfTypeDef adc_ch_conf = {0};

    __HAL_RCC_ADC1_CLK_ENABLE();                                              /* ʹŜADCxʱד */

    if ((uint32_t)DMA1_Channel1 > (uint32_t)DMA1_Channel7)                 /* ճԚDMA1_Channel7, ղΪDMA2քͨր */
    {
        __HAL_RCC_DMA2_CLK_ENABLE();                                        /* DMA2ʱדʹŜ */
    }
    else
    {
        __HAL_RCC_DMA1_CLK_ENABLE();                                        /* DMA1ʱדʹŜ */
    }

    adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);

    gpio_init_struct.Pin = ADC_GPIOPIN1|ADC_GPIOPIN2|ADC_GPIOPIN3;
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);

    DMAHandle.Instance = DMA1_Channel1;                             /* ʨ׃DMAͨր */
    DMAHandle.Init.Direction = DMA_PERIPH_TO_MEMORY;                 /* Փ΢ʨսզԢǷģʽ */
    DMAHandle.Init.PeriphInc = DMA_PINC_DISABLE;                     /* ΢ʨ؇նģʽ */
    DMAHandle.Init.MemInc = DMA_MINC_ENABLE;                         /* զԢǷնģʽ */
    DMAHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    /* ΢ʨ˽ߝӤ׈:16λ */
    DMAHandle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;       /* զԢǷ˽ߝӤ׈:16λ */
    DMAHandle.Init.Mode = DMA_NORMAL;
    DMAHandle.Init.Priority = DMA_PRIORITY_MEDIUM;
    HAL_DMA_Init(&DMAHandle);

    __HAL_LINKDMA(&AdcHandle, DMA_Handle, DMAHandle);

    AdcHandle.Instance = ADC_MEASURE;
    AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    AdcHandle.Init.ScanConvMode = ADC_SCAN_ENABLE;
    AdcHandle.Init.ContinuousConvMode = ENABLE;
    AdcHandle.Init.NbrOfConversion = 3;                         
    AdcHandle.Init.DiscontinuousConvMode = DISABLE;
    AdcHandle.Init.NbrOfDiscConversion = 0;
    AdcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    HAL_ADC_Init(&AdcHandle);

    HAL_ADCEx_Calibration_Start(&AdcHandle);

    adc_ch_conf.Channel = ADC_CHANNEL_1;
    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&AdcHandle, &adc_ch_conf);
	
	adc_ch_conf.Channel = ADC_CHANNEL_2;
	adc_ch_conf.Rank = ADC_REGULAR_RANK_2;
	HAL_ADC_ConfigChannel(&AdcHandle, &adc_ch_conf);
	
	adc_ch_conf.Channel = ADC_CHANNEL_3;
	adc_ch_conf.Rank = ADC_REGULAR_RANK_3;
	HAL_ADC_ConfigChannel(&AdcHandle, &adc_ch_conf);
	

    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 3, 3);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    HAL_DMA_Start_IT(&DMAHandle, (uint32_t)&ADC1->DR, mar, ADC_CH_NUM*ADC_MEM_NUM);
    HAL_ADC_Start_DMA(&AdcHandle, &mar, ADC_CH_NUM*ADC_MEM_NUM);
}

void adc_dma_enable(uint16_t cndtr)
{
    ADC_MEASURE->CR2 &= ~(1 << 0);

    DMA1_Channel1->CCR &= ~(1 << 0);
    while (DMA1_Channel1->CCR & (1 << 0));
    DMA1_Channel1->CNDTR = cndtr;
    DMA1_Channel1->CCR |= 1 << 0;

    ADC_MEASURE->CR2 |= 1 << 0;
    ADC_MEASURE->CR2 |= 1 << 22;
}

/****************************************************************************************************
以下为霍尔传感器信号输入捕获 2024.10.4
****************************************************************************************************/
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

void rcnt(void) //初始化中断引脚，转一整圈触发一次
{
	GPIO_InitTypeDef gpio_init_struct;

	gpio_init_struct.Pin = GPIO_PIN_6;
	gpio_init_struct.Mode = GPIO_MODE_IT_RISING;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_init_struct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIO_LBridge, &gpio_init_struct);
	
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void HAL_MspInit(void)
{

	__HAL_RCC_AFIO_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
	
	__HAL_AFIO_REMAP_SWJ_DISABLE();
}

void sys_init(void)
{
	HAL_Init();
	h_bridge_init();
	l_bridge_init();
	H_sensor_init();
	Stop_Start_Init();
	TIM_Init();
	sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);
    usart_init(115200);
	ctrl_Init();
	adc_dma_init((uint32_t)&g_adc_raw);
	adc_dma_enable(ADC_CH_NUM*ADC_MEM_NUM);
	Cap_Tim_Init();
	rcnt();
	
}
