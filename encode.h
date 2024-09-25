#ifndef ENCODE_H
#define ENCODE_H
#include "cfg.h"

#define AS5407P_CS_L HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET)
#define AS5407P_CS_H HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET)

#define NOP 0x0000
#define ANGLEUNC 0x3FFE

void MX_SPI1_Init(void);
uint16_t Parity_bit_Calculate(uint16_t data_2_cal);
uint16_t SPI_ReadWrite_OneByte(uint16_t _txdata);
uint16_t AS5047_read(uint16_t add);

#endif
