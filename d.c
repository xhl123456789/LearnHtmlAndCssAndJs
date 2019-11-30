	#include<reg52.h>
	#include<intrins.h>
	#include<stdlib.h>
	#define uchar unsigned char
	#define uint unsigned int
	#define LCD_data P0;//数据口

	sbit LCD_RS=P3^5;   //数据选择输入
	sbit LCD_RW=P3^6;   //液晶读写控制
	sbit LCD_EN=P3^4;   //液晶使能控制端
	sbit LCD_PSB=P3^7;  //串并方式控制
	sbit wela=P2^7;
	sbit dula=P2^6;
	uchar code dis1[]= {"I like sleep,but"};
	uchar code dis2[]= {"like candy best!"};
	uchar code dis3[]= {"  卖萌无罪！ "};
	uchar code dis4[]= {"2016-3-21By Echo"};

	void delay_1ms(uint x)
	{
	    uint i,j;
	    for(j=0; j<x; j++)
	        for(i=0; i<110; i++);
	}
	/*写命令到LCD*/
	/*PS=L,RW=L,D0~D7=指令码,E=高脉冲*/
	void write_cmd(uchar cmd)
	{
	    LCD_RS=0;
	    LCD_RW=0;
	    delay_1ms(5);
	    LCD_EN=0;
	    P0=cmd;
	    delay_1ms(5);
	      LCD_EN=1;
	    delay_1ms(5);
	    LCD_EN=0;
	}
	/*写显示数据到LCD*/
	/*RS=H，RW=L，E=高脉冲，D0~D7=数据 */
	void write_dat(uchar dat)
	{
	    LCD_RS=1;
	    LCD_RW=0;
	    LCD_EN=0;
	    P0=dat;
	    delay_1ms(5);
	    LCD_EN=1;
	    delay_1ms(5);
	    LCD_EN=0;
	}
	/*设定显示位置*/
	void lcd_pos(uchar X,uchar Y)
	{
	    uchar pos;
	    if(X==0)
	    {
	        X=0x80;
	    }
	    else if(X==1)
	    {
	        X=0x90;
	    }
	    else if(X==2)
	    {
	        X=0x88;
	    }
	    else if(X==3)
	    {
	        X=0x98;
	    }
	    pos=X+Y;
	    write_cmd(pos);
	}
	/*LCD初始化设置*/
	void lcd_init()
	{
	    LCD_PSB=1;     //并口方式
	    write_cmd(0x30);//基本指令操作
	    delay_1ms(5);
	    write_cmd(0x0C);//先是开，关光标
	    delay_1ms(5);
	    write_cmd(0x01);//清除LCD的显示内容
	    delay_1ms(5);
	}
	void main()
	{
	    uchar i;
	    wela=0;
	    dula=0;
	    delay_1ms(10);
	    lcd_init();
	    i=0;

	    lcd_pos(0,0);
	    while(dis1[i]!='\0')
	    {
	        write_dat(dis1[i]);
	        i++;
	    }

	    lcd_pos(1,0);
	    i=0;
	    while(dis2[i]!='\0')
	    {
	        write_dat(dis2[i]);
	        i++;
	    }

	    lcd_pos(2,0);
	    i=0;
	    while(dis3[i]!='\0')
	    {
	        write_dat(dis3[i]);
	        i++;
	    }

	    lcd_pos(3,0);
	    i=0;
	    while(dis4[i]!='\0')
	    {
	        write_dat(dis4[i]);
	        i++;
	    }
	    while(1);
	}
