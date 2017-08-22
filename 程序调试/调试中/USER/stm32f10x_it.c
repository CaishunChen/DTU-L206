/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
//
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "usart.h"
#include "string.h"
#include "tmr_usart.h"


/*\*****\*****\*****\*****中断转接*****\*****\*****\*****\*/
#define		DTUUsartSerial_InterruptProcess		USART1_IRQHandler
#define		DTUUsartNetwork_InterruptProcess	USART2_IRQHandler
#define		DTUSerialTimer_InterruptProcess		TIM3_IRQHandler
#define		DTUNetworkTimer_InterruptProcess	TIM4_IRQHandler


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input            : None
* Output         : None
* Return         : None
*******************************************************************************/
void DTUUsartSerial_InterruptProcess(void)                	//串口1中断服务程序
{
    //
    if(USART_GetITStatus(DEVICE_SERIAL_DEVICE, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
			//接收到新数据时如果之前数据未处理，则覆盖
				if(0!=(USART_SERIAL_RX_STA&0x8000))
				{
					USART_SERIAL_RX_STA=0;//清除之前接收状态
					memset( (u8*)USART_SERIAL_RX_BUF,0,USART_SERIAL_REC_LEN);//清除之前接收数据
				}
				//
				if((USART_SERIAL_RX_STA & 0x8000) == 0) //接收未完成
				{
					TIM_Cmd(SERIAL_TMR_DEVICE,DISABLE); //关闭定时器计时
					//存储数据到缓冲数组
					USART_SERIAL_RX_BUF[USART_SERIAL_RX_STA++]=USART_ReceiveData( DEVICE_SERIAL_DEVICE);
					TIM_SetCounter( SERIAL_TMR_DEVICE,0 );//清零计数器
					TIM_Cmd(SERIAL_TMR_DEVICE,ENABLE); //开启定时器计时
				}
		}
	//清零接收中断标志		
	USART_ClearITPendingBit( DEVICE_SERIAL_DEVICE,USART_IT_RXNE) ;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input            : None
* Output         : None
* Return         : None
*******************************************************************************/
void DTUUsartNetwork_InterruptProcess(void)                	//串口1中断服务程序
{
		//
    if(USART_GetITStatus(DEVICE_NETWORK_DEVICE, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
			//接收到新数据时如果之前数据未处理，则覆盖
				if(0!=(USART_NETWORK_RX_STA&0x8000))
				{
					USART_NETWORK_RX_STA=0;//清除之前接收状态
					memset( (u8*)USART_NETWORK_RX_BUF,0,USART_NETWORK_REC_LEN);//清除之前接收数据
				}
				//
				if((USART_NETWORK_RX_STA & 0x8000) == 0) //接收未完成
				{
					TIM_Cmd(NETWORK_TMR_DEVICE,DISABLE); //关闭定时器计时
					//存储数据到缓冲数组
					USART_NETWORK_RX_BUF[USART_NETWORK_RX_STA++]=USART_ReceiveData( DEVICE_NETWORK_DEVICE);
					TIM_SetCounter( NETWORK_TMR_DEVICE,0 );//清零计数器
					TIM_Cmd(NETWORK_TMR_DEVICE,ENABLE); //开启定时器计时
				}
		}
	//清零接收中断标志		
	USART_ClearITPendingBit( DEVICE_NETWORK_DEVICE,USART_IT_RXNE) ;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input            : None
* Output         : None
* Return         : None
*******************************************************************************/
void DTUSerialTimer_InterruptProcess(void)           
{
//	static u16 tt = 0;
	//
	if(TIM_GetITStatus(SERIAL_TMR_DEVICE, TIM_IT_Update) != RESET)  //检查TIM4更新中断发生
		{
			TIM_ClearITPendingBit(SERIAL_TMR_DEVICE, TIM_IT_Update  );  //清除TIMx更新中断标志 
			
			//接收数据过程中超过10ms没有新数据则认为是一帧数据结束-置接收完成标志位
			USART_SERIAL_RX_STA |= 0x8000;
			USART_SERIAL_RX_LEN = USART_SERIAL_RX_STA&0x7FFF;
			NewDataUpdate_Serial =1;
			//
			TIM_SetCounter( SERIAL_TMR_DEVICE,0 );//清零计数器
			TIM_Cmd(SERIAL_TMR_DEVICE,DISABLE); //关闭定时器计时-等待新的数据帧启动定时器

//			//定时器测试程序
//			tt++;
//			if( tt>=50)
//			{
//				tt=0;
////				UsartSendStr_SWJ( "siemens test!\r\n",8);
//			}
		}
		
		//清零接收中断标志		
		TIM_ClearITPendingBit( SERIAL_TMR_DEVICE,TIM_IT_Update) ;
}
void DTUNetworkTimer_InterruptProcess(void)           
{
//	static u16 tt = 0;
	//
	if(TIM_GetITStatus(NETWORK_TMR_DEVICE, TIM_IT_Update) != RESET)  //检查TIM4更新中断发生
		{
			TIM_ClearITPendingBit(NETWORK_TMR_DEVICE, TIM_IT_Update  );  //清除TIMx更新中断标志 
			
			//接收数据过程中超过10ms没有新数据则认为是一帧数据结束-置接收完成标志位
				USART_NETWORK_RX_STA |= 0x8000;
				USART_NETWORK_RX_LEN = USART_NETWORK_RX_STA&0x7FFF;
				NewDataUpdate_Network =1;
				//
				TIM_SetCounter( NETWORK_TMR_DEVICE,0 );//清零计数器
				TIM_Cmd(NETWORK_TMR_DEVICE,DISABLE); //关闭定时器计时-等待新的数据帧启动定时器

//			//定时器测试程序
//			tt++;
//			if( tt>=50)
//			{
//				tt=0;
////				UsartSendStr_SWJ( "siemens test!\r\n",8);
//			}
		}
		
		//清零接收中断标志		
		TIM_ClearITPendingBit( NETWORK_TMR_DEVICE,TIM_IT_Update) ;
}





/**********/
//void USART1_IRQHandler(void)                	//串口1中断服务程序
//{
//		DTUUsartSerial_InterruptProcess();
//}
//void USART2_IRQHandler(void)                	//串口1中断服务程序
//{
//   DTUUsartNetwork_InterruptProcess();
//}


//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//定时器4中断服务程序--------10ms
//void TIM3_IRQHandler(void)   //TIM4中断
//{
//		DTUSerialTimer_InterruptProcess();
//}
////通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
////如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
////任何数据,则表示此次接收完毕.
////定时器4中断服务程序--------10ms
//void TIM4_IRQHandler(void)   //TIM4中断
//{
//		DTUNetworkTimer_InterruptProcess();
//}
/**********/

 
void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
