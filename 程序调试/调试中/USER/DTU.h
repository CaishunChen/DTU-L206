#ifndef __DTU_H
#define __DTU_H	 
#include "sys.h"

//
#define		STR_DTU_CFG_BAUDRATESET			"DTU_CFG_BAUDRATESET="
#define		STR_DTU_CFG_PORTSET					"DTU_CFG_PORTSET="
#define		STR_DTU_CFG_HEARTTIMESET		"DTU_CFG_HEARTTIMESET="
//#define		STR_DTU_CFG_MODEIDSET				"DTU_CFG_MODEIDSET="
#define		STR_DTU_CFG_GOGPRSCFG				"DTU_CFG_GOGPRSCFG"
#define		STR_DTU_CFG_SAVE						"DTU_CFG_SAVE"
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
extern vu8 DTU_CurrentState ;

extern vu8 GPRS_CfgState;
extern vu8 GPRS_CfgEvent;

extern vu8 DTU_CfgState;
extern vu8 DTU_CfgEvent;

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
//
typedef enum
{
		DTU_CFG_NULL = 0   ,
		DTU_CFG_PORTSET   ,//232/485
		DTU_CFG_BAUDRATESET   ,	//baud rate
		DTU_CFG_BUILD_CONNECT  ,	//
		DTU_CFG_HEARTSET  ,	//心跳包设置
		DTU_CFG_MODEIDSET  ,	//模块标识符设置
	
		DTU_CFG_GOGPRSCFG  ,	//GPRS模块配置

		DTU_CFG_EXITCFGMODE  ,	//退出配置模式

} DTU_CFG_STEP;
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
	DTU_SUSPENDED
	
}DTU_LINKSTATE;//DTU连接状态
//
typedef enum
{
	DTU_PWRISOFF=  0,
	DTU_PWRISON
	
}DTU_PWRONOFF_STATE;//DTU电源状态
//
typedef enum
{
	DTU_INITNONE=  0,
	DTU_INITOVER
	
}DTU_INIT_STATE;//DTU初始化状态

//
typedef struct
{
	u32 baudrate;//波特率
	DTU_PORT_MODE port_mode;//管脚模式
	u16 hearttime;//心跳时间
	
	//
	char server_ipaddr[20];
	char server_port[10];
	char server_apn[10];
	
}DEVICE_PARA_INFO;
//
typedef struct
{
	//
	DTU_PWRONOFF_STATE pwr_state;
	DTU_INIT_STATE init_state;
	DTU_LINKSTATE link_state;
	
}DEVICE_STATE_INFO;

//
typedef struct
{
		DEVICE_PARA_INFO para_info;
		DEVICE_STATE_INFO state_info;
}DTU_DEVICE_INFO;
extern DTU_DEVICE_INFO DTU_Device_Info;
extern DTU_DEVICE_INFO DeviceInfoForCfg;

//
typedef struct
{
	void(* serial_usart_init)(u32 t);
	void(* network_usart_init)(u32 t);
	void(* serial_tmr_init)(u16 arr,u16 psc);
	void(* network_tmr_init)(u16 arr,u16 psc);
	void(* gpio_init)();

}DTU_INIT_FUNC;
extern DTU_INIT_FUNC DTU_Init_Func;


extern void DTU_Loop(void);
extern u8 DTU_cfg(void);
extern void GPRS_cfg(u8 event);
extern void ReadDTUInfoFromFlash(DTU_DEVICE_INFO* dev);
extern void WriteDTUInfoToFlash(DTU_DEVICE_INFO dev);

extern void gprs_reset( void);
extern u8* gprs_check_cmd(u8* str,u8* ack);
extern void DTU_HardwareInit(void);









		 				    
#endif
