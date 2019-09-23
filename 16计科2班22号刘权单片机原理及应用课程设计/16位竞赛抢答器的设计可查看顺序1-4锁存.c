 
#include<reg51.h>
sbit wr=P3^0; 
sbit cs1=P3^1; //控制段锁存器
sbit cs2=P3^2; //用于选择控制位码

void delay_5ms()	   //延时5ms
{
	unsigned char i,j;
	for(i=0;i<=25;i++)
		for(j=0;j<=2000;j++); 	
}

void scan()
{
unsigned char value,temp,dig;
  P2=0xff;
  P0=0xff;
  value=0;
  dig=P1&0xf0;	
  temp=P1&0x0f;  
    switch(temp)
	{
	  case 0x01:
	  cs1=0; 
	  value=1;   	   
	   P0=dig>>4;	   
	   cs1=1;	
	    break;
	  case 0x02:
	  cs2=0; 
	  value=2;   
	   P0=dig; 
	   cs2=1;
	    break;
	  case 0x03:
	    value=3;
		P2=dig>>4;
	    break;
	  case 0x04:
	    value=4;
		P2=dig;
	    break;
	  default:
	    value=0;
	    break;
	}
	
}
 
void main()
{
  while(1)
  {		 
  wr=1;	 
  scan();
  delay_5ms();
  }
}

