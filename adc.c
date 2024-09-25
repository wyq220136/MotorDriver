#include "adc.h"

ADC_HandleTypeDef hadc;

void ADC_Config(void) {
    // 初始化时钟
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 配置GPIO引脚
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_PIN_0;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置ADC
    hadc.Instance = ADC1;
    hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;  // 扫描模式
    hadc.Init.ContinuousConvMode = ENABLE;       // 连续转换模式
    hadc.Init.DiscontinuousConvMode = DISABLE;    // 非连续模式
    hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START; // 软件触发
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;    // 数据对齐
    hadc.Init.NbrOfConversion = 1;                 // 转换通道数
    // 分辨率使用默认值，通常为12位
    HAL_ADC_Init(&hadc);

    // 启动ADC
    HAL_ADC_Start(&hadc);
}

uint16_t Read_ADC(void) {
    // 开始转换
    HAL_ADC_Start(&hadc);
    // 等待转换完成
    HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
    // 获取ADC值
    return HAL_ADC_GetValue(&hadc);
}
