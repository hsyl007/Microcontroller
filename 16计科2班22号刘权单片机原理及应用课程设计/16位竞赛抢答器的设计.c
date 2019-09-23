

#include<reg51.h>

sbit DIN=P3^0;//��max7219�ӿڶ���
sbit LOAD=P3^1;
sbit CLK=P3^2;	  
sbit key_clear=P2^0;//������ʱ�����
sbit begin=P2^1;//�����˿�ʼ����
sbit time_add=P2^2;//������ʱ���
sbit time_sub=P2^3;//������ʱ���  
sbit sounder=P3^7;//������

unsigned char second=30;//������ֵ
unsigned char counter=0;//counterÿ100��minite��1
unsigned char people=0;//������	
unsigned char num_add[]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
//max7219��д��ַ
unsigned char num_dat[]={0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,
0x89}; //max7219��д����

void delay_5ms()	   //��ʱ5ms
{
	unsigned char i,j;
	for(i=0;i<=25;i++)
		for(j=0;j<=200;j++);
	
}

unsigned char keyscan()//���̶�̬ɨ�躯��
{
  unsigned char keyvalue=0;
  unsigned char key_temp0,key_temp1;
  P0=0x00;
  P1=0x0f;		 //����1
  key_temp0=P1;
  if(key_temp0!=0x0f)	 //��4λ��ȫΪ1�����м�����
  {
    delay_5ms(); //��������
	key_temp0=P1;
	if(key_temp0!=0x0f)//�ض�����ֵ
	{
	  P1=0xf0;		//����1
	  key_temp1=P1;
	  if(key_temp0==0x0e)  //��һ���а���
	  {
	    switch(key_temp1)
		{
		  case 0xe0: 
		  keyvalue=4;
		  break;	   //�������а���
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
		  break;	   //�޼�����
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
  P0=keyvalue&0x0f;	 //����ɨ�����4λ
  return keyvalue;
}

unsigned char scan(x)//ɨ�躯��
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
MAX7219��һ�ּ��ɻ��Ĵ�������/�����������ʾ��������
������΢��������8λ���ֵ�7������LED��ʾ��
Ҳ������������ͼ��ʾ������64��������LED��
���ϰ���һ��Ƭ�ϵ�B��BCD����������·ɨ���·��������������
���һ���һ��8*8�ľ�̬RAM�����洢ÿһ�����ݡ�
*/

void max7219_send(unsigned char add,unsigned char dat)
//��max7219дָ���
{
  unsigned char	ADS,i,j; //����16λ���ݴ�����λ���ܷ�ʽ
  //��Ƭ����16λ����������λ���͵�DIN��
  //��CLK��ÿ�������ؽ�һλ��������MAX7219�ڲ�����λ�Ĵ���
  //��16λ�������������LOAD�����ؽ�16λ����װ��MAX7219����Ӧλ��
  LOAD=0;  //����Ƭѡ�ߣ�ѡ������
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
	  DIN=ADS&0x80;	  //ÿ��ȡ���ֽ�
	  ADS=ADS<<1;	  //����һλ
	  CLK=1;		  //��CLK�������أ����͵�ַ
	  CLK=0;		  //����ʱ������
	}
	i=i+8;
  }
  LOAD=1;  //���ͽ������������������ݣ����ݱ�����оƬ�ڲ�16λ��λ�Ĵ���
}

void max7219_init()//max7219��ʼ������
{
  max7219_send(0x09,0xff);	//������ƼĴ�����ѡ��ȫ����ģʽ
  max7219_send(0x0a,0xff);	//���ȿ��ƼĴ��������ó�ʼ����
  max7219_send(0x0b,0x07);	//ɨ����޼Ĵ�����8ֻledȫ��
  max7219_send(0x0c,0x01);	//�ض�ģʽ�Ĵ�����������������ģʽ
  max7219_send(0x0f,0x00);	//���Կ��ƼĴ�����ѡ����ģʽ
}

void time_display(unsigned char x)//ʱ����ʾ
{
  unsigned char i,j;
  i=x/10;
  j=x%10;
  max7219_send(num_add[1],num_dat[j]);
  max7219_send(num_add[0],num_dat[i]);
}

void score_display(unsigned char x)//��������ʾ
{
  unsigned char i,j;
  i=x/10;
  j=x%10;
  max7219_send(num_add[3],num_dat[j]);
  max7219_send(num_add[2],num_dat[i]);
}

unsigned char sequence_display(unsigned char j)//����˳����ʾ
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
   max7219_send(num_add[5],num_dat[6]);	  //������ʾ
   max7219_send(num_add[4],num_dat[6]);
   }   	
   return j; 
}

void holderscan()//����ʱ�����ã�0-60s
{
  time_display(second);
  score_display(people);

  if(~key_clear)//�������ʱ�䣬����м����£��ı�����ʱ��
  {
    while(~key_clear);
	if(people)//���������û����գ�����������
	{
	  second=30;
	  people=0;
	}
	}
  if(~time_add)
  {
   while(~time_add);//��ʱ��	 
   if(second<60)
   {
	  second++;
  }
 }
   
   if(~time_sub)	//��ʱ��
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
  EA=1;		  //���ж��ܿ���
  ET0=1;		
  TMOD=0x01;   //��ʱ��0��ʽ1
  TH0=0xd8;
//�趨10ms�ж�һ�Σ�TH��TL���ṩ8λ������ֵ����TL��8λ����������TH��λ
//TH����������λTF
  TL0=0xef;
}



 
void main()
{
  unsigned char seq;
  seq=1;
  while(1)
  {
  do{
    holderscan();//��ʼ��������ʱ��
  }while(begin);//��ʼ��������ʼǰ���г�ʼ����,��δ���¿�ʼ��
  while(~begin);//����
  max7219_init(); //оƬ��ʼ��
  timer_init();	  //�жϳ�ʼ��
  TR0=1;		  //��ʼ�ж�
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
  }while((!people)&&(second));//����ֱ�������������ʱ�����
  TR0=0;
  }
}

void timer0() interrupt 1  //������/ʱ�ӡ�����
{
  if(counter<100)
  {
    counter++;
	if(counter==50)	//ÿ0.5����һ��
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

  TH0=0xd8;//����װ��
  TL0=0xef;
  TR0=1;
}