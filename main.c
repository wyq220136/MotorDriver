#include "main.h"
#include "cfg.h"

extern motor_ctrl motor;
extern TIM_HandleTypeDef htim3;
volatile uint16_t ecnt = 0;
int main(void)
{
	sys_init();
	motor.pulsea = (TIMARR+1)/2;
	motor.pulseb = (TIMARR+1)/2;
	motor.pulsec = (TIMARR+1)/2;
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);

    //uint8_t len;
    //uint16_t times = 0;
	motor.dir = FORWARD;
	motor.run_flag = START;
    while (1)
    {
		//ecnt = __HAL_TIM_GET_COUNTER(&htim3);
        /*if (g_usart_rx_sta & 0x8000)      
        {
            len = g_usart_rx_sta & 0x3fff;  
            printf("\r\nGoodMorning!\r\n");

            HAL_UART_Transmit(&g_uart1_handle,(uint8_t*)g_usart_rx_buf, len, 1000);  
            while(__HAL_UART_GET_FLAG(&g_uart1_handle,UART_FLAG_TC) != SET);           
            printf("\r\n\r\n");           
            g_usart_rx_sta = 0;
        }
        else
        {
            times++;

            if (times % 5000 == 0)
            {
                printf("\r\nHello\r\n");
                printf("Ican\r\n\r\n\r\n");
            }

            if (times % 200 == 0)printf("Ohyes\r\n");

            delay_ms(10);
        }*/
    }
}



