#ifndef __DELAY_H_
#define __DELAY_H_

#include "stm32f10x.h"

#define Delay_Nopnms(x) Delay_Nopnus(x*1000)

void Delay_Nopnus(uint32_t timer);

#endif
