#include "delay.h"
#include "gpio.h"
#include "sys.h"
#include "usart.h"
#include "tmr_usart.h"
#include "string.h"
#include "stdlib.h"
#include "DTU.h"



//u8 tempint=0;
//u8 temp_buf[100] = {0};
//u8* tempp=NULL;
//u8 str1[10],str2[10];
//u8 strforsend[100];




int main(void)
{
	//
    delay_init();	    	 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	 
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











