#ifndef __TMR_USART_H__
#define __TMR_USART_H__

#include "stm32f10x.h"



/*****\*****\*****\*****终端串口定时器*****\*****\*****\*****/
#define		SERIAL_TMR_RCCDEVICE						RCC_APB1Periph_TIM3
#define		SERIAL_TMR_DEVICE								TIM3
#define		SERIAL_TMR_IRQN									TIM3_IRQn
#define		SERIAL_TMR_PRE_PRIORITY					0x00
#define		SERIAL_TMR_SUB_PRIORITY					0x01

/*****\*****\*****\*****网络端串口定时器*****\*****\*****\*****/
#define		NETWORK_TMR_RCCDEVICE						RCC_APB1Periph_TIM4
#define		NETWORK_TMR_DEVICE							TIM4
#define		NETWORK_TMR_IRQN								TIM4_IRQn
#define		NETWORK_TMR_PRE_PRIORITY				0x00
#define		NETWORK_TMR_SUB_PRIORITY				0x01








//
extern void tmr_init_serial(u16 arr,u16 psc);
extern void tmr_init_network( u16 arr,u16 psc);












#endif
