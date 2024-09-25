#include "spi.h"

void SPI_Init(void) {
    // 启用时钟
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;  // GPIOA时钟
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;  // SPI1时钟

    // 配置SPI引脚
    GPIOA->CRL &= ~(GPIO_CRL_CNF5 | GPIO_CRL_CNF6 | GPIO_CRL_CNF7); // 清除配置
    GPIOA->CRL |= (GPIO_CRL_MODE5_0 | GPIO_CRL_MODE5_1); // SCK (PA5) 推挽输出
    GPIOA->CRL |= (GPIO_CRL_MODE7_0 | GPIO_CRL_MODE7_1); // MOSI (PA7) 推挽输出
    GPIOA->CRL |= GPIO_CRL_CNF6_1;                          // MISO (PA6) 输入浮空

    // 配置SPI
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_BR_0; // 主模式，时钟分频
    SPI1->CR1 |= SPI_CR1_SPE; // 使能SPI
}

uint8_t SPI_Transmit(uint8_t data) {
    while (!(SPI1->SR & SPI_SR_TXE)); // 等待发送缓冲区空
    SPI1->DR = data; // 发送数据
    while (!(SPI1->SR & SPI_SR_RXNE)); // 等待接收缓冲区非空
    return SPI1->DR; // 读取接收数据
}

uint8_t SPI_Receive(void) {
    while (!(SPI1->SR & SPI_SR_RXNE)); // 等待接收缓冲区非空
    return SPI1->DR; // 读取接收数据
}
