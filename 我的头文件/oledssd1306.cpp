/*
作者：马法源
日期：2024年04月20日
注解：这是一个写给ssd1306（6脚0.9寸OLED）屏幕的驱动程序（依赖于SPI.h的库文件驱动硬件SPI控制PLED）
服务对象：ESP32S3（Arduino环境）
本文件源：.CPP
*/
#include<SPI.h>
#include <Arduino.h>
#include "oledssd1306.h"//控制引脚
#include "oledfont.h"  	 
	
auto spi = SPIClass(HSPI);
void OLED_int1vcc(void)
{
pinMode(OLED_VCC, OUTPUT);     //设置引脚模式
pinMode(OLED_GND, OUTPUT);     //设置引脚模式
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
	spi.write(0xae);	//关显示

	spi.write(0xd5);	//设置显示时钟分频率、振荡器频率
	spi.write(0x80);    //A[3:0]:分频频率1到16，A[7:4]频率

	spi.write(0xa8);	//duty设置
	spi.write(0x3f);	//duty=1/64

	spi.write(0xd3);	//显示偏移
	spi.write(0x00);    //不偏移

	spi.write(0x40);	//起始行 40~7F

	spi.write(0x8d);	//升压允许
	spi.write(0x14);

	spi.write(0x20);	//设置内存地址模式
	spi.write(0x02);    //00水平地址模式，01垂直模式，02页地址模式，


	spi.write(0xc8); //行扫描顺序：从上到下c8	//上下颠倒c0
	spi.write(0xa1); //列扫描顺序：从左到右a1	//左右翻转a0

	spi.write(0xda); //sequential configuration
	spi.write(0x12);

	spi.write(0x81); //微调对比度,本指令的0x81不要改动，改下面的值
	spi.write(0xcf); //微调对比度的值，可设置范围0x00～0xff

	spi.write(0xd9); //Set Pre-Charge Period
	spi.write(0xf1);

	spi.write(0xdb); //Set VCOMH Deselect Level
	spi.write(0x40);

	spi.write(0xaf); //开显示

	spi.endTransaction();//不要动SPI输出保护
	
}
//坐标设置
void OLED_xy(unsigned char x, unsigned char y)
{
		
	OLED_io(0xb0 + y, OLED_CMD);
	OLED_io(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	OLED_io((x & 0x0f), OLED_CMD);
	
}

void OLED_io(unsigned char dat, unsigned char cmd)//SPI放这里
{
	digitalWrite(OLED_DC, cmd);

	//digitalWrite(OLED_CS, 0);//OLED_CS_Clr();
	spi.write(dat);
	//digitalWrite(OLED_CS, 1); //OLED_CS_Set();
	digitalWrite(OLED_DC, 1);
}


//屏幕旋转180度...貌似其他参数可以旋转90度
void OLED_DisplayTurn(unsigned char i)
{
	if (i == 0)
	{
		OLED_io(0xC8, OLED_CMD);//正常显示
		OLED_io(0xA1, OLED_CMD);
	}
	if (i == 1)
	{
		OLED_io(0xC0, OLED_CMD);//反转显示
		OLED_io(0xA0, OLED_CMD);
	}
}

//开启OLED显示    
void OLED_on(void)
{
	OLED_io(0X8D, OLED_CMD);  //SET DCDC命令
	OLED_io(0X14, OLED_CMD);  //DCDC ON
	OLED_io(0XAF, OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_off(void)
{
	OLED_io(0X8D, OLED_CMD);  //SET DCDC命令
	OLED_io(0X10, OLED_CMD);  //DCDC OFF
	OLED_io(0XAE, OLED_CMD);  //DISPLAY OFF
}



//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_kill(void)
{
	unsigned char i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_io(0xb0 + i, OLED_CMD);    //设置页地址（0~7）
		OLED_io(0x00, OLED_CMD);        //设置显示位置—列低地址
		OLED_io(0x10, OLED_CMD);        //设置显示位置—列高地址   
		for (n = 0; n < 128; n++)OLED_io(0, OLED_DATA);
	} //更新显示
}


//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63				 
//sizey:选择字体 6x8  8x16
void OLED_Char(int x, int y, int chr)
{
	OLED_xy(x, y);
	for(int i = 0;i<6;i++)	{OLED_io(asc2_0806[chr-32][i], OLED_DATA);}
}

//显示一个字符号串
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



//显示汉字
void OLED_Zh(int x, int y, int no)
{
    int i;
	for (i = 0; i < 32; i++)
	{
		if (i % 16 == 0) OLED_xy(x, y++);
		OLED_io(ZH[no][i], OLED_DATA);//16x16字号
	}	
}

//显示图片
//x,y显示坐标
//sizex,sizey,图片长宽
//BMP：要显示的图片
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
//显示数字
void OLED_Num3_1(int x, int  y, double num)
{
	int a, b, c, d;
	c = (int)num % 10;//0的ascll为48
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