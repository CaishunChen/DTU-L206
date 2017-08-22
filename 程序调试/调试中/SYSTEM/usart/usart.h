#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 



/*****\*****\*****\*****终端串口*****\*****\*****\*****/
#define		DEVICE_SERIAL_PORT							GPIOA
#define		DEVICE_SERIAL_RCCPORT						RCC_APB2Periph_GPIOA
#define		DEVICE_SERIAL_RCCDEVICE					RCC_APB2Periph_USART1
#define		DEVICE_SERIAL_TX_PIN						GPIO_Pin_9
#define		DEVICE_SERIAL_RX_PIN						GPIO_Pin_10
#define		DEVICE_SERIAL_DEVICE						USART1
#define		DEVICE_SERIAL_IRQN							USART1_IRQn
#define		DEVICE_SERIAL_PRE_PRIORITY			3
#define		DEVICE_SERIAL_SUB_PRIORITY			3


/*****\*****\*****\*****网络端串口*****\*****\*****\*****/
#define		DEVICE_NETWORK_PORT							GPIOA
#define		DEVICE_NETWORK_RCCPORT					RCC_APB2Periph_GPIOA
#define		DEVICE_NETWORK_RCCDEVICE				RCC_APB1Periph_USART2
#define		DEVICE_NETWORK_TX_PIN						GPIO_Pin_2
#define		DEVICE_NETWORK_RX_PIN						GPIO_Pin_3
#define		DEVICE_NETWORK_DEVICE						USART2
#define		DEVICE_NETWORK_IRQN							USART2_IRQn
#define		DEVICE_NETWORK_PRE_PRIORITY			3
#define		DEVICE_NETWORK_SUB_PRIORITY			3



//
#define 	USART_SERIAL_REC_LEN  			200	//定义最大接收字节数
#define 	USART_NETWORK_REC_LEN 			200	//定义最大接收字节数
//
extern vu8  USART_SERIAL_RX_BUF[USART_SERIAL_REC_LEN]; //
extern vu16 USART_SERIAL_RX_STA;         		//
extern vu16 USART_SERIAL_RX_LEN;         		//
//
extern vu8  USART_NETWORK_RX_BUF[USART_NETWORK_REC_LEN]; //
extern vu16 USART_NETWORK_RX_STA;         		//
extern vu16 USART_NETWORK_RX_LEN;         		//
//
extern vu8 NewDataUpdate_Serial ; 			
extern vu8 NewDataUpdate_Network ; 			
//
extern void uart_init_serial(u32 bound);
extern void uart_init_network(u32 bound);
//
extern void UsartSendStr_serial(u8 volatile *Pdata, u8 len);
extern void UsartSendStr_network(u8 volatile *Pdata, u8 len);




#endif


