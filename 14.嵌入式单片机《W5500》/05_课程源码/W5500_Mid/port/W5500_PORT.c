#include "w5500_port.h"

//��ʼ�����ö���--����Ĭ����Ϣ
_NetStruct NetInforStr =
{
    //�ͻ���������Ϣ--�Լ�����Ϣ
    .Client.NetInfor.mac = {0x60, 0x14, 0xb3, 0x79, 0x10, 0x66},
    .Client.NetInfor.ip =  {192, 168, 137, 188},
    .Client.NetInfor.sn =  {255, 255, 255, 0},
    .Client.NetInfor.gw =  {192, 168, 137, 1},
    .Client.NetInfor.dns = {180, 76, 76, 76},
    .Client.NetInfor.dhcp = NETINFO_DHCP,
    .Client.Port = 8886,
    //������������Ϣ 122.114.122.174
    .Server.NetInfor.ip =  {122,114,122,174},
    .Server.Port = 33097,
};

_W5500GetData W5500Data = {"\0"};
/****************************
�������ƣ�W5500_Config
�������ã���̫��ģ���ʼ��
������ڣ���
�������ڣ���
�������ߣ�Ӳ��-����
����ʱ�䣺2021.08.06 15:56
�޸�ʱ�䣺2021.08.06 15:56
****************************/
void W5500_Config(void)
{
    W5500_PortConfig();
    W5500_RegFunction();
}

/****************************
�������ƣ�W5500_Reset
�������ã�W5500��λ���ú���
������ڣ���
�������ڣ���
�������ߣ�Ӳ��-����
����ʱ�䣺2021.08.12 13:44
�޸�ʱ�䣺2021.08.12 13:44
****************************/
void W5500_Reset(void)
{
    W5500_RESET(0);
    Delay_Nopnus(2);
    W5500_RESET(1);
}

/******��̫���ܽų�ʼ��******/
void W5500_PortConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	//GPIO�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//RST
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//CS
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//INT
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;//MISO
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//�ⲿ�ж�����
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource8);
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	NVIC_SetPriority(EXTI9_5_IRQn, 3);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	//SPI1 SP0
	SPI_StructInit(&SPI_InitStructure);
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//������2��Ƶ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//ǰ�ز���
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//���е͵�ƽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//8����
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//˫��˫��
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//����ģʽ
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//�������
	SPI_Init(SPI2, &SPI_InitStructure);
	//SPIʹ��
	SPI_Cmd(SPI2, ENABLE);
	W5500_RESET(1);
}

/******��̫��ģ��д�ֽ�******/
void W5500_WriteData(uint8_t  Data)
{
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI2, Data);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    SPI_I2S_ReceiveData(SPI2);
}
/******��̫��ģ����ֽ�******/
uint8_t W5500_ReadData(void)
{
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI2, 0XFF);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI2);
}
/******��̫���ⲿ�жϷ�����******/
void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line8) == SET)
    {
        EXTI_ClearITPendingBit(EXTI_Line8);

    }
}

void SPI_CrisEnter(void)
{
    __set_PRIMASK(1);   //�����ٽ���
}
void SPI_CrisExit(void)
{
    __set_PRIMASK(0);   //�˳��ٽ���
}
void SPI_CS_Select(void)
{
    W5500_SPICS(0);
}
void SPI_CS_Deselect(void)
{
    W5500_SPICS(1);
}
//ע��TCPͨѶ��صĻص�����
void W5500_RegFunction(void)
{
    reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);    //ע���ٽ����ص�����
#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
    reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);//ע��Ƭѡ�ص�����
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
    reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);
#else
#if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
#error "Unknown _WIZCHIP_IO_MODE_"
#else
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
#endif
#endif
    reg_wizchip_spi_cbfunc(W5500_ReadData, W5500_WriteData);    //SPI��д�ֽڻص�����
}

//��ʼ��оƬ����
void W5500_ChipConfig(void)
{
    uint8_t tmp = '\0';
    uint8_t memsize[2][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};
    if(ctlwizchip(CW_INIT_WIZCHIP, (void *)memsize) == -1)
    {
        while(1);
    }
    do
    {
        if(ctlwizchip(CW_GET_PHYLINK, (void *)&tmp) == -1)
				{
				;
				}
    }
    while(tmp == PHY_LINK_OFF);
}
//��ʼ������ͨѶ����,mac,ip��
void W5500_NetInfoConfig(void)
{
    uint8_t tmpstr[6];
    ctlnetwork(CN_SET_NETINFO, (void *)&NetInforStr.Client.NetInfor);			//����Ĭ��������Ϣ
    ctlnetwork(CN_GET_NETINFO, (void *)&NetInforStr.Client.NetInfor);	//��ȡ������Ϣ
    ctlwizchip(CW_GET_ID, (void *)tmpstr);									//��ȡPHYоƬID
    //���ڴ�ӡ������
    printf("\r\n=== %s SET NET CONF ===\r\n",(char*)tmpstr);
    printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",NetInforStr.Client.NetInfor.mac[0],NetInforStr.Client.NetInfor.mac[1],NetInforStr.Client.NetInfor.mac[2],
            NetInforStr.Client.NetInfor.mac[3],NetInforStr.Client.NetInfor.mac[4],NetInforStr.Client.NetInfor.mac[5]);
    printf("SIP: %d.%d.%d.%d\r\n", NetInforStr.Client.NetInfor.ip[0],NetInforStr.Client.NetInfor.ip[1],NetInforStr.Client.NetInfor.ip[2],NetInforStr.Client.NetInfor.ip[3]);
    printf("GAR: %d.%d.%d.%d\r\n", NetInforStr.Client.NetInfor.gw[0],NetInforStr.Client.NetInfor.gw[1],NetInforStr.Client.NetInfor.gw[2],NetInforStr.Client.NetInfor.gw[3]);
    printf("SUB: %d.%d.%d.%d\r\n", NetInforStr.Client.NetInfor.sn[0],NetInforStr.Client.NetInfor.sn[1],NetInforStr.Client.NetInfor.sn[2],NetInforStr.Client.NetInfor.sn[3]);
    printf("DNS: %d.%d.%d.%d\r\n", NetInforStr.Client.NetInfor.dns[0],NetInforStr.Client.NetInfor.dns[1],NetInforStr.Client.NetInfor.dns[2],NetInforStr.Client.NetInfor.dns[3]);
}

/*
void Task_W5500data(void *p)
{
	W5500_ChipConfig();
	W5500_NetInfoConfig();

	while(1)
	{
		switch(getSn_SR(0))																//��ȡsocket0��״̬	
		{																									
			case SOCK_CLOSED:                               //Socket���ڹر�״̬
					//�����׽���
					socket(0,Sn_MR_TCP,++NetInforStr.Server.Port,Sn_MR_ND); 
					printf("Close %d\r\n",NetInforStr.Server.Port);
				break;
			case SOCK_INIT:																	//Socket���ڳ�ʼ�����(��)״̬
					//����Sn_CRΪCONNECT������TCP������������������
			printf("Connect\r\n");
					printf("%d\r\n",connect(0, NetInforStr.Server.NetInfor.ip,33097));
			vTaskDelay(1000);
				break;
			case SOCK_ESTABLISHED:													//Socket�������ӽ���״̬
				printf("Co ok\r\n");
					if(getSn_IR(0) & Sn_IR_CON) 
					{
						setSn_IR(0, Sn_IR_CON);										//Sn_IR��CONλ��1��֪ͨW5500�����ѽ��� 
					}	
					
//					W5500Data.Len = getSn_RX_RSR(0);	//��ȡ���ݳ���
//					if(W5500Data.Len > 2) 
//					{
//						recv(0,W5500Data.Data,W5500Data.Len);		  //W5500�������Է����������ݣ���ͨ��SPI���͸�MCU
//							
//						memset(W5500Data.Data,0,W5500BufMAX);
//						W5500Data.Len = 0;
//					}
				break;
			case SOCK_CLOSE_WAIT:												 		//Socket���ڵȴ��ر�״̬  
						close(0);           
				break;
		}
		
		vTaskDelay(100);
	}
}
*/