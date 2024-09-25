#ifndef BLDC_H
#define BLDC_H
#include "stm32f1xx_hal.h"
/******************************************
定义电机驱动结构体，用于设置PWM输出脉宽和电机转动方向担后续可以添加更多参数
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

uint8_t hallsensor(void);

void uhvl(void);
void uhwl(void);
void vhul(void);
void vhwl(void);
void whul(void);
void whvl(void);
void Stop_motor(void);
void Start_motor(void);
uint8_t Is_Forward(void);
void getEncode(uint8_t k);
void gtimRestart(void);

#endif
