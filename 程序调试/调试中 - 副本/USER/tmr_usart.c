
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "tmr_usart.h"



//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器4
void tmr_init_serial(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//
	RCC_APB1PeriphClockCmd(SERIAL_TMR_RCCDEVICE,ENABLE);  ///使能SERIAL_TMR_DEVICE时钟
	//
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(SERIAL_TMR_DEVICE,&TIM_TimeBaseInitStructure);//初始化SERIAL_TMR_DEVICE
	//
	NVIC_InitStructure.NVIC_IRQChannel=SERIAL_TMR_IRQN; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=SERIAL_TMR_PRE_PRIORITY; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=SERIAL_TMR_SUB_PRIORITY; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//
	TIM_ClearITPendingBit( SERIAL_TMR_DEVICE,TIM_IT_Update) ;
	TIM_ITConfig(SERIAL_TMR_DEVICE,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	//
	TIM_Cmd(SERIAL_TMR_DEVICE,DISABLE);//等待串口接收首字节开启
	
}


//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器4
void tmr_init_network(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//
	RCC_APB1PeriphClockCmd(NETWORK_TMR_RCCDEVICE,ENABLE);  ///使能TIM3时钟
	//
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(NETWORK_TMR_DEVICE,&TIM_TimeBaseInitStructure);//初始化TIM3
	//
	NVIC_InitStructure.NVIC_IRQChannel=NETWORK_TMR_IRQN; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=NETWORK_TMR_PRE_PRIORITY; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=NETWORK_TMR_SUB_PRIORITY; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//
	TIM_ClearITPendingBit( NETWORK_TMR_DEVICE,TIM_IT_Update) ;
	TIM_ITConfig(NETWORK_TMR_DEVICE,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	//
	TIM_Cmd(NETWORK_TMR_DEVICE,DISABLE);//等待串口接收首字节开启
	
}















