#include "bsp_sys_pub.h"

void delay_us(u16 nCount)
{
    nCount *=3;
    while(--nCount);
}

void delay_ms(u16 nCount)
{
    while(nCount--)
    {
        delay_us(1000);
    }
}

u16 Get_decimal(double dt,u8 deci)
{
    long x1=0;
    u16 x2=0,x3=0;

    if(deci>4) deci=4;
    if(deci<1) deci=1;

    x3=(u16)pow(10, deci);
    x1=(long)(dt*x3);
    x2=(u16)(x1%x3);

    return x2;
}
