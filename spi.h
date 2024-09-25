#ifndef SPI_H
#define SPI_H

#include "stm32f1xx.h"

void SPI_Init(void);
uint8_t SPI_Transmit(uint8_t data);
uint8_t SPI_Receive(void);

#endif // SPI_H
