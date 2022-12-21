#ifndef _APP_GPIO_H_
#define _APP_GPIO_H_

#include <stm32f4xx.h>

void GPIO_Test(GPIO_TypeDef * GPIO , int pin_num , int num_of_blinks);
void SystemClockInit(void);
#endif