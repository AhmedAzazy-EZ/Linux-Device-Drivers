#ifndef _APP_I2C_H_
#define _APP_I2C_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

extern I2C_HandleTypeDef i2c1;
extern I2C_HandleTypeDef i2c2;


struct I2C_data
{
	unsigned int buffer_track;
	char buffer[100];
};

void I2C1_Master_init(void);
void I2C2_Slave_init(void);

#endif