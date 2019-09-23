/*
MAX7219��һ�ּ��ɻ��Ĵ�������/�����������ʾ��������
������΢��������8λ���ֵ�7������LED��ʾ��
Ҳ������������ͼ��ʾ������64��������LED��
���ϰ���һ��Ƭ�ϵ�B��BCD����������·ɨ���·��������������
���һ���һ��8*8�ľ�̬RAM�����洢ÿһ�����ݡ�
*/
#include<reg51.h>

sbit DIN=P3^0;//��max7219�ӿڶ���
sbit LOAD=P3^1;
sbit CLK=P3^2;

sbit key0=P1^0;//8·����������
sbit key1=P1^1;
sbit key2=P1^2;
sbit key3=P1^3;
sbit key4=P1^4;
sbit key5=P1^5;
sbit key6=P1^6;
sbit key7=P1^7;

sbit key_clear=P2^0;//������ʱ�����á����
sbit begin=P2^1;//�����˿�ʼ����

sbit sounder=P3^7;//������

unsigned char second=30;//������ֵ
unsigned char counter=0;//counterÿ100��minite��1
unsigned char people=0;//������

unsigned char num_add[]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};//max7219��д��ַ������
unsigned char num_dat[]={0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89};

unsigned char keyscan()//����ɨ�躯��
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

void max7219_send(unsigned char add,unsigned char dat)//��max7219дָ���
{
  unsigned char	ADS,i,j; 

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

void scare_display(unsigned char x)//��������ʾ
{
  unsigned char i,j;
  i=x/10;
  j=x%10;
  max7219_send(num_add[3],num_dat[j]);
  max7219_send(num_add[2],num_dat[i]);
}

void holderscan()//����ʱ�����ã�0-60s
{
  time_display(second);
  scare_display(people);
  if(~key_clear)//����м����£��ı�����ʱ��
  {
    while(~key_clear);
	if(people)//���������û����գ�����������
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
  TH0=0xd8;//�趨10ms�ж�һ��
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
  TH0=0xd8;//����װ��
  TL0=0xef;
  TR0=1;
}

void main()
{
  while(1)
  {
  do{
    holderscan();
  }while(begin);//��ʼǰ��������,��δ���¿�ʼ��
  while(~begin);//����
  max7219_init(); //оƬ��ʼ��
  timer_init();	  //�жϳ�ʼ��
  TR0=1;		  //��ʼ�ж�
  do{
    time_display(second);
	scare_display(people);
	people=keyscan();
  }while((!people)&&(second));//����ֱ�������������ʱ�����
  TR0=0;
  }
}

