

#include<reg51.h>

sbit DIN=P3^0;//与max7219接口定义
sbit LOAD=P3^1;
sbit CLK=P3^2;	  
sbit key_clear=P2^0;//主持人时间清除
sbit begin=P2^1;//主持人开始按键
sbit time_add=P2^2;//主持人时间加
sbit time_sub=P2^3;//主持人时间减  
sbit sounder=P3^7;//蜂鸣器

unsigned char second=30;//秒表计数值
unsigned char counter=0;//counter每100，minite加1
unsigned char people=0;//抢答结果	
unsigned char num_add[]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
//max7219读写地址
unsigned char num_dat[]={0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,
0x89}; //max7219读写内容

void delay_5ms()	   //延时5ms
{
	unsigned char i,j;
	for(i=0;i<=25;i++)
		for(j=0;j<=200;j++);
	
}

unsigned char keyscan()//键盘动态扫描函数
{
  unsigned char keyvalue=0;
  unsigned char key_temp0,key_temp1;
  P0=0x00;
  P1=0x0f;		 //行置1
  key_temp0=P1;
  if(key_temp0!=0x0f)	 //低4位不全为1，则有键按下
  {
    delay_5ms(); //消除抖动
	key_temp0=P1;
	if(key_temp0!=0x0f)//重读输入值
	{
	  P1=0xf0;		//列置1
	  key_temp1=P1;
	  if(key_temp0==0x0e)  //第一行有按下
	  {
	    switch(key_temp1)
		{
		  case 0xe0: 
		  keyvalue=4;
		  break;	   //第四列有按下
		  case 0xd0: 
		  keyvalue=3;
		  break;
		  case 0xb0: 
		  keyvalue=2;
		  break;
		  case 0x70: 
		  keyvalue=1;
		  break;
		  default: 
		  keyvalue=0;
		  break;	   //无键按下
		}
	  }
	  else if(key_temp0==0x0d)
	  {
	   	switch(key_temp1)
		{
		  case 0xe0: 
		  keyvalue=8;
		  break;
		  case 0xd0: 
		  keyvalue=7;
		  break;
		  case 0xb0: 
		  keyvalue=6;
		  break;
		  case 0x70: 
		  keyvalue=5;
		  break;
		  default: 
		  keyvalue=0;
		  break;
		}
   
	  }
	  else if(key_temp0==0x0b)
	  {
	  	switch(key_temp1)
		{
		  case 0xe0: 
		  keyvalue=12;
		  break;
		  case 0xd0: 
		  keyvalue=11;
		  break;
		  case 0xb0: 
		  keyvalue=10;
		  break;
		  case 0x70: 
		  keyvalue=9;
		  break;
		  default: 
		  keyvalue=0;
		  break;
		}

	  }
	  else if(key_temp0==0x07)
	  {
	  	switch(key_temp1)
		{
		  case 0xe0: 
		  keyvalue=16;
		  break;
		  case 0xd0: 
		  keyvalue=15;
		  break;
		  case 0xb0: 
		  keyvalue=14;
		  break;
		  case 0x70: 
		  keyvalue=13;
		  break;
		  default: 
		  keyvalue=0;
		  break;
		}
	  }
	}
  }
  P0=keyvalue&0x0f;	 //保留扫描码低4位
  return keyvalue;
}

unsigned char scan(x)//扫描函数
{
	switch(x)
	{
		case 1:
		P0=0x00;
		break;
		case 2:
		P0=0x01;
		break;
		case 3:
		P0=0x02;
		break;
		case 4:
		P0=0x03;
		break;
		case 5:
		P0=0x04;
		break;
		case 6:
		P0=0x05;
		break;
		case 7:
		P0=0x06;
		break;
		case 8:
		P0=0x07;
		break;
		case 9:
		P0=0x08;
		break;
		case 10:
		P0=0x09;
		break;
		case 11:
		P0=0x0a;
		break;
		case 12:
		P0=0x0b;
		break;
		case 13:
		P0=0x0c;
		break;
		case 14:
		P0=0x0d;
		break;
		case 15:
		P0=0x0e;
		break;
		case 16:
		P0=0x0f;
		break;
		  default:
		  P0=0x00;
		  break;
	}
	return x;
}

/*
MAX7219是一种集成化的串行输入/输出共阴极显示驱动器，
它连接微处理器与8位数字的7段数字LED显示，
也可以连接条线图显示器或者64个独立的LED。
其上包括一个片上的B型BCD编码器、多路扫描回路，段字驱动器，
而且还有一个8*8的静态RAM用来存储每一个数据。
*/

void max7219_send(unsigned char add,unsigned char dat)
//向max7219写指令函数
{
  unsigned char	ADS,i,j; //采用16位数据串行移位接受方式
  //单片机将16位二进制数逐位发送到DIN端
  //在CLK的每个上升沿将一位数据移入MAX7219内部的移位寄存器
  //当16位数据移入完后在LOAD上升沿将16位数据装入MAX7219的相应位置
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

void score_display(unsigned char x)//抢答结果显示
{
  unsigned char i,j;
  i=x/10;
  j=x%10;
  max7219_send(num_add[3],num_dat[j]);
  max7219_send(num_add[2],num_dat[i]);
}

unsigned char sequence_display(unsigned char j)//抢答顺序显示
{
 
  if(j<10)
  { 
   max7219_send(num_add[5],num_dat[j]);
   max7219_send(num_add[4],num_dat[0]);
   }
   else if(j>10&&j<20)
  { 
   max7219_send(num_add[5],num_dat[j]);
   max7219_send(num_add[4],num_dat[1]);
   }
   else
   { 
   max7219_send(num_add[5],num_dat[6]);	  //出错显示
   max7219_send(num_add[4],num_dat[6]);
   }   	
   return j; 
}

void holderscan()//抢答时间设置，0-60s
{
  time_display(second);
  score_display(people);

  if(~key_clear)//设置清除时间，如果有键按下，改变抢答时间
  {
    while(~key_clear);
	if(people)//如果抢答结果没有清空，抢答器重置
	{
	  second=30;
	  people=0;
	}
	}
  if(~time_add)
  {
   while(~time_add);//加时间	 
   if(second<60)
   {
	  second++;
  }
 }
   
   if(~time_sub)	//减时间
  {
	while(~time_sub);
	if(second>0)
	{
	  second--;
	}
	else
	second=30;
	}
}

void timer_init()
{
  EA=1;		  //开中断总开关
  ET0=1;		
  TMOD=0x01;   //定时器0方式1
  TH0=0xd8;
//设定10ms中断一次，TH，TL各提供8位计数初值，当TL低8位计满回零向TH进位
//TH计满回零置位TF
  TL0=0xef;
}



 
void main()
{
  unsigned char seq;
  seq=1;
  while(1)
  {
  do{
    holderscan();//初始化，设置时间
  }while(begin);//开始按键，开始前进行初始设置,若未按下开始键
  while(~begin);//防抖
  max7219_init(); //芯片初始化
  timer_init();	  //中断初始化
  TR0=1;		  //开始中断
  do{
    time_display(second);
	score_display(people); 	
	scan();
	people=keyscan();
	if(people!=0)
	{
	sequence_display(seq);
	seq=sequence_display();
	seq++;
	}
  }while((!people)&&(second));//运行直到抢答结束或者时间结束
  TR0=0;
  }
}

void timer0() interrupt 1  //计数器/时钟、声音
{
  if(counter<100)
  {
    counter++;
	if(counter==50)	//每0.5秒响一次
	{
	  sounder=0;
	}
  }
  else
  {
    sounder=1;
    counter=0;
	second=second-1;   
	while(second==0)
	{ 
  		sounder=0;
  	}
  }	

  TH0=0xd8;//重新装载
  TL0=0xef;
  TR0=1;
}