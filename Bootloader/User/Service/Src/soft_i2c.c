#include "soft_i2c.h"
#include "stdint.h"

uint32_t cpu_frq = 0;
uint32_t nop_delay_1us_times = 0;

#define IIC_WAIT_4US() iic_delay(4 * nop_delay_1us_times)
#define IIC_WAIT_2US() iic_delay(2 * nop_delay_1us_times)
#define IIC_WAIT_1US() iic_delay(1 * nop_delay_1us_times)

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

__weak uint32_t get_cpu_frq(void)
{
	return 240000000;
}

static void iic_delay(uint32_t times)
{
	while(times--) {
		__nop();
	}
}

__weak uint8_t IIC_SDA_Read_Pin(void)
{
	
}

void IIC_Start(void)
{
	//SDA_OUT();	//SDA Set Output Mode
	
	IIC_SDA_H;	//SDA Set High Level
	IIC_SCL_H;	//SCL Set High Level
	IIC_WAIT_4US();	//Analog bus idle 4us
	IIC_SDA_L;
	IIC_WAIT_4US();
	IIC_SCL_L;
	IIC_WAIT_4US();
}

void IIC_Stop(void)
{
	//SDA_OUT();
	IIC_SDA_L;
	IIC_WAIT_4US();
	IIC_SCL_H;
	IIC_WAIT_4US();
	IIC_SDA_H;
	IIC_WAIT_4US();
}

uint8_t IIC_Wait_ACK(void)
{
	uint8_t errTimes = 0;
	//SDA_IN();
	IIC_SCL_L;
	IIC_WAIT_4US();
	IIC_SDA_H;
	IIC_WAIT_4US();
	
	IIC_SCL_H;
	IIC_WAIT_4US();
	
	while(IIC_SDA_Read_Pin()) {
		errTimes++;
		if (errTimes > 250) {
			IIC_Stop();
			return 1;
		}
	}
	
	IIC_SCL_L;
	IIC_WAIT_4US();
	
	return 0;
}

void IIC_ACK(void)
{
	IIC_SCL_L;
	SDA_OUT();
	IIC_SDA_L;
	IIC_WAIT_2US();
	IIC_SCL_H;
	IIC_WAIT_2US();
	IIC_SCL_L;
}

void IIC_NACK(void)
{
	IIC_SCL_L;
	SDA_OUT();
	IIC_SDA_H;
	IIC_WAIT_2US();
	IIC_SCL_H;
	IIC_WAIT_2US();
	IIC_SCL_L;
}

uint8_t IIC_Send_Byte(uint8_t ch)
{
	//SDA_OUT();
	for (uint8_t i = 0 ; i < 8 ; i++) {
		IIC_SCL_L;
		IIC_WAIT_4US();
		if ((ch << i) & 0x80) {
			IIC_SDA_H;
		} else {
			IIC_SDA_L;
		}
		IIC_WAIT_4US();
		IIC_SCL_H;
		IIC_WAIT_4US();
	}
	
	IIC_SCL_L;
	IIC_WAIT_2US();
	return IIC_Wait_ACK();
}

void IIC_Test()
{
	SDA_OUT();
	SCL_OUT();
	for (int i = 0 ; i < 20 ; i++) {
		if (i % 2 == 0) {
			IIC_SCL_H;
			IIC_SDA_L;
		} else {
			IIC_SCL_L;
			IIC_SDA_H;
		}
		IIC_WAIT_2US();
	}
}

uint8_t IIC_Recv_Byte(uint8_t ack)
{
	uint8_t recv = 0;
	SDA_IN();
	for (int i = 0 ; i < 8 ; i++) {
		IIC_SCL_L;
		IIC_WAIT_2US();
		IIC_SCL_H;
		recv = recv << 1;
		if (IIC_SDA_Read_Pin()) {
			recv |= 1;
		}
		IIC_WAIT_1US();
	}
	
	if (ack) {
		IIC_ACK();
	} else {
		IIC_NACK();
	}
	
	return recv;
}

void IIC_Init(void)
{
	SDA_OUT();
	SCL_OUT();
	
	cpu_frq = get_cpu_frq();
	nop_delay_1us_times = cpu_frq / 1000000;
	
	IIC_SCL_H;
	IIC_SDA_H;
}