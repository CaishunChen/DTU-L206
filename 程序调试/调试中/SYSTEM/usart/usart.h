#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 



#define USART_SERIAL_REC_LEN  			200  	//�����������ֽ��� 200
#define USART_NETWORK_REC_LEN 			200  	//�����������ֽ��� 200


//
extern vu8  USART_SERIAL_RX_BUF[USART_SERIAL_REC_LEN]; //���ջ���,���USART_SERIAL_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern vu16 USART_SERIAL_RX_STA;         		//����״̬���	
extern vu16 USART_SERIAL_RX_LEN;         		//���ճ���

//
extern vu8  USART_NETWORK_RX_BUF[USART_NETWORK_REC_LEN]; //���ջ���,���USART_SERIAL_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern vu16 USART_NETWORK_RX_STA;         		//����״̬���	
extern vu16 USART_NETWORK_RX_LEN;         		//���ճ���


extern vu8 DTU_CurrentState/*=DTU_INIT_READY*/;







//
typedef enum
{
		DTU_INIT_NULL = 0 ,
		DTU_INIT_CHECKSTA  ,
    DTU_INIT_PWROFF  ,
		DTU_INIT_PWRON  ,
		DTU_INIT_BOOTING  ,
    DTU_INIT_DTUCFG ,
		DTU_INIT_GPRSCFG ,
		DTU_NORMAL_OFFLINE  ,
    DTU_NORMAL_ONLINE  ,
    DTU_NORMAL_SUSPEND  ,
    DTU_NORMAL_REQEXITSUSPEND

} DTU_RUN_STEP;
extern vu8 DTU_CurrentState ;

//
typedef enum
{
		GPRS_CFG_NULL = 0   ,
		GPRS_CFG_RESET    ,
		GPRS_CFG_RD_INFO   ,
		GPRS_CFG_AT   ,
		GPRS_CFG_ATE0   ,
		GPRS_CFG_ATCSQ   ,
		GPRS_CFG_ATCREG   ,
		GPRS_CFG_ATCGATT   ,
		GPRS_CFG_ATCSTT   ,
		GPRS_CFG_ATCIICR   ,
		GPRS_CFG_ATCIFSR   ,
		GPRS_CFG_ATCIPSHUT   ,
		GPRS_CFG_ATCIPMODE   ,
		GPRS_CFG_ATCIPMUX   ,
		GPRS_CFG_ATCIPSTART   ,

} GPRS_CFG_STEP;
extern vu8 GPRS_CfgState;
extern vu8 GPRS_CfgEvent;


//
#define		STR_DTU_CFG_BAUDRATESET			"DTU_CFG_BAUDRATESET="
#define		STR_DTU_CFG_PORTSET					"DTU_CFG_PORTSET="
#define		STR_DTU_CFG_HEARTTIMESET		"DTU_CFG_HEARTTIMESET="
//#define		STR_DTU_CFG_MODEIDSET				"DTU_CFG_MODEIDSET="
#define		STR_DTU_CFG_GOGPRSCFG				"DTU_CFG_GOGPRSCFG"
#define		STR_DTU_CFG_EXITCFGMODE			"DTU_CFG_EXITCFGMODE"
//
#define		STR_DTU_CFG_BAUDRATEGET			"DTU_CFG_BAUDRATEGET=?"
#define		STR_DTU_CFG_PORTGET					"DTU_CFG_PORTGET=?"
#define		STR_DTU_CFG_HEARTTIMEGET		"STR_DTU_CFG_HEARTTIMEGET=?"
//#define		STR_DTU_CFG_MODEIDGET				"DTU_CFG_MODEIDSET="
//
//
#define		STR_DTU_CFGSTEPSET			"STR_DTU_CFGSTEPSET="
#define		STR_GPRS_CFGSTEPSET			"STR_GPRS_CFGSTEPSET="
#define		STR_DTU_RUNSTEPSET			"STR_DTU_RUNSTEPSET="
//
#define		STR_DTU_CFGSTEPGET			"STR_DTU_CFGSTEPGET=?"
#define		STR_GPRS_CFGSTEPGET			"STR_GPRS_CFGSTEPGET=?"
#define		STR_DTU_RUNSTEPGET			"STR_DTU_RUNSTEPGET=?"

//




typedef enum
{
		DTU_CFG_NULL = 0   ,
		DTU_CFG_PORTSET   ,//232/485?
		DTU_CFG_BAUDRATESET   ,	//baud rate
		DTU_CFG_BUILD_CONNECT  ,	//
		DTU_CFG_HEARTSET  ,	//����������
		DTU_CFG_MODEIDSET  ,	//ģ���ʶ������
	
		DTU_CFG_GOGPRSCFG  ,	//GPRSģ������

		DTU_CFG_EXITCFGMODE  ,	//�˳�����ģʽ


} DTU_CFG_STEP;
extern vu8 DTU_CfgState;
extern vu8 DTU_CfgEvent;



extern vu8 NewDataUpdate_Serial ; /*���ڶ����ݸ���*/
extern vu8 NewDataUpdate_Network ; /*��������ݸ���*/



//����봮���жϽ��գ��벻Ҫע�����º궨��
extern void uart_init_serial(u32 bound);
extern void uart_init_network(u32 bound);


extern void UsartSendStr_serial(u8 volatile *Pdata, u8 len);
extern void UsartSendStr_network(u8 volatile *Pdata, u8 len);

extern void GPRS_cfg(u8 event);
extern void DTU_cfg(void);

//
typedef enum
{
	PORT_MODE_232 = 0,
	PORT_MODE_485
}DTU_PORT_MODE;
//
typedef enum
{
	DTU_UNLINKED = 0,
	DTU_LINKED,
	DTU_SUSPENDED,
}DTU_LINKSTATE;//DTU����״̬
//
typedef enum
{
	DTU_PWRISOFF=  0,
	DTU_PWRISON
}DTU_PWRONOFF_STATE;//DTU��Դ״̬
//
typedef enum
{
	DTU_INITNONE=  0,
	DTU_INITOVER
}DTU_INIT_STATE;//DTU��ʼ��״̬
//
typedef struct
{
	u32 baudrate;//������
	DTU_PORT_MODE port_mode;//�ܽ�ģʽ
	u16 hearttime;//����ʱ��
	
	//
	char server_ipaddr[20];
	char server_port[10];
	char server_apn[10];

	//
	DTU_PWRONOFF_STATE pwr_state;
	DTU_INIT_STATE init_state;
	DTU_LINKSTATE link_state;
	
}DTU_DEVICE_INFO;



#endif


