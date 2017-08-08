#ifndef __GPIO_H
#define __GPIO_H	 
#include "sys.h"




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

extern void Gpio_Init(void);//≥ı ºªØ

		 				    
#endif
