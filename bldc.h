#ifndef BLDC_H
#define BLDC_H
#include "stm32f1xx_hal.h"
/******************************************
电机基本驱动配置文件
******************************************/
typedef struct
{
	__IO uint16_t pulsea;
	__IO uint16_t pulseb;
	__IO uint16_t pulsec;
	__IO uint8_t dir;
	__IO uint8_t step_now;
	__IO uint8_t step_last;
	__IO uint8_t run_flag;
}motor_ctrl;

void phase_zero(void);
void phase_one(void);
void phase_two(void);
void phase_three(void);
void phase_four(void);
void phase_five(void);
void Stop_motor(void);
uint8_t Is_Forward(void);
void getEncode(uint8_t k);
void gtimRestart(void);

#endif
