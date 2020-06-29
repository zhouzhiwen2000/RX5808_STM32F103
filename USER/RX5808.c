#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "adc.h"
#define Synthesizer_Register_B 				0x01  //频率设置寄存器
#define Power_Down_Control_Register   0x0a  //电源控制寄存器
#define State_Register                0x0f  //状态控制寄存器
#define  SCK PAout(5)
#define  MOSI PAout(6)
#define  LE PAout(7)
void Delay(__IO u32 nCount);
void Delay(__IO u32 nCount)	 //简单的延时函数
{
    for(; nCount != 0; nCount--);
}
void GPIO_Config(void)
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*开启GPIOC的外设时钟*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);

    /*选择要控制的GPIOC引脚*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7  ;

    /*设置引脚模式为通用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    /*设置引脚速率为50MHz */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /*调用库函数，初始化GPIOC*/
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 关闭所有led灯	*/
    GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
}

uint16_t Freq_Buff[48]=
{
    5865,5845,5825,5805,5785,5765,5745,5725,		//A	CH1-8
    5733,5752,5771,5790,5809,5828,5847,5866,		//B	CH1-8
    5705,5685,5665,5645,5885,5905,5925,5945,		//C	CH1-8
    5740,5760,5780,5800,5820,5840,5860,5880,		//D	CH1-8
    5658,5695,5732,5769,5806,5843,5880,5917,		//E	CH1-8
    5362,5399,5436,5473,5510,5547,5584,5621,		//F	CH1-8
};

uint8_t channel=0;
//spi发送一位数据
void SPI_Send_Bit(uint8_t Value)
{
    SCK =0;
    Delay(5);
    MOSI=Value;
    Delay(5);
    SCK =1;
    Delay(5);
}

//spi发送长度为len位数据
//根据时序图，要求先发送数据低位
void SPI_Send_Bits(u32 Value,u8 len)
{
    u8 i;
    for(i=0; i<len; i++)
    {
        SPI_Send_Bit(Value&0x01);
        Value>>=1;
    }
}

void Send_Register_Data(u8 addr,u32 Dat)   //向地址为addr寄存器发送数据
{
    LE=0;                   //相当于CS，低电平选中芯片，允许传输数据
    SPI_Send_Bits(addr,4);  //先发送寄存器4位地址
    SPI_Send_Bit(1);        //允许写入
    SPI_Send_Bits(Dat,20);  //发送20位控制数据

    //一帧数据传输完成
    LE=1;
    SCK=0;
    MOSI=0;
}

void RX5808_Set_Freq(uint16_t F_rf)   //RX5808设置一个频率
{
    uint32_t dat;
    uint16_t F_lo=(F_rf-479)/2;
    uint16_t SYN_RF_N;
    uint16_t SYN_RF_A;

    SYN_RF_N=F_lo/32;    //计算此频率下N值
    SYN_RF_A=F_lo%32;    //计算此频率下A值

    dat=SYN_RF_N;
    dat<<=7;
    dat|=SYN_RF_A;       //得到20位控制数据

    //设置频率控制寄存器
    Send_Register_Data(Synthesizer_Register_B,dat);
}

void Channel_Scan()  //自动搜频
{
    u8 i;
    u8 sig;
    u8 sig_max=0;

    for(i=0; i<48; i++)
    {
        RX5808_Set_Freq(Freq_Buff[i]);
        delay_ms(15);
        sig=Get_Adc_Average(ADC_Channel_1,10);
        if(sig>sig_max)sig_max=sig,channel=i;
    }
    RX5808_Set_Freq(Freq_Buff[channel]);    //设置频率
}
