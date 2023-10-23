#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "stdio.h"
#include "w5500_port.h"

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	USART_Config();
	printf("金橙智能\r\n");
	printf("www.jcznedu.com\r\n");
	printf("串口初始化成功\r\n");
	W5500_Config();
	
	W5500_ChipConfig();
	W5500_NetInfoConfig();
	
	while(1)
	{		
		switch(getSn_SR(0))																//获取socket0的状态	
		{																									
			case SOCK_CLOSED:                               //Socket处于关闭状态
					//建立套接字
					socket(0,Sn_MR_TCP,NetInforStr.Server.Port,Sn_MR_ND); 
				break;
			case SOCK_INIT:																	//Socket处于初始化完成(打开)状态
					//配置Sn_CR为CONNECT，并向TCP服务器发出连接请求
					printf("Connect\r\n");
					printf("%d\n",connect(0, NetInforStr.Server.NetInfor.ip,NetInforStr.Server.Port));
					Delay_Nopnms(1000);
				break;
			case SOCK_ESTABLISHED:													//Socket处于连接建立状态
				
					if(getSn_IR(0) & Sn_IR_CON) 
					{
						printf("Connect ok\r\n");
						setSn_IR(0, Sn_IR_CON);										//Sn_IR的CON位置1，通知W5500连接已建立 
					}	
					
					W5500Data.Len = getSn_RX_RSR(0);	//获取数据长度
					if(W5500Data.Len > 2) 
					{
						recv(0,W5500Data.Data,W5500Data.Len);		  //W5500接收来自服务器的数据，并通过SPI发送给MCU
						
						printf("get w5500 data:*\r\n%s\r\n*\r\n",W5500Data.Data);
						send(0,"金橙智能",9);
						
						memset(W5500Data.Data,0,W5500BufMAX);
						W5500Data.Len = 0;
					}
				break;
			case SOCK_CLOSE_WAIT:												 		//Socket处于等待关闭状态  
						close(0);           
				break;
		}
	}
}





