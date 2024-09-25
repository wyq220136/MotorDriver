#ifndef INIT_H
#define INIT_H
#include "stm32f1xx_hal.h"

void h_bridge_init(void);
void l_bridge_init(void);
void H_sensor_init(void);
void Stop_Start_Init(void);
void TIM_Init(void);
void sys_init(void);
void Cap_Tim_Init(void);
void capture_init(void);

#endif
