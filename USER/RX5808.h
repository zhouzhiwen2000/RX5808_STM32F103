#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "adc.h"
void GPIO_Config(void);
void RX5808_Set_Freq(uint16_t F_rf);   //RX5808设置一个频率
void Channel_Scan(void);  //自动搜频

