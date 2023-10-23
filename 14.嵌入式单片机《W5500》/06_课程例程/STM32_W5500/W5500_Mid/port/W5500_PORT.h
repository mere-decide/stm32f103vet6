#ifndef _W5500_PORT_H_
#define _W5500_PORT_H_

#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "delay.h"
#include "W5500.h"
#include "socket.h"
#include "wizchip_conf.h"

/****************************
			W-RST			PA0
			W-INT			PA1
			W-CS			PA4
			W-SCLK		PA5
			W-MISO		PA6
			W-MOSI		PA7
****************************/

#define W5500_RESET(x) (GPIO_WriteBit(GPIOD,GPIO_Pin_9,(BitAction)x))
#define W5500_SPICS(x) (GPIO_WriteBit(GPIOB,GPIO_Pin_12,(BitAction)x))


#define W5500BufMAX 1152
typedef struct{
	uint8_t Data[W5500BufMAX];
	uint16_t Len;
}_W5500GetData;
extern _W5500GetData W5500Data;

typedef struct{
	wiz_NetInfo NetInfor;	//基本IP、MAC...
	uint16_t Port;				//端口号
}_NetInfor;
typedef struct{
	_NetInfor Server;	//服务器
	_NetInfor Client;
}_NetStruct;
extern _NetStruct NetInforStr;


void W5500_Config(void);
void W5500_Reset(void);
void W5500_PortConfig(void);
void W5500_RegFunction(void);
void W5500_ChipConfig(void);
void W5500_NetInfoConfig(void);
void DHCP_SET(void);

#endif
