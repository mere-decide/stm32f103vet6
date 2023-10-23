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
	printf("�������\r\n");
	printf("www.jcznedu.com\r\n");
	printf("���ڳ�ʼ���ɹ�\r\n");
	W5500_Config();
	
	W5500_ChipConfig();
	W5500_NetInfoConfig();
	
	while(1)
	{		
		switch(getSn_SR(0))																//��ȡsocket0��״̬	
		{																									
			case SOCK_CLOSED:                               //Socket���ڹر�״̬
					//�����׽���
					socket(0,Sn_MR_TCP,NetInforStr.Server.Port,Sn_MR_ND); 
				break;
			case SOCK_INIT:																	//Socket���ڳ�ʼ�����(��)״̬
					//����Sn_CRΪCONNECT������TCP������������������
					printf("Connect\r\n");
					printf("%d\n",connect(0, NetInforStr.Server.NetInfor.ip,NetInforStr.Server.Port));
					Delay_Nopnms(1000);
				break;
			case SOCK_ESTABLISHED:													//Socket�������ӽ���״̬
				
					if(getSn_IR(0) & Sn_IR_CON) 
					{
						printf("Connect ok\r\n");
						setSn_IR(0, Sn_IR_CON);										//Sn_IR��CONλ��1��֪ͨW5500�����ѽ��� 
					}	
					
					W5500Data.Len = getSn_RX_RSR(0);	//��ȡ���ݳ���
					if(W5500Data.Len > 2) 
					{
						recv(0,W5500Data.Data,W5500Data.Len);		  //W5500�������Է����������ݣ���ͨ��SPI���͸�MCU
						
						printf("get w5500 data:*\r\n%s\r\n*\r\n",W5500Data.Data);
						send(0,"�������",9);
						
						memset(W5500Data.Data,0,W5500BufMAX);
						W5500Data.Len = 0;
					}
				break;
			case SOCK_CLOSE_WAIT:												 		//Socket���ڵȴ��ر�״̬  
						close(0);           
				break;
		}
	}
}





