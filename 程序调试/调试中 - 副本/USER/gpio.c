#include "gpio.h"




//LED IO��ʼ��
void L206_Gpio_Init(void)
{
 
		GPIO_InitTypeDef  GPIO_InitStructure;

		RCC_APB2PeriphClockCmd(DEVICE_PWR_RCCPORT, ENABLE);	 //
		RCC_APB2PeriphClockCmd(DEVICE_PWR_RCCPORT, ENABLE);	 //
		RCC_APB2PeriphClockCmd(DEVICE_PWR_RCCPORT, ENABLE);	 //

		GPIO_InitStructure.GPIO_Pin = DEVICE_PWR_PIN;				 //LED0-->PB.5 �˿�����
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(DEVICE_PWR_PORT, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
		GPRS_PWR = GPRS_PWR_OFF;
	  //
		GPIO_InitStructure.GPIO_Pin = DEVICE_RESET_PIN;				 //LED0-->PB.5 �˿�����
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(DEVICE_RESET_PORT, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
		GPRS_RESET = GPRS_RESET_OFF;
		//
		GPIO_InitStructure.GPIO_Pin = DEVICE_PWRSTATE_PIN;				 //LED0-->PB.5 �˿�����
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(DEVICE_PWRSTATE_PORT, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
		GPRS_RESET = GPRS_RESET_OFF;
		//

}
 
