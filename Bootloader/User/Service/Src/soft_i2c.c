#include "soft_i2c.h"
#include "stdint.h"

__weak void SCL_Set_Pin(uint8_t pinset)
{
	
}

__weak void SDA_Set_Pin(uint8_t pinset)
{
	
}

__weak void SDA_OUT()
{

}

__weak void SDA_IN()
{

}

__weak void SCL_OUT()
{

}

static void iic_delay()
{
	
}

__weak uint8_t IIC_SDA_Read_Pin(void)
{
	
}

void IIC_Start(void)
{
	SDA_OUT();
	
	IIC_SDA_H;
	IIC_SCL_H;
	
	
}

void IIC_Stop(void)
{
	
}

void IIC_Init(void)
{
	SDA_OUT();
	SCL_OUT();
	
	IIC_SCL_H;
	IIC_SDA_H;
}