#include "sim900a.h"
#include "delay.h"
#include "string.h"
#include "gpio.h"



const char *Server_IpAddr = "115.159.180.213";	//������ip
const char *Server_Port = "502";							//�����������˿�
const char *Server_APN = "CMNET";							//****/


/*sim900aģ�鸴λ----��λ�ܽ�PE4
**��λ��ȴ��豸����������"RDY"�ַ�����------------>���Է���ATָ��
**�����ȴ��������������"Call Ready"�ַ�����------->���Խ���tcp����
*/
void gprs_reset( void)
{
		GPRS_RESET = GPRS_RESET_ON;
		delay_ms( 300);
		GPRS_RESET = GPRS_RESET_OFF;
		delay_ms( 300);
}
/*
**sim900a����ip��port���ӷ�����
**���룺 none
**����ֵ��0-���ӳɹ�			1-����ʧ��
*/
int sim900a_connect( void)
{
    char server_info[100];		//���ڴ洢������������Ϣ
    char temp_buf[20];

    //����ģ��
    //����Ƿ�Ӧ��ATָ��
//    lcd_text24(300, 100, BLACK, WHITE, "�����豸. . .");
    while(sim900a_send_cmd("AT", "OK", 100));
    //������
    sim900a_send_cmd("ATE0", "OK", 200);
//    lcd_text24(300, 140, BLACK, WHITE, "���ӳɹ���");
    //��ѯע��״̬----+CGREG:0,1��ʾ��ע�ᣬ����
//    lcd_text24(300, 180, BLACK, WHITE, "����ע������. . .");
    while(sim900a_send_cmd("AT+CGREG?", "+CGREG: 0,1", 500));
    //��ѯGPRS����״̬----+CGATT:1��ʾ�Ѹ���
//    lcd_text24(300, 220, BLACK, WHITE, "���ڸ�������. . .");
    while(sim900a_send_cmd("AT+CGATT?", "+CGATT: 1", 500));
    //AT+CSTT
    sprintf((char*)temp_buf, "AT+CSTT=\"%s\"", Server_APN);
//    lcd_text24(300, 260, BLACK, WHITE, "��������APN�����. . .");
    while(sim900a_send_cmd(temp_buf, "OK", 500));
    //AT+CIICR
//    lcd_text24(300, 300, BLACK, WHITE, "���������ƶ�����. . .");
    while(sim900a_send_cmd("AT+CIICR", "OK", 500));
    //AT+CIFSR
//    lcd_text24(300, 340, BLACK, WHITE, "��ȡ�豸IP��ַ. . .");
    while(sim900a_send_cmd("AT+CIFSR", ".", 500));
    //CIPSHUT�ر�֮ǰ����---SHUT OK
//    lcd_text24(300, 380, BLACK, WHITE, "�ر���Ч����. . .");
    while(sim900a_send_cmd("AT+CIPSHUT", "SHUT OK", 500));
    //CIPMUX����-----��������
//    lcd_text24(300, 410, BLACK, WHITE, "���õ�������. . .");
    while(sim900a_send_cmd("AT+CIPMUX=0", "OK", 500));
    //CIPMODE����---͸��ģʽ(�����CIPCCFG��ʱʹ��Ĭ�ϲ���)
//    lcd_text24(300, 440, BLACK, WHITE, "����͸��ģʽ. . .");
    while(sim900a_send_cmd("AT+CIPMODE=1", "OK", 500));
    //��������
    sprintf((char*)server_info, "AT+CIPSTART=\"%s\",\"%s\",\"%s\"", \
			"TCP", Server_IpAddr, Server_Port);
    //��������,�豸����OK֮����CONNECT���ɴ�ֱ�ӵȴ�CONNECT�ַ���
    if(sim900a_send_cmd(server_info, "CONNECT", 2000)) return -1;
//    lcd_text24(500, 450, BLACK, WHITE, "�ѳɹ����ӵ�������");
return 0;
}
/*sim900aģ���ʼ��
**��ʼ���ɹ���ָ���豸ͨ�ųɹ�����ȷ���ܳɹ����ӷ�������ǰ�����˳��˺���
**���룺 none
**����ֵ��0-���ӷ������ɹ�			1-���ӷ�����ʧ��
*/
int sim900a_init( void)
{

//    GPIO_InitTypeDef  GPIO_InitStructure;

//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOE��Fʱ��

//    /****************GPIOF9,F10��Ϊ����������*********************/
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ģʽ
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
//    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOE4
//    //��λģ��
//    sim900a_reset( );
//    //����ģ��
//    if( sim900a_connect( ))return -1;
    return 0;
}


//sim900a���������,�����յ���Ӧ��
//str:�����ҵ��ַ���
//ack:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
u8* sim900a_check_cmd(u8* str,u8* ack)
{
    char *strx = 0;
	//
		strx = strstr((const char*)str, (const char*)ack);
	
    return (u8*)strx;
}
//��sim900a��������
//cmd:���͵������ַ���(�Զ����'\r''\n'),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
    u8 res = 0;
//    USART_SERIAL_RX_STA = 0;
//    if((u32)cmd <= 0XFF)
//    {
////        USART_SendStr( USART2, (char*)cmd);
//    } else	u1_printf("%s\r\n", cmd); //��������
//    
    return res;
}












