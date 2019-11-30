#include "reg52.h"
#include "oled.h"
#include "HX711.h"
#include "EEPROM.H"
#include <reg52.h>

sbit Buzzer = P2^4;
float k1,k2;
int abs;

//按键 IO设置
sbit KEY1 = P1^4;
sbit KEY2 = P1^5;
sbit KEY3 = P1^6;
sbit KEY4 = P1^7;

void Delay_ms(unsigned int n);
unsigned char Scan_Key();
void Buzzer_Di();
void Get_Maopi();
void Get_Weight();


unsigned char KEY_NUM;

//定义变量
unsigned char KEY_NUM = 0;   //用来存放按键按下的键值
unsigned long HX711_Buffer = 0;  //用来存放HX711读取出来的数据
unsigned long Weight_Maopi = 0; //用来存放毛皮数据
long Weight_Shiwu = 0;          //用来存放实物重量
long Max_Value = 0;             //用来存放设置最大值
char maxValueTable[4] = {1,0,0,0};
unsigned char state = 0;    //用来存放设置状态
unsigned char Blink_Speed = 0;

float tsum=0.0;  //存放每一次的时间
unsigned int icount=0;

#define Blink_Speed_Max 6          //该值可以改变设置指针闪烁频率

//校准参数
//因为不同的传感器特性曲线不是很一致，因此，每一个传感器需要矫正这里这个参数才能使测量值很准确。
//当发现测试出来的重量偏大时，增加该数值。
//如果测试出来的重量偏小时，减小改数值。
//该值可以为小数
#define GapValue 430

//传感器最大测量值，单位是g
#define AlarmValue 5000			


int  time=0;
float     s=0;
	
sbit RX=P3^6;               //Echo
sbit TX=P3^7;               //Trig



void  StartModule() 		         //T1中断用来扫描数码管和计800MS启动模块
  {
  unsigned char a,b;
	  TX=1;			                 //800MS  启动一次模块
	  for(b=3;b>0;b--)
        for(a=10;a>0;a--);
	  TX=0;
  }

/*void Conut(void)
   {
	
	 StartModule();
	 while(!RX);		//当RX为零时等待
	 TR0=1;			    //开启计数
	 while(RX);			//当RX为1计数并等待
	 TR0=0;				//关闭计数
	 time=TH0*256+TL0;
	 TH0=0;
	 TL0=0;
	 S=(time*1.87)/10;     //算出来是mm
   }*/

void Conut(void)
   {
	
	 StartModule();
	 while(!RX);		//当RX为零时等待
	 TR0=1;			    //开启计数
	 while(RX);			//当RX为1计数并等待
	 TR0=0;				//关闭计数
	 time=TH0*256+TL0;
	 tsum += time;
	 icount++;
	 TH0=0;
	 TL0=0;
	 if(icount==10)
	{
		icount++;
		s=(time*1.87)/100;     //算出来是mm
	}
	
   }
		
	void xianshi()
{
  	unsigned int  t;
	t = 221;  //超声波到杯底的距离
	t -= S;	   //用到杯底的距离减去超声波实际测得得距离，就是液位

//在这里对数据进行分段补偿后再进行显示，让最后显示的误差值减小
	if(0<=t<=35)//根据超声波测量值和实际的值进行分段
	{
		t+=2;
	}
	if(36<=t<=43)
	{
		t+=8;
	}
 	if(44<=t<=47)
	{
		t+=15;
	}

	OLED_ShowNum(0,6,t,8,16);//显示	
	Delay_ms(1000);
}

float leilei()
{
	unsigned int i;
	float rou,sum,h;
	sum=0;
	for(i=0; i < 10;i++){
		Get_Weight();
		sum+=(Weight_Shiwu/1000);
	}
	sum = sum/10;
	h = 0;
	abs = 0;
	for(i=0;i<500;i++){
		Conut();
		abs = S;
		h+=abs;
	}
	h=h/500;

   rou=sum/(h*8);
   return rou;		
}		

void jiben(){
	  xianshi();	  
 	   Conut();
		
     	Get_Weight();		
	
		//显示当前重量
		OLED_ShowString(0,2,"wight");
		OLED_ShowNum(48,2,Weight_Shiwu/1000,4,16);		
		OLED_ShowString(96,2,"g");
		
}
//****************************************************
//主函数
//****************************************************
void main()
{
	 
	TMOD=0x21;		   //设T0为方式1，GATE=1；	 

	TH0=0;
	TL0=0;

	OLED_Init();			//初始化OLED 
	
	Get_Maopi();
	Get_Maopi();
	Delay_ms(2000);		 //延时2s
	Get_Maopi();
	Get_Maopi();				//称毛皮重量	//多次测量有利于HX711稳定

    //读取EEPROM中保存的报警值
//    maxValueTable[0] = byte_read(0x2000);
//    maxValueTable[1] = byte_read(0x2001);
//    maxValueTable[2] = byte_read(0x2002);
//    maxValueTable[3] = byte_read(0x2003);
//    Max_Value = maxValueTable[0]*1000+maxValueTable[1]*100+maxValueTable[2]*10+maxValueTable[3];    //计算超限报警界限值
	while(1)
	{
		 jiben();
		
		KEY_NUM = Scan_Key();
		
		if(KEY_NUM == 1){
			k1 = leilei();
			OLED_ShowNum(0,0,k1*100,6,16);
			}
//			Delay_ms(10000);
//			Get_Weight();
//			while(Weight_Shiwu/1000 == 0){
//				OLED_ShowString(0,0,"waiting...");
//				Get_Weight();
//			}
//			
//			Delay_ms(100000);
//			OLED_Clear();
//			k2 = leilei();
//			OLED_ShowNum(0,0,k2*100,4,16);
//			Delay_ms(1000);
//			if(k1>k2){
//	  			OLED_ShowCHinese(0,0,1);
//				OLED_ShowCHinese(16,0,2);
//				OLED_ShowCHinese(32,0,3);
//				}
//			else if(k1<k2){
//				OLED_ShowCHinese(0,0,0);
//				OLED_ShowCHinese(16,0,2);
//				OLED_ShowCHinese(32,0,3);
//				}
//			else
//		    	OLED_ShowString(0,0,"0");
//		}
//
//		if(KEY_NUM == 2){
//			k1 = leilei();
//			if(k1>=x && k1 <= x){
//			
//			}
//		
//		}





//		switch(abs){
//			case 1:	k1 = leilei();OLED_ShowNum(0,0,k1,4,16); break;
//			case 2:	k2 = leilei();OLED_ShowNum(0,0,k2,4,16);break;
//			case 3:		
//			if(k1>k2)
//		  		OLED_ShowString(0,0,"1a");
//			else if(k1<k2)
//				OLED_ShowString(0,0,"2a");
//			else
//			    OLED_ShowString(0,0,"0a");
//			default: break;	
//		}
//
//		k2 = leilei();
		

//		KEY_NUM = Scan_Key();
//        if(KEY_NUM == 1)        //按键1切换设置状态
//        {
//            state++;
//            if(state == 5)
//            {
//                state = 0;
//                SectorErase(0x2000);
//                byte_write(0x2000,maxValueTable[0]);				//保存EEPROM数据
//                byte_write(0x2001,maxValueTable[1]);
//                byte_write(0x2002,maxValueTable[2]);				//保存EEPROM数据
//                byte_write(0x2003,maxValueTable[3]);
//                Max_Value = maxValueTable[0]*1000+maxValueTable[1]*100+maxValueTable[2]*10+maxValueTable[3];    //计算超限报警界限值
//            }
//        }
//        if(KEY_NUM == 2)        //按键加
//        {
//            if(state != 0)
//            {
//                maxValueTable[state-1]++;
//                if(maxValueTable[state-1] >= 10)
//                {
//                    maxValueTable[state-1] = 0;
//                }
//            }
//            
//        }
//        if(KEY_NUM == 3)        //按键减
//        {
//            if(state != 0)
//            {
//                maxValueTable[state-1]--;
//                if(maxValueTable[state-1] <= -1)
//                {
//                    maxValueTable[state-1] = 9;
//                }
//            }
//        }
//		if(KEY_NUM == 4)
//		{
//			Get_Maopi();			//去皮	
//		}
//        
//        
//        if(state != 0)
//        {
//            Blink_Speed ++;
//            if(Blink_Speed == Blink_Speed_Max)
//            {
//                Blink_Speed = 0;
//            }
//        }
//        switch(state)
//        {
//            case 0:
//								OLED_ShowString(0,4,"MAX=");
//								OLED_ShowNum(48,4,maxValueTable[0],1,16);	
//								OLED_ShowNum(64,4,maxValueTable[1],1,16);
//								OLED_ShowNum(80,4,maxValueTable[2],1,16);
//								OLED_ShowNum(96,4,maxValueTable[3],1,16);
//                OLED_ShowString(112,4,"KG");
//                break;
//            case 1:
//                OLED_ShowString(0,4,"MAX=");
//                if(Blink_Speed < Blink_Speed_Max/2)
//                {
//										OLED_ShowNum(48,4,maxValueTable[0],1,16);	
//                }
//                else
//                {
////                    LCD1602_write_data(' ');
//										OLED_ShowString(48,4," ");
//                }
//								OLED_ShowString(56,4,".");
////                LCD1602_write_data(maxValueTable[1]+0x30);
////                LCD1602_write_data(maxValueTable[2]+0x30);
////                LCD1602_write_data(maxValueTable[3]+0x30);
//								OLED_ShowNum(64,4,maxValueTable[1],1,16);	
//								OLED_ShowNum(80,4,maxValueTable[2],1,16);	
//								OLED_ShowNum(96,4,maxValueTable[3],1,16);	
//                OLED_ShowString(112,4,"g");
//                break;
//            case 2:
//                OLED_ShowString(0,4,"MAX=");
//                OLED_ShowNum(48,4,maxValueTable[0],1,16);	
//                OLED_ShowString(56,4,".");
//                if(Blink_Speed < Blink_Speed_Max/2)
//                {
//                    OLED_ShowNum(64,4,maxValueTable[1],1,16);	
//                }
//                else
//                {
//                    OLED_ShowString(64,4," ");
//                }
//                OLED_ShowNum(80,4,maxValueTable[2],1,16);	
//								OLED_ShowNum(96,4,maxValueTable[3],1,16);
//                OLED_ShowString(112,4,"g");
//                break;
//            case 3:
//                OLED_ShowString(0,4,"MAX=");
//                OLED_ShowNum(48,4,maxValueTable[0],1,16);
//                OLED_ShowString(56,4,".");
//                OLED_ShowNum(64,4,maxValueTable[1],1,16);
//                if(Blink_Speed < Blink_Speed_Max/2)
//                {
//                    OLED_ShowNum(80,4,maxValueTable[2],1,16);
//                }
//                else
//                {
//                    OLED_ShowString(80,4," ");
//                }                
//                OLED_ShowNum(96,4,maxValueTable[3],1,16);
//                OLED_ShowString(112,4,"g");
//                break;
//            case 4:
//                OLED_ShowString(0,4,"MAX=");
//                OLED_ShowNum(48,4,maxValueTable[0],1,16);
//                OLED_ShowString(56,4,".");
//                OLED_ShowNum(64,4,maxValueTable[1],1,16);
//                OLED_ShowNum(80,4,maxValueTable[2],1,16);
//                if(Blink_Speed < Blink_Speed_Max/2)
//                {
//                    OLED_ShowNum(96,4,maxValueTable[3],1,16);
//                }
//                else
//                {
//                    OLED_ShowString(96,4," ");
//                }                 
//                OLED_ShowString(112,4,"g");
//                break;
//            default:
//                break;
//            
//        }
        
        //超限报警
//       if(Weight_Shiwu >= Max_Value || Weight_Shiwu >= AlarmValue)	        //超过设置最大值或者传感器本身量程最大值报警	
//		{
//			Buzzer = 0;	
//		}
//		else
//		{
//			Buzzer = 1;
//		}
	}
}

//****************************************************
//称重
//****************************************************
void Get_Weight()
{
	Weight_Shiwu = HX711_Read();
	Weight_Shiwu = Weight_Shiwu - Weight_Maopi;		//获取净重
	if(Weight_Shiwu >= 0)			
	{	
		Weight_Shiwu = (unsigned long)((float)Weight_Shiwu/GapValue); 	//计算实物的实际重量
	}
	else
	{
		Weight_Shiwu = 0;
	}
	
}

//****************************************************
//获取毛皮重量
//****************************************************
void Get_Maopi()
{
	Weight_Maopi = HX711_Read();	
} 


//****************************************************
//MS延时函数(12M晶振下测试)
//****************************************************
void Delay_ms(unsigned int n)
{
	unsigned int  i,j;
	for(i=0;i<n;i++)
		for(j=0;j<123;j++);
}

//****************************************************
//蜂鸣器程序
//****************************************************
void Buzzer_Di()
{
	Buzzer = 0;
	Delay_ms(10);
	Buzzer = 1;
	Delay_ms(10);
}

//****************************************************
//按键扫描程序
//****************************************************
unsigned char Scan_Key()
{	
    if( KEY1 == 0 )						//按键扫描
	{
		Delay_ms(10);					//延时去抖
		if( KEY1 == 0 )
		{
			Buzzer_Di();
			while(KEY1 == 0);			//等待松手
			return 1;
		}
	}
    if( KEY2 == 0 )						//按键扫描
	{
		Delay_ms(10);					//延时去抖
		if( KEY2 == 0 )
		{
			Buzzer_Di();
			while(KEY2 == 0);			//等待松手
			return 2;
		}
	}
    if( KEY3 == 0 )						//按键扫描
	{
		Delay_ms(10);					//延时去抖
		if( KEY3 == 0 )
		{
			Buzzer_Di();
			while(KEY3 == 0);			//等待松手
			return 3;
		}
	}
	if( KEY4 == 0 )						//按键扫描
	{
		Delay_ms(10);					//延时去抖
		if( KEY4 == 0 )
		{
			Buzzer_Di();
			while(KEY4 == 0);			//等待松手
			return 4;
		}
	}
    return 0;
}



