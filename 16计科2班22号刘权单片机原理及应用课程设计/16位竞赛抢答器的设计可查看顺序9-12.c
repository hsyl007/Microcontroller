

#include<reg51.h>
sbit wr=P3^0; 
sbit cs1=P3^1; //控制段锁存器
sbit cs2=P3^2; //用于选择控制位码

void delay_5ms()	   //延时5ms
{
	unsigned char i,j;
	for(i=0;i<=25;i++)
		for(j=0;j<=200;j++); 	
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
	  case 0x09:
	    value=9;
		P0=dig;
	    break;
	  case 0x0a:
	    value=10;
		P2=dig; 
	    break;

		/*
	  case 0x0c:
	    value=13;
	    break;
	  case 0x0d:
	    value=14;
	    break;
	  case 0x0e:
	    value=15;
	    break;
      case 0x0f:
	    value=16;
	    break;
		*/
	  default:
	    value=0;
	    break;
	}

}
 
void main()
{
  while(1)
  {
  /*unsigned char dig;
  P2=0x00;
  P0=0x00;  
  dig=P1&0xf0;
  P0=dig>>4;
  P2=dig;*/	
  wr=0;	 
  scan();


	
  }
}

