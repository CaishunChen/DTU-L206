#include "sys.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////



//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
    int handle;

};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
    x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
    while((USART1->SR & 0X40) == 0); //ѭ������,ֱ���������
    USART1->DR = (u8) ch;
    return ch;
}
#endif

/*ʹ��microLib�ķ���*/
/*
int fputc(int ch, FILE *f)
{
USART_SendData(USART1, (uint8_t) ch);

while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}

   return ch;
}
int GetKey (void)  {

   while (!(USART1->SR & USART_FLAG_RXNE));

   return ((int)(USART1->DR & 0x1FF));
}
*/




//����״̬
//bit15��	������ɱ�־
//bit14~0��	���յ�����Ч�ֽ���Ŀ
vu16 USART_SERIAL_RX_STA = 0;	//����״̬���
vu16 USART_SERIAL_RX_LEN = 0;	//���ճ���
//���ջ���,���USART_SERIAL_REC_LEN���ֽ�.
vu8 USART_SERIAL_RX_BUF[USART_SERIAL_REC_LEN];   

/*******************************************************************************
* Function Name  : 
* Description    : �ն˴��ڳ�ʼ��
* Input					 : bound��������
* Output         : None
* Return         : None
*******************************************************************************/
void uart_init_serial(u32 bound) {
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(DEVICE_SERIAL_RCCDEVICE | DEVICE_SERIAL_RCCPORT, ENABLE);	//ʹ��USART1��GPIOAʱ��

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = DEVICE_SERIAL_TX_PIN; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(DEVICE_SERIAL_PORT, &GPIO_InitStructure);//��ʼ��GPIOA.9

    //USART1_RX	  GPIOA.10��ʼ��
    GPIO_InitStructure.GPIO_Pin = DEVICE_SERIAL_RX_PIN;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(DEVICE_SERIAL_PORT, &GPIO_InitStructure);//��ʼ��GPIOA.10

    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = DEVICE_SERIAL_IRQN;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DEVICE_SERIAL_PRE_PRIORITY ; //��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DEVICE_SERIAL_SUB_PRIORITY;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

    //USART ��ʼ������

    USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(DEVICE_SERIAL_DEVICE, &USART_InitStructure); //��ʼ������1
    USART_ITConfig(DEVICE_SERIAL_DEVICE, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
    USART_Cmd(DEVICE_SERIAL_DEVICE, ENABLE);                    //ʹ�ܴ���1
}




//����״̬
//bit15��	������ɱ�־
//bit14~0��	���յ�����Ч�ֽ���Ŀ
vu16 USART_NETWORK_RX_STA = 0;	//����״̬���
vu16 USART_NETWORK_RX_LEN = 0;	//���ճ���
//���ջ���,���USART_SERIAL_REC_LEN���ֽ�.
vu8 USART_NETWORK_RX_BUF[USART_NETWORK_REC_LEN];  
/*******************************************************************************
* Function Name  : 
* Description    : ����˴��ڳ�ʼ��
* Input					 : bound��������
* Output         : None
* Return         : None
*******************************************************************************/
void uart_init_network(u32 bound)
{
		 //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(DEVICE_NETWORK_RCCDEVICE , ENABLE);	//ʹ��USART1��GPIOAʱ��
    RCC_APB2PeriphClockCmd(DEVICE_NETWORK_RCCPORT, ENABLE);	//ʹ��USART1��GPIOAʱ��

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = DEVICE_NETWORK_TX_PIN; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(DEVICE_NETWORK_PORT, &GPIO_InitStructure);//��ʼ��GPIOA.9

    //USART1_RX	  GPIOA.10��ʼ��
    GPIO_InitStructure.GPIO_Pin = DEVICE_NETWORK_RX_PIN;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(DEVICE_NETWORK_PORT, &GPIO_InitStructure);//��ʼ��GPIOA.10

    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = DEVICE_NETWORK_IRQN;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DEVICE_SERIAL_SUB_PRIORITY; //��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DEVICE_NETWORK_SUB_PRIORITY;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

    //USART ��ʼ������

    USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(DEVICE_NETWORK_DEVICE, &USART_InitStructure); //��ʼ������1
    USART_ITConfig(DEVICE_NETWORK_DEVICE, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
    USART_Cmd(DEVICE_NETWORK_DEVICE, ENABLE);                    //ʹ�ܴ���1
}




/*******************************************************************************
* Function Name  : 
* Description    : �ն˴������ݷ���
* Input          : Pdata������ָ��	len�����ݳ���
* Output         : None
* Return         : None
*******************************************************************************/
void UsartSendStr_serial(u8 volatile *Pdata, u8 len)
{
    u16 i;
	//		
	/*�������ݵĵ�һλ�ᶪʧ���������ｫ��һλ���ݷ�������*/
	for(i = 0; i < len; i++)
	{
		USART_ClearFlag(DEVICE_SERIAL_DEVICE, USART_FLAG_TC);	//����жϱ�־λ,��֤��һ���ֽڵ����ݲ���ʧ
		USART_SendData(DEVICE_SERIAL_DEVICE, Pdata[i]);//�򴮿�1��������
		while(USART_GetFlagStatus(DEVICE_SERIAL_DEVICE, USART_FLAG_TC) != SET); //�ȴ����ͽ���
	}
}

/*******************************************************************************
* Function Name  : 
* Description    : ����˴������ݷ���
* Input          : Pdata������ָ��	len�����ݳ���
* Output         : None
* Return         : None
*******************************************************************************/
void UsartSendStr_network(u8 volatile *Pdata, u8 len)
{
    u16 i;
	//		
	/*�������ݵĵ�һλ�ᶪʧ���������ｫ��һλ���ݷ�������*/
	for(i = 0; i < len; i++)
	{
		USART_ClearFlag(DEVICE_NETWORK_DEVICE, USART_FLAG_TC);	//����жϱ�־λ,��֤��һ���ֽڵ����ݲ���ʧ
		USART_SendData(DEVICE_NETWORK_DEVICE, Pdata[i]);//�򴮿�1��������
		while(USART_GetFlagStatus(DEVICE_NETWORK_DEVICE, USART_FLAG_TC) != SET); //�ȴ����ͽ���
	}
}








