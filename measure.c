#include "cfg.h"

/*********************************************************************
2024.8.17 负责人：wyq
本文件用于模拟信号采集和测量，控制电流，电压和温度
*********************************************************************/
uint16_t g_adc_raw[ADC_CH_NUM*ADC_MEM_NUM];
float g_adc_ave[ADC_CH_NUM];
uint16_t phase_offset[ADC_COLL] = {0};
uint8_t g_adc_dma_sta = 0;

extern motor_ctrl motor;
extern ADC_HandleTypeDef    AdcHandle;
extern pulse_volt volt_out;
extern adrc Adrc1, Adrc2, Adrc3;
extern Foc motor_foc;

void adc_ave_filter(void)
{
	uint8_t i = 0, j = 0;
	float temp;
	uint32_t tmp[ADC_CH_NUM]={0};
	for(i = 0;i<ADC_CH_NUM;i++)
	{
		for(j = 0; j<ADC_MEM_NUM;j++)
		{
			tmp[i] += g_adc_raw[i+j*ADC_CH_NUM];
		}
		temp = (float)tmp[i]/ADC_MEM_NUM*PROPA_PARAM*3/4095;
		g_adc_ave[i] = temp;
	}
}

//电机停止状态下调用，获得相电压零漂值
void adc_offset(void)
{
	uint8_t i = 0, j = 0;
	uint32_t tmp[ADC_COLL]={0};
	for(i = ADC_COLL-1;i<ADC_CH_NUM;i++)
	{
		for(j = 0; j<ADC_MEM_NUM;j++)
		{
			tmp[i] += g_adc_raw[i+j*ADC_CH_NUM];
		}
		phase_offset[i] = tmp[i]/ADC_MEM_NUM;
	}
}

/**************************************************************************************************
DMA中断服务函数，用于采集模拟信号量（电流，电压，温度）
**************************************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */
	if (( DMA1->ISR & (1 << 1) ))
    {
        g_adc_dma_sta = 1;
        do{ DMA1->IFCR |= 1 << 1; }while(0);
    }
}
