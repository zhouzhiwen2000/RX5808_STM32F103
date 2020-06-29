/******************** (C) COPYRIGHT 2012 **************************
 * 文件名  ：main.c
 * 描述    ：LED流水灯，频率可调……
 * 实验平台：STM32开发板
 * 库版本  ：ST3.5.0
 *
**********************************************************************************/
#include "stm32f10x.h"
#include "delay.h"
#include "RX5808.h"
/*
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  ：无
 */
int a=5000;
int main(void)
{
    /* LED 端口初始化 */
    GPIO_Config();
    Adc_Init();
    delay_init();
	  //Channel_Scan();
		RX5808_Set_Freq(5705);
    while (1)
    {

    }
}


/******************* (C) COPYRIGHT 2012 *****END OF FILE************/
