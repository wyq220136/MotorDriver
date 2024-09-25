// adc.h
#ifndef __ADC_H
#define __ADC_H

#include "stm32f1xx.h"

void ADC_Configuration(void);
uint16_t ADC_ReadChannel(uint8_t channel);

#endif // __ADC_H

