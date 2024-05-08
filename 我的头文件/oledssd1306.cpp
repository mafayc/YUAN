/*
���ߣ���Դ
���ڣ�2024��04��20��
ע�⣺����һ��д��ssd1306��6��0.9��OLED����Ļ����������������SPI.h�Ŀ��ļ�����Ӳ��SPI����PLED��
�������ESP32S3��Arduino������
���ļ�Դ��.CPP
*/
#include<SPI.h>
#include <Arduino.h>
#include "oledssd1306.h"//��������
#include "oledfont.h"  	 
	
auto spi = SPIClass(HSPI);
void OLED_int1vcc(void)
{
pinMode(OLED_VCC, OUTPUT);     //��������ģʽ
pinMode(OLED_GND, OUTPUT);     //��������ģʽ
pinMode(OLED_SCL, OUTPUT);
pinMode(OLED_MO, OUTPUT);
pinMode(OLED_RES, OUTPUT);
pinMode(OLED_DC, OUTPUT);
digitalWrite(OLED_GND, 0);

digitalWrite(OLED_VCC, 1);
digitalWrite(OLED_RES, 0);
delay(1);
digitalWrite(OLED_RES, 1);
digitalWrite(OLED_DC, OLED_CMD);
Serial.println("spi.begin");
spi.begin(OLED_SCL, OLED_MI, OLED_MO, OLED_CS);//spi.end();
}

void OLED_int2oled(void)
{
	spi.beginTransaction(SPISettings(OLED_HZ, MSBFIRST, SPI_MODE0));
	spi.write(0xae);	//����ʾ

	spi.write(0xd5);	//������ʾʱ�ӷ�Ƶ�ʡ�����Ƶ��
	spi.write(0x80);    //A[3:0]:��ƵƵ��1��16��A[7:4]Ƶ��

	spi.write(0xa8);	//duty����
	spi.write(0x3f);	//duty=1/64

	spi.write(0xd3);	//��ʾƫ��
	spi.write(0x00);    //��ƫ��

	spi.write(0x40);	//��ʼ�� 40~7F

	spi.write(0x8d);	//��ѹ����
	spi.write(0x14);

	spi.write(0x20);	//�����ڴ��ַģʽ
	spi.write(0x02);    //00ˮƽ��ַģʽ��01��ֱģʽ��02ҳ��ַģʽ��


	spi.write(0xc8); //��ɨ��˳�򣺴��ϵ���c8	//���µߵ�c0
	spi.write(0xa1); //��ɨ��˳�򣺴�����a1	//���ҷ�תa0

	spi.write(0xda); //sequential configuration
	spi.write(0x12);

	spi.write(0x81); //΢���Աȶ�,��ָ���0x81��Ҫ�Ķ����������ֵ
	spi.write(0xcf); //΢���Աȶȵ�ֵ�������÷�Χ0x00��0xff

	spi.write(0xd9); //Set Pre-Charge Period
	spi.write(0xf1);

	spi.write(0xdb); //Set VCOMH Deselect Level
	spi.write(0x40);

	spi.write(0xaf); //����ʾ

	spi.endTransaction();//��Ҫ��SPI�������
	
}
//��������
void OLED_xy(unsigned char x, unsigned char y)
{
		
	OLED_io(0xb0 + y, OLED_CMD);
	OLED_io(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	OLED_io((x & 0x0f), OLED_CMD);
	
}

void OLED_io(unsigned char dat, unsigned char cmd)//SPI������
{
	digitalWrite(OLED_DC, cmd);

	//digitalWrite(OLED_CS, 0);//OLED_CS_Clr();
	spi.write(dat);
	//digitalWrite(OLED_CS, 1); //OLED_CS_Set();
	digitalWrite(OLED_DC, 1);
}


//��Ļ��ת180��...ò����������������ת90��
void OLED_DisplayTurn(unsigned char i)
{
	if (i == 0)
	{
		OLED_io(0xC8, OLED_CMD);//������ʾ
		OLED_io(0xA1, OLED_CMD);
	}
	if (i == 1)
	{
		OLED_io(0xC0, OLED_CMD);//��ת��ʾ
		OLED_io(0xA0, OLED_CMD);
	}
}

//����OLED��ʾ    
void OLED_on(void)
{
	OLED_io(0X8D, OLED_CMD);  //SET DCDC����
	OLED_io(0X14, OLED_CMD);  //DCDC ON
	OLED_io(0XAF, OLED_CMD);  //DISPLAY ON
}
//�ر�OLED��ʾ     
void OLED_off(void)
{
	OLED_io(0X8D, OLED_CMD);  //SET DCDC����
	OLED_io(0X10, OLED_CMD);  //DCDC OFF
	OLED_io(0XAE, OLED_CMD);  //DISPLAY OFF
}



//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_kill(void)
{
	unsigned char i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_io(0xb0 + i, OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_io(0x00, OLED_CMD);        //������ʾλ�á��е͵�ַ
		OLED_io(0x10, OLED_CMD);        //������ʾλ�á��иߵ�ַ   
		for (n = 0; n < 128; n++)OLED_io(0, OLED_DATA);
	} //������ʾ
}


//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63				 
//sizey:ѡ������ 6x8  8x16
void OLED_Char(int x, int y, int chr)
{
	OLED_xy(x, y);
	for(int i = 0;i<6;i++)	{OLED_io(asc2_0806[chr-32][i], OLED_DATA);}
}

//��ʾһ���ַ��Ŵ�
void OLED_String(int x, int y, const char* chr)
{
	int j = 0;

	while (chr[j] != '\0')
	{	

		if (j == 22)    x=0;
		if (j == 22)    y++;
		if (j == 22*2)  y++;
		if (j == 22*3)  y++;
		if (j == 22*4)  y++;
		if (j ==22* 5)  y++;
		OLED_Char(x, y, chr[j++]);
		x += 6;
	}
}



//��ʾ����
void OLED_Zh(int x, int y, int no)
{
    int i;
	for (i = 0; i < 32; i++)
	{
		if (i % 16 == 0) OLED_xy(x, y++);
		OLED_io(ZH[no][i], OLED_DATA);//16x16�ֺ�
	}	
}

//��ʾͼƬ
//x,y��ʾ����
//sizex,sizey,ͼƬ����
//BMP��Ҫ��ʾ��ͼƬ
void OLED_DrawBMP(unsigned char x, unsigned char y, unsigned char sizex, unsigned char sizey, unsigned char BMP[])
{
	unsigned int j = 0;
	unsigned char i, m;
	sizey = sizey / 8 + ((sizey % 8) ? 1 : 0);
	for (i = 0; i < sizey; i++)
	{
		OLED_xy(x, i + y);
		for (m = 0; m < sizex; m++)
		{
			OLED_io(BMP[j++], OLED_DATA);
		}
	}
}
//��ʾ����
void OLED_Num3_1(int x, int  y, double num)
{
	int a, b, c, d;
	c = (int)num % 10;//0��ascllΪ48
	b = (int)num / 10 % 10;
	a = (int)num / 100 % 10;
	d = (int)(num * 10) % 10;

	OLED_Char(x, y, a + 48);
	OLED_Char(x += 6, y, b + 48);
	OLED_Char(x += 6, y, c + 48);
	OLED_Char(x += 6, y, 46);
	OLED_Char(x += 4, y, d + 48);
}
void OLED_Num2_0(int x, int  y, double num)
{
	int a, b;
	a = (int)num % 10;
	b = (int)num / 10 % 10;
	
	OLED_Char(x, y, b + 48);
	OLED_Char(x += 6, y,a + 48);
}