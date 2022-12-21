#include <stm32f4xx.h>
#include "stm32f4xx_hal.h"
#include "string.h"

#include "APP_GPIO.h"
#include "APP_I2C.h"

HAL_StatusTypeDef ret;

struct I2C_data I2C1_Master;
struct I2C_data I2C2_Slave;

int main(void)
{
	SystemClockInit();
	HAL_Init();
	
//	GPIO_Test(GPIOA , 5, 20);
//	HAL_Delay(5000);
//	GPIO_Test(GPIOA , 5, 20);
	
	I2C1_Master_init();
	I2C2_Slave_init();
	
	NVIC_EnableIRQ(I2C1_EV_IRQn);
	NVIC_EnableIRQ(I2C2_EV_IRQn);
	strcpy(I2C1_Master.buffer , "Hello Yooo\n");
	HAL_Delay(1000);
	HAL_I2C_Slave_Receive_IT(&i2c2, (uint8_t *)&I2C2_Slave.buffer[0], 1);
	ret = HAL_I2C_Master_Transmit_IT(&i2c1, 2, (uint8_t *)&I2C1_Master.buffer[0], 1);
	
	while(1)
	{
	
	}
	
	return 0;
}

void SysTick_Handler(void)
{
	uwTick+=1;
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef * hi2c)
{
	struct I2C_data * i2c_data;
	if(hi2c == &i2c1)
	{
		i2c_data = &I2C1_Master;
	}
	
	else if(hi2c == &i2c2)
	{
		i2c_data = &I2C2_Slave;
	}
	
	else 
	{
		return ;
	}
	
	if(++i2c_data->buffer_track == strlen(i2c_data->buffer))
	{
		//Clear the buffer
		for(int i = 0 ; i < i2c_data->buffer_track ; i++)
			{
				i2c_data->buffer[i] = 0x00;
			}
		i2c_data->buffer_track = 0;
	}
	
	else 
	{
		HAL_I2C_Master_Transmit_IT(hi2c, 2, (uint8_t *)&i2c_data->buffer[i2c_data->buffer_track], 1);
	}
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	HAL_I2C_Slave_Receive_IT(hi2c, (uint8_t *)&I2C2_Slave.buffer[++I2C2_Slave.buffer_track], 1);
}

void I2C1_EV_IRQHandler(void)
{
	HAL_I2C_EV_IRQHandler(&i2c1);
}

void I2C2_EV_IRQHandler(void)
{
	HAL_I2C_EV_IRQHandler(&i2c2);
}