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


/*\*****\*****\*****\*****�ж�ת��*****\*****\*****\*****\*/
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
void DTUUsartSerial_InterruptProcess(void)                	//����1�жϷ������
{
    //
    if(USART_GetITStatus(DEVICE_SERIAL_DEVICE, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
    {
			//���յ�������ʱ���֮ǰ����δ�����򸲸�
				if(0!=(USART_SERIAL_RX_STA&0x8000))
				{
					USART_SERIAL_RX_STA=0;//���֮ǰ����״̬
					memset( (u8*)USART_SERIAL_RX_BUF,0,USART_SERIAL_REC_LEN);//���֮ǰ��������
				}
				//
				if((USART_SERIAL_RX_STA & 0x8000) == 0) //����δ���
				{
					TIM_Cmd(SERIAL_TMR_DEVICE,DISABLE); //�رն�ʱ����ʱ
					//�洢���ݵ���������
					USART_SERIAL_RX_BUF[USART_SERIAL_RX_STA++]=USART_ReceiveData( DEVICE_SERIAL_DEVICE);
					TIM_SetCounter( SERIAL_TMR_DEVICE,0 );//���������
					TIM_Cmd(SERIAL_TMR_DEVICE,ENABLE); //������ʱ����ʱ
				}
		}
	//��������жϱ�־		
	USART_ClearITPendingBit( DEVICE_SERIAL_DEVICE,USART_IT_RXNE) ;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input            : None
* Output         : None
* Return         : None
*******************************************************************************/
void DTUUsartNetwork_InterruptProcess(void)                	//����1�жϷ������
{
		//
    if(USART_GetITStatus(DEVICE_NETWORK_DEVICE, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
    {
			//���յ�������ʱ���֮ǰ����δ�����򸲸�
				if(0!=(USART_NETWORK_RX_STA&0x8000))
				{
					USART_NETWORK_RX_STA=0;//���֮ǰ����״̬
					memset( (u8*)USART_NETWORK_RX_BUF,0,USART_NETWORK_REC_LEN);//���֮ǰ��������
				}
				//
				if((USART_NETWORK_RX_STA & 0x8000) == 0) //����δ���
				{
					TIM_Cmd(NETWORK_TMR_DEVICE,DISABLE); //�رն�ʱ����ʱ
					//�洢���ݵ���������
					USART_NETWORK_RX_BUF[USART_NETWORK_RX_STA++]=USART_ReceiveData( DEVICE_NETWORK_DEVICE);
					TIM_SetCounter( NETWORK_TMR_DEVICE,0 );//���������
					TIM_Cmd(NETWORK_TMR_DEVICE,ENABLE); //������ʱ����ʱ
				}
		}
	//��������жϱ�־		
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
	if(TIM_GetITStatus(SERIAL_TMR_DEVICE, TIM_IT_Update) != RESET)  //���TIM4�����жϷ���
		{
			TIM_ClearITPendingBit(SERIAL_TMR_DEVICE, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
			
			//�������ݹ����г���10msû������������Ϊ��һ֡���ݽ���-�ý�����ɱ�־λ
			USART_SERIAL_RX_STA |= 0x8000;
			USART_SERIAL_RX_LEN = USART_SERIAL_RX_STA&0x7FFF;
			NewDataUpdate_Serial =1;
			//
			TIM_SetCounter( SERIAL_TMR_DEVICE,0 );//���������
			TIM_Cmd(SERIAL_TMR_DEVICE,DISABLE); //�رն�ʱ����ʱ-�ȴ��µ�����֡������ʱ��

//			//��ʱ�����Գ���
//			tt++;
//			if( tt>=50)
//			{
//				tt=0;
////				UsartSendStr_SWJ( "siemens test!\r\n",8);
//			}
		}
		
		//��������жϱ�־		
		TIM_ClearITPendingBit( SERIAL_TMR_DEVICE,TIM_IT_Update) ;
}
void DTUNetworkTimer_InterruptProcess(void)           
{
//	static u16 tt = 0;
	//
	if(TIM_GetITStatus(NETWORK_TMR_DEVICE, TIM_IT_Update) != RESET)  //���TIM4�����жϷ���
		{
			TIM_ClearITPendingBit(NETWORK_TMR_DEVICE, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
			
			//�������ݹ����г���10msû������������Ϊ��һ֡���ݽ���-�ý�����ɱ�־λ
				USART_NETWORK_RX_STA |= 0x8000;
				USART_NETWORK_RX_LEN = USART_NETWORK_RX_STA&0x7FFF;
				NewDataUpdate_Network =1;
				//
				TIM_SetCounter( NETWORK_TMR_DEVICE,0 );//���������
				TIM_Cmd(NETWORK_TMR_DEVICE,DISABLE); //�رն�ʱ����ʱ-�ȴ��µ�����֡������ʱ��

//			//��ʱ�����Գ���
//			tt++;
//			if( tt>=50)
//			{
//				tt=0;
////				UsartSendStr_SWJ( "siemens test!\r\n",8);
//			}
		}
		
		//��������жϱ�־		
		TIM_ClearITPendingBit( NETWORK_TMR_DEVICE,TIM_IT_Update) ;
}





/**********/
//void USART1_IRQHandler(void)                	//����1�жϷ������
//{
//		DTUUsartSerial_InterruptProcess();
//}
//void USART2_IRQHandler(void)                	//����1�жϷ������
//{
//   DTUUsartNetwork_InterruptProcess();
//}


//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//��ʱ��4�жϷ������--------10ms
//void TIM3_IRQHandler(void)   //TIM4�ж�
//{
//		DTUSerialTimer_InterruptProcess();
//}
////ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
////���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
////�κ�����,���ʾ�˴ν������.
////��ʱ��4�жϷ������--------10ms
//void TIM4_IRQHandler(void)   //TIM4�ж�
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
