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
//



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input            : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)                	//����1�жϷ������
{
    //
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
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
					TIM_Cmd(TIM3,DISABLE); //�رն�ʱ����ʱ
					//�洢���ݵ���������
					USART_SERIAL_RX_BUF[USART_SERIAL_RX_STA++]=USART_ReceiveData( USART1);
					TIM_SetCounter( TIM3,0 );//���������
					TIM_Cmd(TIM3,ENABLE); //������ʱ����ʱ
				}
		}
	//��������жϱ�־		
	USART_ClearITPendingBit( USART1,USART_IT_RXNE) ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input            : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)                	//����1�жϷ������
{
//    u8 Res;
    //
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
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
					TIM_Cmd(TIM4,DISABLE); //�رն�ʱ����ʱ
					//�洢���ݵ���������
					USART_NETWORK_RX_BUF[USART_NETWORK_RX_STA++]=USART_ReceiveData( USART2);
					TIM_SetCounter( TIM4,0 );//���������
					TIM_Cmd(TIM4,ENABLE); //������ʱ����ʱ
				}
		}
	//��������жϱ�־		
	USART_ClearITPendingBit( USART2,USART_IT_RXNE) ;
}


//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//��ʱ��4�жϷ������--------10ms
void TIM3_IRQHandler(void)   //TIM4�ж�
{
	static u16 tt = 0;
	u16 rec_len =0; 
	//
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM4�����жϷ���
		{
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
			
			//�������ݹ����г���10msû������������Ϊ��һ֡���ݽ���-�ý�����ɱ�־λ
			USART_SERIAL_RX_STA |= 0x8000;
			USART_SERIAL_RX_LEN = USART_SERIAL_RX_STA&0x7FFF;
			NewDataUpdate_Serial =1;
			//
			TIM_SetCounter( TIM3,0 );//���������
			TIM_Cmd(TIM3,DISABLE); //�رն�ʱ����ʱ-�ȴ��µ�����֡������ʱ��

//			//��ʱ�����Գ���
//			tt++;
//			if( tt>=50)
//			{
//				tt=0;
////				UsartSendStr_SWJ( "siemens test!\r\n",8);
//			}
		}
		
		//��������жϱ�־		
		TIM_ClearITPendingBit( TIM3,TIM_IT_Update) ;
}



//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//��ʱ��4�жϷ������--------10ms
void TIM4_IRQHandler(void)   //TIM4�ж�
{
	static u16 tt = 0;
	u16 rec_len =0; 
	//
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //���TIM4�����жϷ���
		{
			TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
			
			//�������ݹ����г���10msû������������Ϊ��һ֡���ݽ���-�ý�����ɱ�־λ
				USART_NETWORK_RX_STA |= 0x8000;
				USART_NETWORK_RX_LEN = USART_NETWORK_RX_STA&0x7FFF;
				NewDataUpdate_Network =1;
				//
				TIM_SetCounter( TIM4,0 );//���������
				TIM_Cmd(TIM4,DISABLE); //�رն�ʱ����ʱ-�ȴ��µ�����֡������ʱ��

//			//��ʱ�����Գ���
//			tt++;
//			if( tt>=50)
//			{
//				tt=0;
////				UsartSendStr_SWJ( "siemens test!\r\n",8);
//			}
		}
		
		//��������жϱ�־		
		TIM_ClearITPendingBit( TIM4,TIM_IT_Update) ;
}







 
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
