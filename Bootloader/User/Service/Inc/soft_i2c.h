#ifndef __SOFT_IIC_H
#define __SOFT_IIC_H

#define IIC_SCL_H SCL_Set_Pin(1)
#define IIC_SCL_L SCL_Set_Pin(0)
#define IIC_SDA_H SDA_Set_Pin(1)
#define IIC_SDA_L SDA_Set_Pin(0)

void IIC_Start(void);
void IIC_Stop(void);
void IIC_Init(void);

#endif