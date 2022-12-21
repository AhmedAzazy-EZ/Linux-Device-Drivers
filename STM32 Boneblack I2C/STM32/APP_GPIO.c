#include "APP_GPIO.h"
#include <stdio.h>
void GPIO_Test(GPIO_TypeDef * GPIO , int pin_num , int num_of_blinks)
{
	volatile int GPIO_EN;
	switch((uint32_t)GPIO)
	{
		case (uint32_t)GPIOA:
			GPIO_EN = 0;
			break;
		
		case (uint32_t)GPIOB:
			GPIO_EN = 1;
			break;

		case (uint32_t)GPIOC:
			GPIO_EN = 2;
			break;

		case (uint32_t)GPIOD:
			GPIO_EN = 3;
			break;	

		case (uint32_t)GPIOE:
			GPIO_EN = 4;
			break;

		default : 
			//printf("Invalid Port .... Aborting\n");
			return;
	}
	
	RCC->AHB1ENR |= (1<<GPIO_EN);
	GPIO->MODER &=~ (3<<(pin_num*2));
	GPIO->MODER |= (1<<(pin_num*2));
	
	unsigned int i = 0;
	while(num_of_blinks--)
	{
		GPIO->ODR |= (1<<pin_num);
		for(i = 0 ; i < 1000000 ; i++);
		GPIO->ODR &=~ (1<<pin_num);
		for(i = 0 ; i < 1000000 ; i++);
	}
	
	GPIO->ODR &=~ (1<<pin_num);
}


void SystemClockInit(void)
{

											/***Set PLL Divider Factors***/
 RCC->PLLCFGR&=~(0x3F);
 RCC->PLLCFGR|=(2);          //PLL_M = 2
 RCC->PLLCFGR&=~(0x1ff<<6);
 RCC->PLLCFGR|=(80<<6);     //PLL_N=80
 RCC->PLLCFGR|=(3<<16);     //PLL_P 8
 RCC->PLLCFGR&=~(1<<22);     //Write zero To bit 22 which assign HSI as input to the PLL


	/*
	* Enable POWER interface Clock
	*/
	RCC->APB1ENR|=(1<<28);

	                      /***Set Clock Configuration Reg***/
	RCC->CFGR|=(4<<13);        //Divide AHB by 2 gives , APB2 = 40MHz since AHB is 180MHz
	RCC->CFGR|=(5<<10);        //Divide AHB by 4 then , APB1 = 20MHz
  RCC->CFGR&=~(0xF<<4);      //Ensure AHB Clock is not Divided = 80 MHz
	RCC->CR|=(1<<24);             //Enable main PLL
	while(!((RCC->CR)&(1<<25))) ;  //wait until PLL is Ready

	/*I Don't Know what The FUCK this line is doing , I have just copied it
	 *from somwhere on internet and paste it here  xD*/
	FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;

	RCC->CFGR|=(2);            //Switch Clock To PLL
	  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL)
        {

        }
				
	SystemCoreClockUpdate();
	}
