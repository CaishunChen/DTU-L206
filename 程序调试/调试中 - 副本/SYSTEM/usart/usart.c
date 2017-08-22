#include "sys.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////



//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;

};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    while((USART1->SR & 0X40) == 0); //循环发送,直到发送完毕
    USART1->DR = (u8) ch;
    return ch;
}
#endif

/*使用microLib的方法*/
/*
int fputc(int ch, FILE *f)
{
USART_SendData(USART1, (uint8_t) ch);

while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}

   return ch;
}
int GetKey (void)  {

   while (!(USART1->SR & USART_FLAG_RXNE));

   return ((int)(USART1->DR & 0x1FF));
}
*/




//接收状态
//bit15，	接收完成标志
//bit14~0，	接收到的有效字节数目
vu16 USART_SERIAL_RX_STA = 0;	//接收状态标记
vu16 USART_SERIAL_RX_LEN = 0;	//接收长度
//接收缓冲,最大USART_SERIAL_REC_LEN个字节.
vu8 USART_SERIAL_RX_BUF[USART_SERIAL_REC_LEN];   

/*******************************************************************************
* Function Name  : 
* Description    : 终端串口初始化
* Input					 : bound：波特率
* Output         : None
* Return         : None
*******************************************************************************/
void uart_init_serial(u32 bound) {
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(DEVICE_SERIAL_RCCDEVICE | DEVICE_SERIAL_RCCPORT, ENABLE);	//使能USART1，GPIOA时钟

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = DEVICE_SERIAL_TX_PIN; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(DEVICE_SERIAL_PORT, &GPIO_InitStructure);//初始化GPIOA.9

    //USART1_RX	  GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = DEVICE_SERIAL_RX_PIN;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(DEVICE_SERIAL_PORT, &GPIO_InitStructure);//初始化GPIOA.10

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = DEVICE_SERIAL_IRQN;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DEVICE_SERIAL_PRE_PRIORITY ; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DEVICE_SERIAL_SUB_PRIORITY;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(DEVICE_SERIAL_DEVICE, &USART_InitStructure); //初始化串口1
    USART_ITConfig(DEVICE_SERIAL_DEVICE, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(DEVICE_SERIAL_DEVICE, ENABLE);                    //使能串口1
}




//接收状态
//bit15，	接收完成标志
//bit14~0，	接收到的有效字节数目
vu16 USART_NETWORK_RX_STA = 0;	//接收状态标记
vu16 USART_NETWORK_RX_LEN = 0;	//接收长度
//接收缓冲,最大USART_SERIAL_REC_LEN个字节.
vu8 USART_NETWORK_RX_BUF[USART_NETWORK_REC_LEN];  
/*******************************************************************************
* Function Name  : 
* Description    : 网络端串口初始化
* Input					 : bound：波特率
* Output         : None
* Return         : None
*******************************************************************************/
void uart_init_network(u32 bound)
{
		 //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(DEVICE_NETWORK_RCCDEVICE , ENABLE);	//使能USART1，GPIOA时钟
    RCC_APB2PeriphClockCmd(DEVICE_NETWORK_RCCPORT, ENABLE);	//使能USART1，GPIOA时钟

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = DEVICE_NETWORK_TX_PIN; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(DEVICE_NETWORK_PORT, &GPIO_InitStructure);//初始化GPIOA.9

    //USART1_RX	  GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = DEVICE_NETWORK_RX_PIN;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(DEVICE_NETWORK_PORT, &GPIO_InitStructure);//初始化GPIOA.10

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = DEVICE_NETWORK_IRQN;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DEVICE_SERIAL_SUB_PRIORITY; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DEVICE_NETWORK_SUB_PRIORITY;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(DEVICE_NETWORK_DEVICE, &USART_InitStructure); //初始化串口1
    USART_ITConfig(DEVICE_NETWORK_DEVICE, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(DEVICE_NETWORK_DEVICE, ENABLE);                    //使能串口1
}




/*******************************************************************************
* Function Name  : 
* Description    : 终端串口数据发送
* Input          : Pdata：数据指针	len：数据长度
* Output         : None
* Return         : None
*******************************************************************************/
void UsartSendStr_serial(u8 volatile *Pdata, u8 len)
{
    u16 i;
	//		
	/*发送数据的第一位会丢失，故在这里将第一位数据发送两次*/
	for(i = 0; i < len; i++)
	{
		USART_ClearFlag(DEVICE_SERIAL_DEVICE, USART_FLAG_TC);	//清除中断标志位,保证第一个字节的数据不丢失
		USART_SendData(DEVICE_SERIAL_DEVICE, Pdata[i]);//向串口1发送数据
		while(USART_GetFlagStatus(DEVICE_SERIAL_DEVICE, USART_FLAG_TC) != SET); //等待发送结束
	}
}

/*******************************************************************************
* Function Name  : 
* Description    : 网络端串口数据发送
* Input          : Pdata：数据指针	len：数据长度
* Output         : None
* Return         : None
*******************************************************************************/
void UsartSendStr_network(u8 volatile *Pdata, u8 len)
{
    u16 i;
	//		
	/*发送数据的第一位会丢失，故在这里将第一位数据发送两次*/
	for(i = 0; i < len; i++)
	{
		USART_ClearFlag(DEVICE_NETWORK_DEVICE, USART_FLAG_TC);	//清除中断标志位,保证第一个字节的数据不丢失
		USART_SendData(DEVICE_NETWORK_DEVICE, Pdata[i]);//向串口1发送数据
		while(USART_GetFlagStatus(DEVICE_NETWORK_DEVICE, USART_FLAG_TC) != SET); //等待发送结束
	}
}








