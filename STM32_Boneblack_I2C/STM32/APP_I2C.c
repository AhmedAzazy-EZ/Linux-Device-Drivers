#include "APP_I2C.h"


I2C_HandleTypeDef i2c1;
I2C_HandleTypeDef i2c2;

/**
  * @brief Initialize I2C1 to operate in Master Mode
	* PB7 --->SDA 
	* PB8 --->SCL
  * @note 
  * @retval 
  */
void I2C1_Master_init(void)
{
	GPIO_InitTypeDef GPIO;
	
	__GPIOB_CLK_ENABLE();
	__I2C1_CLK_ENABLE();
	
	GPIO.Pin = GPIO_PIN_7;
	GPIO.Mode = GPIO_MODE_AF_OD;
	GPIO.Pull = GPIO_PULLUP;
	GPIO.Alternate = GPIO_AF4_I2C1;
	GPIO.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOB , &GPIO);
	
	GPIO.Pin = GPIO_PIN_8;
	HAL_GPIO_Init(GPIOB , &GPIO);	
	
	i2c1.Instance = I2C1;
	i2c1.Init.ClockSpeed = 100000;
	i2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	i2c1.Init.OwnAddress1 = 1;
	i2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	i2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c1.Init.OwnAddress2 = 68;
	i2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if(HAL_I2C_Init(&i2c1) != HAL_OK)
	{
		while(1);
	}
}


void I2C2_Slave_init(void)
{
	GPIO_InitTypeDef GPIO;
	
	__GPIOB_CLK_ENABLE();
	__I2C2_CLK_ENABLE();
	
	GPIO.Pin = GPIO_PIN_3;
	GPIO.Mode = GPIO_MODE_AF_OD;
	GPIO.Pull = GPIO_PULLUP;
	GPIO.Alternate = GPIO_AF4_I2C1;
	GPIO.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOB , &GPIO);
	
	GPIO.Pin = GPIO_PIN_10;
	HAL_GPIO_Init(GPIOB , &GPIO);	
	
	i2c2.Instance = I2C2;
	i2c2.Init.ClockSpeed = 100000;
	i2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
	i2c2.Init.OwnAddress1 = 2;
	i2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	i2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c2.Init.OwnAddress2 = 69;
	i2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if(HAL_I2C_Init(&i2c2) != HAL_OK)
	{
		while(1);
	}
}	

