
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "tmr_usart.h"



//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��4
void tmr_init_serial(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//
	RCC_APB1PeriphClockCmd(SERIAL_TMR_RCCDEVICE,ENABLE);  ///ʹ��SERIAL_TMR_DEVICEʱ��
	//
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(SERIAL_TMR_DEVICE,&TIM_TimeBaseInitStructure);//��ʼ��SERIAL_TMR_DEVICE
	//
	NVIC_InitStructure.NVIC_IRQChannel=SERIAL_TMR_IRQN; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=SERIAL_TMR_PRE_PRIORITY; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=SERIAL_TMR_SUB_PRIORITY; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//
	TIM_ClearITPendingBit( SERIAL_TMR_DEVICE,TIM_IT_Update) ;
	TIM_ITConfig(SERIAL_TMR_DEVICE,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	//
	TIM_Cmd(SERIAL_TMR_DEVICE,DISABLE);//�ȴ����ڽ������ֽڿ���
	
}


//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��4
void tmr_init_network(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//
	RCC_APB1PeriphClockCmd(NETWORK_TMR_RCCDEVICE,ENABLE);  ///ʹ��TIM3ʱ��
	//
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(NETWORK_TMR_DEVICE,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	//
	NVIC_InitStructure.NVIC_IRQChannel=NETWORK_TMR_IRQN; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=NETWORK_TMR_PRE_PRIORITY; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=NETWORK_TMR_SUB_PRIORITY; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//
	TIM_ClearITPendingBit( NETWORK_TMR_DEVICE,TIM_IT_Update) ;
	TIM_ITConfig(NETWORK_TMR_DEVICE,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	//
	TIM_Cmd(NETWORK_TMR_DEVICE,DISABLE);//�ȴ����ڽ������ֽڿ���
	
}















