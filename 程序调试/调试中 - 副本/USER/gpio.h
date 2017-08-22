#ifndef __GPIO_H
#define __GPIO_H	 
#include "sys.h"


/*****\*****\*****\*****GPIO管脚*****\*****\*****\*****/
//L206电源信号
#define		DEVICE_PWR_PORT									GPIOB
#define		DEVICE_PWR_PIN									GPIO_Pin_5
#define		DEVICE_PWR_RCCPORT							RCC_APB2Periph_GPIOB
//L206复位信号
#define		DEVICE_RESET_PORT								GPIOB
#define		DEVICE_RESET_PIN								GPIO_Pin_6
#define		DEVICE_RESET_RCCPORT						RCC_APB2Periph_GPIOB
//L206电源状态
#define		DEVICE_PWRSTATE_PORT						GPIOB
#define		DEVICE_PWRSTATE_PIN							GPIO_Pin_7
#define		DEVICE_PWRSTATE_RCCPORT					RCC_APB2Periph_GPIOB



//
typedef enum
{
	GPRS_PWR_ON = 0,
	GPRS_PWR_OFF
}GPRS_PWR_ON_OFF;
//
typedef enum
{
	GPRS_RESET_ON = 0,
	GPRS_RESET_OFF
}GPRS_RESET_ON_OFF;
//
typedef enum
{
	GPRS_STATE_PWRON = 0,
	GPRS_STATE_PWROFF
}GPRS_STATE_ON_OFF;


#define		GPRS_PWR					PBout(5)// PB5
#define		GPRS_RESET				PBout(6)// PE5	
#define		GPRS_PWRSTATE			PBin(7)// PE5	

extern void L206_Gpio_Init(void);//初始化

		 				    
#endif
