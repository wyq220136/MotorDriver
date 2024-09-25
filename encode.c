#include "encode.h"
#include "cfg.h"

SPI_HandleTypeDef hspi1;
void MX_SPI1_Init(void)
{
  /* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	HAL_SPI_Init(&hspi1);
	
	
	GPIO_InitTypeDef gpio_init_struct = {0};
	gpio_init_struct.Pin = GPIO_PIN_4;
	gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_init_struct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &gpio_init_struct);
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

	AS5407P_CS_L;	//cs拉低
	uint16_t rxdata;
	if(HAL_SPI_TransmitReceive(&hspi1,(uint8_t *)&_txdata,(uint8_t *)&rxdata,1,1000) !=HAL_OK)
		rxdata=0;
	AS5407P_CS_H;		//cs拉高
	return rxdata;
}
uint16_t AS5047_read(uint16_t add)
{
	uint16_t data;
	add |= 0x4000;	//读指令 bit14 置1
	if(Parity_bit_Calculate(add)==1) add=add|0x8000; //如果前15位 1的个数位偶数，则Bit15 置1
	SPI_ReadWrite_OneByte(add);		//发送一条指令，不管读回的数据
		data=SPI_ReadWrite_OneByte(NOP|0x4000); //发送一条空指令，读取上一次指令返回的数据。
	data &=0x3fff;
	return data;
}
