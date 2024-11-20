#include "encode.h"
#include "cfg.h"

SPI_HandleTypeDef hspi1;
float angle = 0.0;
extern float last_angle;

void MX_SPI1_Init(void)
{
  /* SPI1 parameter configuration*/
	__HAL_RCC_SPI1_CLK_ENABLE();
	   // __HAL_RCC_SPI1_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	hspi1.Instance = SPI1;
	  hspi1.Init.Mode = SPI_MODE_MASTER;
	  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	  hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
	  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	  hspi1.Init.NSS = SPI_NSS_SOFT;
	  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	  hspi1.Init.CRCPolynomial = 7;
		HAL_SPI_Init(&hspi1);
	
	//__HAL_AFIO_REMAP_SPI1_ENABLE();
	
	GPIO_InitTypeDef gpio_init_struct = {0};
	gpio_init_struct.Pin = GPIO_PIN_4;
	gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_init_struct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &gpio_init_struct);
	
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hspi->Instance==SPI1)
  {

	GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	  
	//HAL_NVIC_SetPriority(SPI1_IRQn, 1, 0);
    //HAL_NVIC_EnableIRQ(SPI1_IRQn);
  }

}

//奇偶校验函数
uint16_t Parity_bit_Calculate(uint16_t data_2_cal)
{
	uint16_t parity_bit_value=0;
	while(data_2_cal != 0)
	{
		parity_bit_value ^= data_2_cal; 
		data_2_cal >>=1;
	}
	return (parity_bit_value & 0x1); 
}
//SPI发送读取函数
uint16_t SPI_ReadWrite_OneByte(uint16_t _txdata)
{

	AS5407P_CS_L;
	uint16_t rxdata;
	if(HAL_SPI_TransmitReceive(&hspi1,(uint8_t *)&_txdata,(uint8_t *)&rxdata,1,1000) !=HAL_OK)
		rxdata=0;
  AS5407P_CS_H;
	return rxdata;
}
uint16_t AS5047_read(uint16_t add)
{
	uint16_t data;
	add |= 0x4000;
	if(Parity_bit_Calculate(add)==1) add=add|0x8000; 
	SPI_ReadWrite_OneByte(add);
		data=SPI_ReadWrite_OneByte(NOP|0x4000); 
	data &=0x3fff;
	return data;
}

void getAngle(void)
{
	last_angle = angle;
	angle=(float)(AS5047_read(ANGLEUNC));
	angle = LowPass(angle);
	angle = angle/16384*360;
}
