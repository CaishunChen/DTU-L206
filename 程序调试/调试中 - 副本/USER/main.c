#include "delay.h"
#include "gpio.h"
#include "sys.h"
#include "usart.h"
#include "timer4_usart.h"
#include "sim900a.h"
#include "string.h"
#include "stdlib.h"




vu8 NewDataUpdate_Serial = 0; /*串口端数据更新*/
vu8 NewDataUpdate_Network = 0; /*网络端数据更新*/

//
vu8 DTU_CurrentState = DTU_INIT_CHECKSTA;
//
vu8 GPRS_CfgState = GPRS_CFG_NULL;
//
vu8 DTU_CfgState = DTU_CFG_NULL;

//
DTU_DEVICE_INFO DTU_Device_Info=
		{115200,
		PORT_MODE_485,
		200,
			
		"115.159.180.213",
		"502",
		"CMNET",
			
		DTU_PWRISOFF,
		DTU_INITNONE,
		DTU_UNLINKED
		};



u8 tempint=0;
u8 temp_buf[100] = {0};

extern void DTU_Loop(void);
extern void DTU_cfg(void);
extern void GPRS_cfg(u8 event);
extern void ReadDTUInfoFromFlash(DTU_DEVICE_INFO* dev);




u8* tempp=NULL;
u8 str1[10],str2[10];
u8 strforsend[100];
int main(void)
{
	//
    delay_init();	    	 //
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    uart_init_serial(115200);	 //
    uart_init_network(115200);	 //
    TIM3_Int_Init(100 - 1, 7199); //100us * n
    TIM4_Int_Init(100 - 1, 7199); //100us * n
		Gpio_Init();
	
//		tempp = sim900a_check_cmd("df sdf Ready","SMS Ready");

//		strcpy((char*)strforsend,(const char*)"AT\r\n");
//				tempint=	strlen((const char*)strforsend);
//	
	
//	while(1)
//	{
//		DTU_cfg();
//	}
	
//	while(1)
//	{
////		GPRS_cfg(0);
//	
//		UsartSendStr_network(temp_buf,8);
//		delay_ms(200);
//	  UsartSendStr_serial(temp_buf, 8);
//		delay_ms(200);

//	}
	

		while(1)
    {
				DTU_Loop();

    }
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
						if(sim900a_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"Ready"))
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
					DTU_cfg();
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
							if(sim900a_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
							{
								DTU_CurrentState=DTU_INIT_DTUCFG;
									DTU_Device_Info.link_state=DTU_SUSPENDED;
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
							if(sim900a_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
							{
								DTU_CurrentState=DTU_NORMAL_ONLINE;
									DTU_Device_Info.link_state=DTU_LINKED;
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


//u32 DTU_Device_Info.baudrate=115200;
//u8 DTU_Device_Info.port_mode=PORT_MODE_485;
//u8 DTU_Device_Info.hearttime=100;
void String2Int(u8* str,u32* i)
{}
void Int2String(u32 i,u8* str)//以'\0'结尾
{}



/*******************************************************************************
* Function Name  :
* Description    :
* Input            : None
* Output         : None
* Return         : None
*******************************************************************************/
void DTU_cfg(void)
{ 
	char *para_point = NULL;
	char rx_buf[100];
	char tempbuf[100];

	if (NewDataUpdate_Serial)
	{
		memcpy(rx_buf,(const u8*)USART_SERIAL_RX_BUF,USART_SERIAL_RX_LEN);
		rx_buf[USART_SERIAL_RX_LEN]='\0';
		//
		//
		if ((para_point=strstr((const char*)rx_buf, STR_DTU_CFG_BAUDRATESET)))
		{
			para_point += strlen(STR_DTU_CFG_BAUDRATESET);
			strcpy(tempbuf,para_point);
			//string to int
			DTU_Device_Info.baudrate = atoi((const char*)tempbuf);
		}
		//
		else if ((para_point=strstr((const char*)rx_buf, STR_DTU_CFG_PORTSET)))
		{
			para_point += strlen(STR_DTU_CFG_PORTSET);
			strcpy(tempbuf,para_point);
			if(strcmp((const char*)tempbuf,"RS232"))
			{
					DTU_Device_Info.port_mode=PORT_MODE_232;
			}
			else if(strcmp((const char*)tempbuf,"RS485"))
			{
					DTU_Device_Info.port_mode=PORT_MODE_485;
			}
		}
		//
		else if ((para_point=strstr((const char*)rx_buf, STR_DTU_CFG_HEARTTIMESET)))
		{
			para_point += strlen(STR_DTU_CFG_HEARTTIMESET);
			strcpy(tempbuf,para_point);
			//string to int
			DTU_Device_Info.hearttime = atoi((const char*)tempbuf);
		}
		//
		else if ((para_point=strstr((const char*)rx_buf, STR_DTU_CFG_GOGPRSCFG)))
		{

		}
		//
		else if ((para_point=strstr((const char*)rx_buf, STR_DTU_CFG_EXITCFGMODE)))
		{
			if((1) && (DTU_SUSPENDED==DTU_Device_Info.link_state))
			{
				DTU_CurrentState = DTU_NORMAL_REQEXITSUSPEND;
			}
		}
		
		
		
		//
		if ((para_point=strstr((const char*)USART_SERIAL_RX_BUF, STR_DTU_CFG_BAUDRATEGET)))
		{
			sprintf((char*)tempbuf, "%d", DTU_Device_Info.baudrate);
			UsartSendStr_serial((vu8*)tempbuf,strlen(tempbuf));
		}
		//
		else if ((para_point=strstr((const char*)USART_SERIAL_RX_BUF, STR_DTU_CFG_PORTGET)))
		{
			switch(DTU_Device_Info.port_mode)
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
		else if ((para_point=strstr((const char*)USART_SERIAL_RX_BUF, STR_DTU_CFG_HEARTTIMEGET)))
		{
			sprintf((char*)tempbuf, "%d", DTU_Device_Info.hearttime);
			UsartSendStr_serial((vu8*)tempbuf,strlen(tempbuf));
		}
		
	  //
		NewDataUpdate_Serial=0;
	}
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
					if(sim900a_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"SMS Ready"))
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
							if(sim900a_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
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
							if(sim900a_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
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
							if(sim900a_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
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
							if(sim900a_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
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
							if(sim900a_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
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
					sprintf((char*)temp_buf, "AT+CSTT=\"%s\"\r\n", Server_APN);
					strcpy((char*)strforsend,(const char*)temp_buf);
//					strcpy((char*)strforsend,(const char*)"AT+CSTT='CMNET'\r\n");
					UsartSendStr_network(strforsend,strlen((const char*)strforsend));
					GoHereTimes=1;
				}
				else
				{
						if(NewDataUpdate_Network)
						{
							if(sim900a_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
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
							if(sim900a_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
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
							if(sim900a_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
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
							if(sim900a_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"SHUT OK"))
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
							if(sim900a_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
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
								if(sim900a_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"OK"))
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
																			"TCP", Server_IpAddr, Server_Port);
					strcpy((char*)strforsend,(const char*)server_info);
					UsartSendStr_network(strforsend,strlen((const char*)strforsend));
					GoHereTimes=1;
				}
				else
				{
						if(NewDataUpdate_Network)
						{
							if(sim900a_check_cmd((u8*)USART_NETWORK_RX_BUF,(u8*)"CONNECT OK"))
							{
								GPRS_CfgState=GPRS_CFG_NULL;
									DTU_Device_Info.link_state=DTU_LINKED;
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









