#include "w5500_port.h"

//初始化配置定义--网关默认信息
_NetStruct NetInforStr =
{
    //客户机基本信息--自己的信息
    .Client.NetInfor.mac = {0x60, 0x14, 0xb3, 0x79, 0x10, 0x66},
    .Client.NetInfor.ip =  {192, 168, 137, 188},
    .Client.NetInfor.sn =  {255, 255, 255, 0},
    .Client.NetInfor.gw =  {192, 168, 137, 1},
    .Client.NetInfor.dns = {180, 76, 76, 76},
    .Client.NetInfor.dhcp = NETINFO_DHCP,
    .Client.Port = 8886,
    //服务器基本信息 122.114.122.174
    .Server.NetInfor.ip =  {122,114,122,174},
    .Server.Port = 33097,
};

_W5500GetData W5500Data = {"\0"};
/****************************
函数名称：W5500_Config
函数作用：以太网模块初始化
函数入口：无
函数出口：无
函数作者：硬件-王玉川
创建时间：2021.08.06 15:56
修改时间：2021.08.06 15:56
****************************/
void W5500_Config(void)
{
    W5500_PortConfig();
    W5500_RegFunction();
}

/****************************
函数名称：W5500_Reset
函数作用：W5500复位设置函数
函数入口：无
函数出口：无
函数作者：硬件-王玉川
创建时间：2021.08.12 13:44
修改时间：2021.08.12 13:44
****************************/
void W5500_Reset(void)
{
    W5500_RESET(0);
    Delay_Nopnus(2);
    W5500_RESET(1);
}

/******以太网管脚初始化******/
void W5500_PortConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	//GPIO端口配置
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
	//外部中断配置
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
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//波特率2分频
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//前沿采样
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//空闲低电平
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//8数据
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//双线双向
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//主机模式
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//软件触发
	SPI_Init(SPI2, &SPI_InitStructure);
	//SPI使能
	SPI_Cmd(SPI2, ENABLE);
	W5500_RESET(1);
}

/******以太网模块写字节******/
void W5500_WriteData(uint8_t  Data)
{
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI2, Data);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    SPI_I2S_ReceiveData(SPI2);
}
/******以太网模块读字节******/
uint8_t W5500_ReadData(void)
{
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI2, 0XFF);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI2);
}
/******以太网外部中断服务函数******/
void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line8) == SET)
    {
        EXTI_ClearITPendingBit(EXTI_Line8);

    }
}

void SPI_CrisEnter(void)
{
    __set_PRIMASK(1);   //进入临界区
}
void SPI_CrisExit(void)
{
    __set_PRIMASK(0);   //退出临界区
}
void SPI_CS_Select(void)
{
    W5500_SPICS(0);
}
void SPI_CS_Deselect(void)
{
    W5500_SPICS(1);
}
//注册TCP通讯相关的回调函数
void W5500_RegFunction(void)
{
    reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);    //注册临界区回调函数
#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
    reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);//注册片选回调函数
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
    reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);
#else
#if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
#error "Unknown _WIZCHIP_IO_MODE_"
#else
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
#endif
#endif
    reg_wizchip_spi_cbfunc(W5500_ReadData, W5500_WriteData);    //SPI读写字节回调函数
}

//初始化芯片参数
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
//初始化网络通讯参数,mac,ip等
void W5500_NetInfoConfig(void)
{
    uint8_t tmpstr[6];
    ctlnetwork(CN_SET_NETINFO, (void *)&NetInforStr.Client.NetInfor);			//设置默认网关信息
    ctlnetwork(CN_GET_NETINFO, (void *)&NetInforStr.Client.NetInfor);	//获取网关信息
    ctlwizchip(CW_GET_ID, (void *)tmpstr);									//获取PHY芯片ID
    //串口打印出参数
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
		switch(getSn_SR(0))																//获取socket0的状态	
		{																									
			case SOCK_CLOSED:                               //Socket处于关闭状态
					//建立套接字
					socket(0,Sn_MR_TCP,++NetInforStr.Server.Port,Sn_MR_ND); 
					printf("Close %d\r\n",NetInforStr.Server.Port);
				break;
			case SOCK_INIT:																	//Socket处于初始化完成(打开)状态
					//配置Sn_CR为CONNECT，并向TCP服务器发出连接请求
			printf("Connect\r\n");
					printf("%d\r\n",connect(0, NetInforStr.Server.NetInfor.ip,33097));
			vTaskDelay(1000);
				break;
			case SOCK_ESTABLISHED:													//Socket处于连接建立状态
				printf("Co ok\r\n");
					if(getSn_IR(0) & Sn_IR_CON) 
					{
						setSn_IR(0, Sn_IR_CON);										//Sn_IR的CON位置1，通知W5500连接已建立 
					}	
					
//					W5500Data.Len = getSn_RX_RSR(0);	//获取数据长度
//					if(W5500Data.Len > 2) 
//					{
//						recv(0,W5500Data.Data,W5500Data.Len);		  //W5500接收来自服务器的数据，并通过SPI发送给MCU
//							
//						memset(W5500Data.Data,0,W5500BufMAX);
//						W5500Data.Len = 0;
//					}
				break;
			case SOCK_CLOSE_WAIT:												 		//Socket处于等待关闭状态  
						close(0);           
				break;
		}
		
		vTaskDelay(100);
	}
}
*/