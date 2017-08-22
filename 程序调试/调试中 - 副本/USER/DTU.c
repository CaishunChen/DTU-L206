#include "DTU.h"
#include "delay.h"
#include "gpio.h"
#include "sys.h"
#include "usart.h"
#include "tmr_usart.h"
#include "string.h"
#include "stdlib.h"


//
vu8 NewDataUpdate_Serial = 0; /*串口端数据更新*/
vu8 NewDataUpdate_Network = 0; /*网络端数据更新*/
//
vu8 DTU_CurrentState = DTU_INIT_CHECKSTA;	/*DTU当前状态*/
//
vu8 GPRS_CfgState = GPRS_CFG_NULL;	/*GPRS模块配置步骤*/
//
vu8 DTU_CfgState = DTU_CFG_NULL;	/*DTU配置步骤*/

//
DTU_DEVICE_INFO DTU_Device_Info=
{
		{
			115200,
			PORT_MODE_485,
			200,

			"221.226.199.97",
			"502",
			"CMNET"
		},
		{
			DTU_PWRISOFF,
			DTU_INITNONE,
			DTU_UNLINKED
		}
};
//在线配置时的参数暂存
DTU_DEVICE_INFO DeviceInfoForCfg;

//
DTU_INIT_FUNC DTU_Init_Func=
{
		uart_init_serial,
		uart_init_network,
		tmr_init_serial,
		tmr_init_network,
		L206_Gpio_Init
};


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input            : None
* Output         : None
* Return         : None
*******************************************************************************/
void DTU_HardwareInit(void)
{
    uart_init_serial(115200);	 //
    uart_init_network(115200);	 //
    tmr_init_serial(100 - 1, 7199); //100us * n
    tmr_init_network(100 - 1, 7199); //100us * n
		L206_Gpio_Init();
	
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input            : None
* Output         : None
* Return         : None
*******************************************************************************/
void DTU_Loop(void)
{
		static u8 GoHereTimes=0;
		u8 tmp=0;
	
		/*DTU state machine*/
		switch (DTU_CurrentState)
		{
		case DTU_INIT_NULL:

		break;
		
		case DTU_INIT_CHECKSTA://模块状态检测
				if(GPRS_STATE_PWRON != GPRS_PWRSTATE)
				{
						DTU_CurrentState=DTU_INIT_PWROFF;
				}
				else
				{
						DTU_CurrentState=DTU_INIT_PWRON;
				}
		break;

		case DTU_INIT_PWROFF:
				if(!GoHereTimes)
				{
					//模块上电
					GPRS_PWR=GPRS_PWR_ON;
					delay_ms(1000);
					GoHereTimes=1;
				}
				else
				{
					if(NewDataUpdate_Network)
					{
						if(gprs_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"Ready"))
						{
							GPRS_PWR=GPRS_PWR_OFF;
							DTU_CurrentState=DTU_INIT_BOOTING;
							GoHereTimes=0;
						}
						else
						{
						}
						NewDataUpdate_Network = 0;
					}
				}					
		break;

		case DTU_INIT_PWRON:
					DTU_CurrentState=DTU_INIT_BOOTING;
		break;
		
		case DTU_INIT_BOOTING:
				if(!GoHereTimes)
				{
					ReadDTUInfoFromFlash(&DTU_Device_Info);
					DTU_Init_Func.serial_usart_init(DTU_Device_Info.para_info.baudrate);//variable
					DTU_Init_Func.serial_tmr_init(100 - 1, 7199);//this value should change with the baudrate
					DTU_Init_Func.network_usart_init(115200);//const
					DTU_Init_Func.network_tmr_init(100 - 1, 7199);//const
					DTU_Init_Func.gpio_init();
					
					GPRS_CfgState=1;
					GoHereTimes=1;
				}
				else
				{
					if(GPRS_CfgState)//配置状态不为0，继续配置工作
					{
						GPRS_cfg(0);
					}
					else
					{
						DTU_CurrentState=DTU_NORMAL_ONLINE;
						GoHereTimes=0;
					}
				}
		break;

		case DTU_INIT_DTUCFG:
				if(!GoHereTimes)
				{
					memcpy(&DTU_Device_Info.para_info,&DeviceInfoForCfg.para_info,sizeof(DTU_Device_Info.para_info));
					GoHereTimes=1;
				}
				tmp=DTU_cfg();
				if(0x88==tmp || 0x99==tmp)	GoHereTimes=0;

		break;

		case DTU_INIT_GPRSCFG:
					
		break;

		case DTU_NORMAL_OFFLINE:
		break;

		case DTU_NORMAL_ONLINE:
		/**/
		if(NewDataUpdate_Serial) {
			//配置命令
			if(strstr((const char*)USART_SERIAL_RX_BUF,"+++") && (USART_SERIAL_RX_LEN==3))
			{
//					UsartSendStr_serial("config mode", 12);
					DTU_CurrentState=DTU_NORMAL_SUSPEND;
			}
			else
			{
					UsartSendStr_network(USART_SERIAL_RX_BUF, USART_SERIAL_RX_LEN);
			}
			NewDataUpdate_Serial = 0;
		}

		/**/
		if(NewDataUpdate_Network) {
			//配置命令
			if(strstr((const char*)USART_NETWORK_RX_BUF,"+++") && (USART_NETWORK_RX_LEN==3))
			{
//					UsartSendStr_network("config mode", 12);
						DTU_CurrentState=DTU_NORMAL_SUSPEND;
			}
			else
			{
				UsartSendStr_serial(USART_NETWORK_RX_BUF, USART_NETWORK_RX_LEN);
			}
			NewDataUpdate_Network = 0;
		}
		break;

		case DTU_NORMAL_SUSPEND:
				if(!GoHereTimes)
				{
						delay_ms(500);//保证500ms内无数据
						UsartSendStr_network((vu8*)"+++", 3);
						GoHereTimes=1;
				}
				else
				{
						if(NewDataUpdate_Network)
						{
							if(gprs_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
							{
								DTU_CurrentState=DTU_INIT_DTUCFG;
								DTU_Device_Info.state_info.link_state=DTU_SUSPENDED;
								GoHereTimes=0;
							}
							else
							{
							}
							NewDataUpdate_Network = 0;
						}
				}	
		break;
				
		case DTU_NORMAL_REQEXITSUSPEND:
				if(!GoHereTimes)
				{
						delay_ms(500);//保证500ms内无数据
						UsartSendStr_network((vu8*)"ATO\r\n", 5);
						GoHereTimes=1;
				}
				else
				{
						if(NewDataUpdate_Network)
						{
							if(gprs_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
							{
								DTU_CurrentState=DTU_NORMAL_ONLINE;
								DTU_Device_Info.state_info.link_state=DTU_LINKED;
								GoHereTimes=0;
							}
							else
							{
							}
							NewDataUpdate_Network = 0;
						}
				}	
		break;

		default:
		break;
		}
}





/*******************************************************************************
* Function Name  :
* Description    :
* Input            : None
* Output         : None
* Return         : 0x88：配置结束
*******************************************************************************/
u8 DTU_cfg(void)
{ 
	char *para_point = NULL;
	char rx_buf[100];
	char tempbuf[100];

	if (NewDataUpdate_Serial)
	{
		memcpy(rx_buf,(const u8*)USART_SERIAL_RX_BUF,USART_SERIAL_RX_LEN);
		rx_buf[USART_SERIAL_RX_LEN]='\0';
		//
		if ((para_point=strstr((const char*)rx_buf, STR_DTU_CFG_BAUDRATESET)))
		{
			para_point += strlen(STR_DTU_CFG_BAUDRATESET);
			strcpy(tempbuf,para_point);
			//string to int
			DTU_Device_Info.para_info.baudrate = atoi((const char*)tempbuf);
		}
		//
		else if ((para_point=strstr((const char*)rx_buf, STR_DTU_CFG_PORTSET)))
		{
			para_point += strlen(STR_DTU_CFG_PORTSET);
			strcpy(tempbuf,para_point);
			if(strcmp((const char*)tempbuf,"RS232"))
			{
					DTU_Device_Info.para_info.port_mode=PORT_MODE_232;
			}
			else if(strcmp((const char*)tempbuf,"RS485"))
			{
					DTU_Device_Info.para_info.port_mode=PORT_MODE_485;
			}
		}
		//
		else if ((para_point=strstr((const char*)rx_buf, STR_DTU_CFG_HEARTTIMESET)))
		{
			para_point += strlen(STR_DTU_CFG_HEARTTIMESET);
			strcpy(tempbuf,para_point);
			//string to int
			DTU_Device_Info.para_info.hearttime = atoi((const char*)tempbuf);
		}
		//
		else if ((para_point=strstr((const char*)rx_buf, STR_DTU_CFG_GOGPRSCFG)))
		{

		}
		//
		else if ((para_point=strstr((const char*)rx_buf, STR_DTU_CFG_EXITCFGMODE)))
		{
			if((1) && (DTU_SUSPENDED==DTU_Device_Info.state_info.link_state))
			{
				DTU_CurrentState = DTU_NORMAL_REQEXITSUSPEND;
				return 0x99;
			}
		}
		//
		if ((para_point=strstr((const char*)rx_buf, STR_DTU_CFG_BAUDRATEGET)))
		{
			sprintf((char*)tempbuf, "%d", DTU_Device_Info.para_info.baudrate);
			UsartSendStr_serial((vu8*)tempbuf,strlen(tempbuf));
		}
		//
		else if ((para_point=strstr((const char*)rx_buf, STR_DTU_CFG_PORTGET)))
		{
			switch(DTU_Device_Info.para_info.port_mode)
			{
			case PORT_MODE_232:
				strcpy(tempbuf,"RS232");	
				break;
			case PORT_MODE_485:
				strcpy(tempbuf,"RS485");	
				break;
			default:
				break;
			}
			UsartSendStr_serial((vu8*)tempbuf,strlen(tempbuf));
		}
		//
		else if ((para_point=strstr((const char*)rx_buf, STR_DTU_CFG_HEARTTIMEGET)))
 		{
			sprintf((char*)tempbuf, "%d", DTU_Device_Info.para_info.hearttime);
			UsartSendStr_serial((vu8*)tempbuf,strlen(tempbuf));
		}
		//
		else if ((para_point=strstr((const char*)rx_buf, STR_DTU_CFG_SAVE)))
 		{
		  //将修改后的配置参数存入flash并应用到当前系统中
			
			memcpy(&DeviceInfoForCfg.para_info,&DTU_Device_Info.para_info,sizeof(DTU_Device_Info.para_info));
			return 0x88;
		}
	  //
		NewDataUpdate_Serial=0;
	}
	return 0;
}




/*******************************************************************************
* Function Name  : 
* Description    : sim900a模块复位----复位管脚PE4
									 复位后等待设备就绪（返回"RDY"字符串）------------>可以发送AT指令
									 继续等待网络就绪（返回"Call Ready"字符串）------->可以进行tcp连接
* Input            : None
* Output         : None
* Return         : None
*******************************************************************************/
void gprs_reset( void)
{
		GPRS_RESET = GPRS_RESET_ON;
		delay_ms( 300);
		GPRS_RESET = GPRS_RESET_OFF;
		delay_ms( 300);
}

/*******************************************************************************
* Function Name  : 
* Description    : sim900a发送命令后,检测接收到的应答
* Input          : str:被查找的字符串	
									 ack:期待的应答结果
* Output         : None
* Return         : 0,没有得到期待的应答结果
									 void DTU_HardwareInit(void)
*******************************************************************************/
u8* gprs_check_cmd(u8* str,u8* ack)
{
    char *strx = strstr((const char*)str, (const char*)ack);
	
    return (u8*)strx;
}

/*******************************************************************************
* Function Name  :
* Description    :
* Input            : None
* Output         : None
* Return         : None
*******************************************************************************/

void GPRS_cfg(u8 event)
{
		static u8 GoHereTimes=0;
		char server_info[100];		//用于存储服务器连接信息
    char temp_buf[20];
		u8 strforsend[100];

	//
    switch (GPRS_CfgState)
    {
    case GPRS_CFG_NULL:
			
        break;

    case GPRS_CFG_RESET:
			if(!GoHereTimes)
			{
				gprs_reset();
				GoHereTimes=1;
			}
			else
			{
				if(NewDataUpdate_Network) {
					if(gprs_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"SMS Ready"))
					{
						GPRS_CfgState=GPRS_CFG_RD_INFO;
						GoHereTimes=0;
					}
					else
					{
					}
					NewDataUpdate_Network = 0;
				}
			}
        break;

    case GPRS_CFG_RD_INFO:
			if(!GoHereTimes)
			{
				//read info from eep
					GoHereTimes=1;
			}
			else
			{
						GPRS_CfgState=GPRS_CFG_AT;
						GoHereTimes=0;
			}

        break;

    case GPRS_CFG_AT:
				if(!GoHereTimes)
				{
					//
					strcpy((char*)strforsend,(const char*)"AT\r\n");
					UsartSendStr_network(strforsend,strlen((const char*)strforsend));
					GoHereTimes=1;
				}
				else
				{
						if(NewDataUpdate_Network)
						{
							if(gprs_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
							{
								GPRS_CfgState=GPRS_CFG_ATE0;
								GoHereTimes=0;
							}
							else
							{
							}
							NewDataUpdate_Network = 0;
						}
				}

        break;

    case GPRS_CFG_ATE0:
				if(!GoHereTimes)
				{
					//
					strcpy((char*)strforsend,(const char*)"ATE0\r\n");
					UsartSendStr_network(strforsend,strlen((const char*)strforsend));
					GoHereTimes=1;
				}
				else
				{
						if(NewDataUpdate_Network)
						{
							if(gprs_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
							{
								GPRS_CfgState=GPRS_CFG_ATCSQ;
								GoHereTimes=0;
							}
							else
							{
							}
							NewDataUpdate_Network = 0;
						}
				}
        break;

    case GPRS_CFG_ATCSQ:
				if(!GoHereTimes)
				{
					//
					strcpy((char*)strforsend,(const char*)"AT+CSQ\r\n");
					UsartSendStr_network(strforsend,strlen((const char*)strforsend));
					GoHereTimes=1;
				}
				else
				{
						if(NewDataUpdate_Network)
						{
							if(gprs_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
							{
								GPRS_CfgState=GPRS_CFG_ATCREG;
								GoHereTimes=0;
							}
							else
							{
							}
							NewDataUpdate_Network = 0;
						}
				}
        break;

    case GPRS_CFG_ATCREG:
				if(!GoHereTimes)
				{
					//
					strcpy((char*)strforsend,(const char*)"AT+CREG?\r\n");
					UsartSendStr_network(strforsend,strlen((const char*)strforsend));
					GoHereTimes=1;
				}
				else
				{
						if(NewDataUpdate_Network)
						{
							if(gprs_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
							{
								GPRS_CfgState=GPRS_CFG_ATCGATT;
								GoHereTimes=0;
							}
							else
							{
							}
							NewDataUpdate_Network = 0;
						}
				}
        break;

    case GPRS_CFG_ATCGATT:
	if(!GoHereTimes)
				{
					//
					strcpy((char*)strforsend,(const char*)"AT+CGATT?\r\n");
					UsartSendStr_network(strforsend,strlen((const char*)strforsend));
					GoHereTimes=1;
				}
				else
				{
						if(NewDataUpdate_Network)
						{
							if(gprs_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
							{
								GPRS_CfgState=GPRS_CFG_ATCSTT;
								GoHereTimes=0;
							}
							else
							{
							}
							NewDataUpdate_Network = 0;
						}
				}
        break;

    case GPRS_CFG_ATCSTT:
				if(!GoHereTimes)
				{
					//
					sprintf((char*)temp_buf, "AT+CSTT=\"%s\"\r\n", (const char*)DTU_Device_Info.para_info.server_apn);
					strcpy((char*)strforsend,(const char*)temp_buf);
//					strcpy((char*)strforsend,(const char*)"AT+CSTT='CMNET'\r\n");
					UsartSendStr_network(strforsend,strlen((const char*)strforsend));
					GoHereTimes=1;
				}
				else
				{
						if(NewDataUpdate_Network)
						{
							if(gprs_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
							{
								GPRS_CfgState=GPRS_CFG_ATCIICR;
								GoHereTimes=0;
							}
							else
							{
								GoHereTimes=0;
							}
							NewDataUpdate_Network = 0;
						}
				}
        break;

    case GPRS_CFG_ATCIICR:
				if(!GoHereTimes)
				{
					//
					strcpy((char*)strforsend,(const char*)"AT+CIICR\r\n");
					UsartSendStr_network(strforsend,strlen((const char*)strforsend));
					GoHereTimes=1;
				}
				else
				{
						if(NewDataUpdate_Network)
						{
							if(gprs_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
							{
								GPRS_CfgState=GPRS_CFG_ATCIFSR;
								GoHereTimes=0;
							}
							else
							{
							}
							NewDataUpdate_Network = 0;
						}
				}
        break;

    case GPRS_CFG_ATCIFSR:
				if(!GoHereTimes)
				{
					//
					strcpy((char*)strforsend,(const char*)"AT+CIFSR\r\n");
					UsartSendStr_network(strforsend,strlen((const char*)strforsend));
					GoHereTimes=1;
				}
				else
				{
						if(NewDataUpdate_Network)
						{
							if(gprs_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
							{
								GPRS_CfgState=GPRS_CFG_ATCIPSHUT;
								GoHereTimes=0;
							}
							else
							{
							}
							NewDataUpdate_Network = 0;
						}
				}
        break;

    case GPRS_CFG_ATCIPSHUT:
				if(!GoHereTimes)
				{
					//
					strcpy((char*)strforsend,(const char*)"AT+CIPSHUT\r\n");
					UsartSendStr_network(strforsend,strlen((const char*)strforsend));
					GoHereTimes=1;
				}
				else
				{
						if(NewDataUpdate_Network)
						{
							if(gprs_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"SHUT OK"))
							{
								GPRS_CfgState=GPRS_CFG_ATCIPMODE;
								GoHereTimes=0;
							}
							else
							{
							}
							NewDataUpdate_Network = 0;
						}
				}
        break;

    case GPRS_CFG_ATCIPMODE:
				if(!GoHereTimes)
				{
					//
					strcpy((char*)strforsend,(const char*)"AT+CIPMODE=1\r\n");
					UsartSendStr_network(strforsend,strlen((const char*)strforsend));
					GoHereTimes=1;
				}
				else
				{
						if(NewDataUpdate_Network)
						{
							if(gprs_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
							{
								GPRS_CfgState=GPRS_CFG_ATCIPMUX;
								GoHereTimes=0;
							}
							else
							{
							}
							NewDataUpdate_Network = 0;
						}
				}
        break;
				
	 case GPRS_CFG_ATCIPMUX:
					if(!GoHereTimes)
					{
						//
						strcpy((char*)strforsend,(const char*)"AT+CIPMUX=0\r\n");
						UsartSendStr_network(strforsend,strlen((const char*)strforsend));
						GoHereTimes=1;
					}
					else
					{
							if(NewDataUpdate_Network)
							{
								if(gprs_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
								{
									GPRS_CfgState=GPRS_CFG_ATCIPSTART;
									GoHereTimes=0;
								}
								else
								{
								}
								NewDataUpdate_Network = 0;
							}
					}
					break;

    case GPRS_CFG_ATCIPSTART:
				if(!GoHereTimes)
				{
					//
					sprintf((char*)server_info, "AT+CIPSTART=\"%s\",\"%s\",\"%s\"\r\n", \
																			"TCP", (const char*)DTU_Device_Info.para_info.server_ipaddr, (const char*)DTU_Device_Info.para_info.server_port);
					strcpy((char*)strforsend,(const char*)server_info);
					UsartSendStr_network(strforsend,strlen((const char*)strforsend));
					GoHereTimes=1;
				}
				else
				{
						if(NewDataUpdate_Network)
						{
							if(gprs_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"CONNECT OK"))
							{
								GPRS_CfgState=GPRS_CFG_NULL;
								DTU_Device_Info.state_info.link_state=DTU_LINKED;
								GoHereTimes=0;
							}
							else
							{
							}
							NewDataUpdate_Network = 0;
						}
				}
        break;

    default:
        break;
    }
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input            : None
* Output         : None
* Return         : None
*******************************************************************************/
void ReadDTUInfoFromFlash(DTU_DEVICE_INFO* dev)
{
	
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input            : None
* Output         : None
* Return         : None
*******************************************************************************/
void WriteDTUInfoToFlash(DTU_DEVICE_INFO dev)
{
	
}
































