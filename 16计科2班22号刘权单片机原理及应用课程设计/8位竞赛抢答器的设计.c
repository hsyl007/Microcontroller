/*
MAX7219是一种集成化的串行输入/输出共阴极显示驱动器，
它连接微处理器与8位数字的7段数字LED显示，
也可以连接条线图显示器或者64个独立的LED。
其上包括一个片上的B型BCD编码器、多路扫描回路，段字驱动器，
而且还有一个8*8的静态RAM用来存储每一个数据。
*/
#include<reg51.h>

sbit DIN=P3^0;//与max7219接口定义
sbit LOAD=P3^1;
sbit CLK=P3^2;

sbit key0=P1^0;//8路抢答器按键
sbit key1=P1^1;
sbit key2=P1^2;
sbit key3=P1^3;
sbit key4=P1^4;
sbit key5=P1^5;
sbit key6=P1^6;
sbit key7=P1^7;

sbit key_clear=P2^0;//主持人时间设置、清除
sbit begin=P2^1;//主持人开始按键

sbit sounder=P3^7;//蜂鸣器

unsigned char second=30;//秒表计数值
unsigned char counter=0;//counter每100，minite加1
unsigned char people=0;//抢答结果

unsigned char num_add[]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};//max7219读写地址、内容
unsigned char num_dat[]={0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89};

unsigned char keyscan()//键盘扫描函数
{
  unsigned char keyvalue,temp;
  
  keyvalue=0;
  P1=0xff;
  temp=P1;
  if(~(P1&temp))
  {
    switch(temp)
	{
	  case 0xfe:
	    keyvalue=1;
	    break;
	  case 0xfd:
	    keyvalue=2;
	    break;
	  case 0xfb:
	    keyvalue=3;
	    break;
	  case 0xf7:
	    keyvalue=4;
	    break;
	  case 0xef:
	    keyvalue=5;
	    break;
	  case 0xdf:
	    keyvalue=6;
	    break;
	  case 0xbf:
	    keyvalue=7;
	    break;
      case 0x7f:
	    keyvalue=8;
	    break;
	  default:
	    keyvalue=0;
	    break;
	}
  }

  return keyvalue;
}

void max7219_send(unsigned char add,unsigned char dat)//向max7219写指令函数
{
  unsigned char	ADS,i,j; 

  LOAD=0;  //拉低片选线，选中器件
  i=0;
  while(i<16)
  {
    if(i<8)
	{
	  ADS=add;
	}
	else
	{
	  ADS=dat;
	}
	for(j=8;j>=1;j--)
	{				  
	  DIN=ADS&0x80;	  //每次取高字节
	  ADS=ADS<<1;	  //左移一位
	  CLK=1;		  //在CLK的上升沿，发送地址
	  CLK=0;		  //清零时钟总线
	}
	i=i+8;
  }
  LOAD=1;  //发送结束，上升沿锁存数据，数据被锁入芯片内部16位移位寄存器
}

void max7219_init()//max7219初始化函数
{
  max7219_send(0x09,0xff);	//译码控制寄存器，选用全译码模式
  max7219_send(0x0a,0xff);	//亮度控制寄存器，设置初始亮度
  max7219_send(0x0b,0x07);	//扫描界限寄存器，8只led全用
  max7219_send(0x0c,0x01);	//关断模式寄存器，开启正常工作模式
  max7219_send(0x0f,0x00);	//测试控制寄存器，选择工作模式
}

void time_display(unsigned char x)//时间显示
{
  unsigned char i,j;
  i=x/10;
  j=x%10;
  max7219_send(num_add[1],num_dat[j]);
  max7219_send(num_add[0],num_dat[i]);
}

void scare_display(unsigned char x)//抢答结果显示
{
  unsigned char i,j;
  i=x/10;
  j=x%10;
  max7219_send(num_add[3],num_dat[j]);
  max7219_send(num_add[2],num_dat[i]);
}

void holderscan()//抢答时间设置，0-60s
{
  time_display(second);
  scare_display(people);
  if(~key_clear)//如果有键按下，改变抢答时间
  {
    while(~key_clear);
	if(people)//如果抢答结果没有清空，抢答器重置
	{
	  second=30;
	  people=0;
	}
	if(second<60)
	{
	  second++;
	}
	else
	{
	  second=0;
	}
  }
}

void timer_init()
{
  EA=1;
  ET0=1;
  TMOD=0x01;
  TH0=0xd8;//设定10ms中断一次
  TL0=0xef;
}

void timer0() interrupt 1
{
  if(counter<100)
  {
    counter++;
	if(counter==50)
	{
	  sounder=0;
	}
  }
  else
  {
    sounder=1;
    counter=0;
	second=second-1;
  }
  TH0=0xd8;//重新装载
  TL0=0xef;
  TR0=1;
}

void main()
{
  while(1)
  {
  do{
    holderscan();
  }while(begin);//开始前进行设置,若未按下开始键
  while(~begin);//防抖
  max7219_init(); //芯片初始化
  timer_init();	  //中断初始化
  TR0=1;		  //开始中断
  do{
    time_display(second);
	scare_display(people);
	people=keyscan();
  }while((!people)&&(second));//运行直到抢答结束或者时间结束
  TR0=0;
  }
}

