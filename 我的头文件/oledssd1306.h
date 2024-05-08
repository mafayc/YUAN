#ifndef __OLEDSSD1306_H
#define __OLEDSSD1306_H
/*
作者：马法源
日期：2024年04月20日
注解：这是一个写给ssd1306（6脚0.9寸OLED）屏幕的驱动程序（依赖于SPI.h的库文件驱动硬件SPI控制PLED）
服务对象：ESP32S3（Arduino环境）
本文件源：.h
*/

#define OLED_HZ   8000000//SCL
#define OLED_SCL  37//SCL
#define OLED_MO   38//SDA
#define OLED_MI   -1//SDA
#define OLED_RES  39//RES
#define OLED_DC   40//DC
#define OLED_CS   -1//用到cs时，须在cpp文件的“OLED_io()”中取消“OLED_CS”控制语句的注释
#define OLED_DATA  1
#define OLED_CMD  0
#define OLED_VCC  36
#define OLED_GND  35

//主要
void OLED_int1vcc(void);
void OLED_int2oled(void);
void OLED_io(unsigned char dat, unsigned char cmd);
void OLED_DisplayTurn(unsigned char i);
void OLED_off(void);
void OLED_on(void);
void OLED_xy(unsigned char x, unsigned char y);
void OLED_kill(void);



//次要
void OLED_Zh(int x, int y, int no);
void OLED_DrawBMP(unsigned char x, unsigned char y, unsigned char sizex, unsigned char sizey, unsigned char BMP[]);
void OLED_Char(int x, int y, int chr);
void OLED_String(int x, int y, const char* chr);
void OLED_Num3_1(int x, int  y, double num);
void OLED_Num2_0(int x, int  y, double num);

#endif  
/****示例
char string1[] = "MFY^";
double k=32;
OLED_Zh(16*1,0,1);//源
OLED_String(0,4,string1);
OLED_Num3_1(0,6,456.7);
    delay(1000);
OLED_DrawBMP(16*4,0,16,16,BMP1);
OLED_Num2_0(16*3,6,k+1);
    delay(1000);
    OLED_kill();
*/



