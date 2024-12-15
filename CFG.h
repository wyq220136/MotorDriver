#ifndef CFG_H
#define CFG_H
#include "stm32f1xx_hal.h"
#include "init.h"
#include "bldc.h"
#include "motor_ctrl.h"
#include "measure.h"
#include "comm.h"
#include "foc.h"
#include "encode.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"

#define TIMPSC 71
#define TIMARR 499
#define TIMPSC2 35
#define RADIUS 0.04//车轮半径
#define PI 3.141592654
#define SQRT3 1.732051

//霍尔信号采集引脚相关设置
#define GPIO_HALLSENSOR  GPIOB
#define PINHALLU GPIO_PIN_14
#define PINHALLV GPIO_PIN_13
#define PINHALLW GPIO_PIN_12

//下桥臂配置相关
#define GPIO_LBridge GPIOB
#define LBU GPIO_PIN_0
#define LBV GPIO_PIN_1
#define LBW GPIO_PIN_2
#define LBPIN_CLKON __HAL_RCC_GPIOB_CLK_ENABLE()

//上桥臂PWM发波相关设置
#define GPIO_HBridge GPIOA
#define PINPWMU GPIO_PIN_11
#define PINPWMV GPIO_PIN_9
#define PINPWMW GPIO_PIN_10
#define PWMPIN_CLKON __HAL_RCC_GPIOA_CLK_ENABLE()
#define PWMTIM TIM1
#define PWMTIM_CLKON __HAL_RCC_TIM1_CLK_ENABLE()
#define PWM_REMAP __HAL_AFIO_REMAP_TIM1_PARTIAL()

#define __Constrain(x) x>0.375?0.375:(x<-0.375?-0.375:x)

//下桥臂开关函数
#define UL_ON  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET)//A相开启
#define VL_ON  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET)//B相开启
#define WL_ON  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET)
#define UL_OFF  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET)
#define VL_OFF  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET)
#define WL_OFF  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET)

#define SHUTOFF HAL_GPIO_WritePin(GPIOF, GPIO_PIN_5, GPIO_PIN_RESET)
#define SHUTON   HAL_GPIO_WritePin(GPIOF, GPIO_PIN_5, GPIO_PIN_SET)

/*********************************************************************
电机启停状况和转动方向参数宏定义
**********************************************************************/
#define START 1
#define STOP 0

#define FORWARD 1
#define BACKWARD 0

#define ADC_MEASURE ADC1
#define ADC_CH_NUM 3
#define ADC_MEM_NUM 50
#define ADC_COLL 3
#define ADC_GPIOPIN1 GPIO_PIN_1
#define ADC_GPIOPIN2 GPIO_PIN_2
#define ADC_GPIOPIN3 GPIO_PIN_3

/**********************************************************************************
电机电学参数
**********************************************************************************/
#define V_SOURCE 24  //驱动板最大供电
#define V_MAX 18 //电机最大耐受电压
#define POLE_NUM 8 //maxon极对数
#define ENC_OLD 51 //老电机一圈码数
#define THETA_MAX 45 //最大回正力矩对应的角度
#define V_OFFSET 0.12//防止FOC陷入死区，零点加入偏置
//#define KE 24 //电机反电动势常数
#define PROPA_PARAM 8 //相电压采样比

#define SAMPLETIME 0.002//2ms读取一次电机转速
#define ENATURE 2.718282//自然对数
#define OMEGAC 50//adrc观测器带宽
#define RADRC 20//ADRC跟踪因子，越大跟踪越快，越小超调幅度越小
#define BADRC 0.01//控制器增益
#define ADRCLIMIT 150//ADRC输出限幅
#define KPADRC 500
#define KDADRC 45

#define ENCODE_PORT GPIOA
#define PHASE_A GPIO_PIN_6
#define PHASE_B GPIO_PIN_7

#define BRAKE_PORT GPIOA
#define BRAKE_PIN GPIO_PIN_5

typedef void(*pctr) (void);

#endif
