#include "sim900a.h"
#include "delay.h"
#include "string.h"
#include "gpio.h"



const char *Server_IpAddr = "115.159.180.213";	//服务器ip
const char *Server_Port = "502";							//服务器监听端口
const char *Server_APN = "CMNET";							//****/


/*sim900a模块复位----复位管脚PE4
**复位后等待设备就绪（返回"RDY"字符串）------------>可以发送AT指令
**继续等待网络就绪（返回"Call Ready"字符串）------->可以进行tcp连接
*/
void gprs_reset( void)
{
		GPRS_RESET = GPRS_RESET_ON;
		delay_ms( 300);
		GPRS_RESET = GPRS_RESET_OFF;
		delay_ms( 300);
}
/*
**sim900a根据ip和port连接服务器
**输入： none
**返回值：0-连接成功			1-连接失败
*/
int sim900a_connect( void)
{
    char server_info[100];		//用于存储服务器连接信息
    char temp_buf[20];

    //连接模块
    //检测是否应答AT指令
//    lcd_text24(300, 100, BLACK, WHITE, "连接设备. . .");
    while(sim900a_send_cmd("AT", "OK", 100));
    //不回显
    sim900a_send_cmd("ATE0", "OK", 200);
//    lcd_text24(300, 140, BLACK, WHITE, "连接成功！");
    //查询注册状态----+CGREG:0,1表示已注册，本网
//    lcd_text24(300, 180, BLACK, WHITE, "正在注册网络. . .");
    while(sim900a_send_cmd("AT+CGREG?", "+CGREG: 0,1", 500));
    //查询GPRS附着状态----+CGATT:1表示已附着
//    lcd_text24(300, 220, BLACK, WHITE, "正在附着网络. . .");
    while(sim900a_send_cmd("AT+CGATT?", "+CGATT: 1", 500));
    //AT+CSTT
    sprintf((char*)temp_buf, "AT+CSTT=\"%s\"", Server_APN);
//    lcd_text24(300, 260, BLACK, WHITE, "正在设置APN接入点. . .");
    while(sim900a_send_cmd(temp_buf, "OK", 500));
    //AT+CIICR
//    lcd_text24(300, 300, BLACK, WHITE, "正在启动移动场景. . .");
    while(sim900a_send_cmd("AT+CIICR", "OK", 500));
    //AT+CIFSR
//    lcd_text24(300, 340, BLACK, WHITE, "获取设备IP地址. . .");
    while(sim900a_send_cmd("AT+CIFSR", ".", 500));
    //CIPSHUT关闭之前连接---SHUT OK
//    lcd_text24(300, 380, BLACK, WHITE, "关闭无效连接. . .");
    while(sim900a_send_cmd("AT+CIPSHUT", "SHUT OK", 500));
    //CIPMUX配置-----单点连接
//    lcd_text24(300, 410, BLACK, WHITE, "设置单点连接. . .");
    while(sim900a_send_cmd("AT+CIPMUX=0", "OK", 500));
    //CIPMODE配置---透传模式(后面的CIPCCFG暂时使用默认参数)
//    lcd_text24(300, 440, BLACK, WHITE, "设置透传模式. . .");
    while(sim900a_send_cmd("AT+CIPMODE=1", "OK", 500));
    //建立连接
    sprintf((char*)server_info, "AT+CIPSTART=\"%s\",\"%s\",\"%s\"", \
			"TCP", Server_IpAddr, Server_Port);
    //发起连接,设备返回OK之后还有CONNECT，干脆直接等待CONNECT字符串
    if(sim900a_send_cmd(server_info, "CONNECT", 2000)) return -1;
//    lcd_text24(500, 450, BLACK, WHITE, "已成功连接到服务器");
return 0;
}
/*sim900a模块初始化
**初始化成功（指与设备通信成功，不确保能成功连接服务器）前不会退出此函数
**输入： none
**返回值：0-连接服务器成功			1-连接服务器失败
*/
int sim900a_init( void)
{

//    GPIO_InitTypeDef  GPIO_InitStructure;

//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOE、F时钟

//    /****************GPIOF9,F10作为数字量输入*********************/
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOE4
//    //复位模块
//    sim900a_reset( );
//    //连接模块
//    if( sim900a_connect( ))return -1;
    return 0;
}


//sim900a发送命令后,检测接收到的应答
//str:被查找的字符串
//ack:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* sim900a_check_cmd(u8* str,u8* ack)
{
    char *strx = 0;
	//
		strx = strstr((const char*)str, (const char*)ack);
	
    return (u8*)strx;
}
//向sim900a发送命令
//cmd:发送的命令字符串(自动添加'\r''\n'),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
    u8 res = 0;
//    USART_SERIAL_RX_STA = 0;
//    if((u32)cmd <= 0XFF)
//    {
////        USART_SendStr( USART2, (char*)cmd);
//    } else	u1_printf("%s\r\n", cmd); //发送命令
//    
    return res;
}












