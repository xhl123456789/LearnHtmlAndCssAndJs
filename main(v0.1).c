#include "reg52.h"
#include "oled.h"
#include "HX711.h"
#include "EEPROM.H"
#include <reg52.h>

sbit Buzzer = P2^4;
float k1,k2;
int abs;

//���� IO����
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

//�������
unsigned char KEY_NUM = 0;   //������Ű������µļ�ֵ
unsigned long HX711_Buffer = 0;  //�������HX711��ȡ����������
unsigned long Weight_Maopi = 0; //�������ëƤ����
long Weight_Shiwu = 0;          //�������ʵ������
long Max_Value = 0;             //��������������ֵ
char maxValueTable[4] = {1,0,0,0};
unsigned char state = 0;    //�����������״̬
unsigned char Blink_Speed = 0;

float tsum=0.0;  //���ÿһ�ε�ʱ��
unsigned int icount=0;

#define Blink_Speed_Max 6          //��ֵ���Ըı�����ָ����˸Ƶ��

//У׼����
//��Ϊ��ͬ�Ĵ������������߲��Ǻ�һ�£���ˣ�ÿһ����������Ҫ�������������������ʹ����ֵ��׼ȷ��
//�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
//������Գ���������ƫСʱ����С����ֵ��
//��ֵ����ΪС��
#define GapValue 430

//������������ֵ����λ��g
#define AlarmValue 5000			


int  time=0;
float     s=0;
	
sbit RX=P3^6;               //Echo
sbit TX=P3^7;               //Trig



void  StartModule() 		         //T1�ж�����ɨ������ܺͼ�800MS����ģ��
  {
  unsigned char a,b;
	  TX=1;			                 //800MS  ����һ��ģ��
	  for(b=3;b>0;b--)
        for(a=10;a>0;a--);
	  TX=0;
  }

/*void Conut(void)
   {
	
	 StartModule();
	 while(!RX);		//��RXΪ��ʱ�ȴ�
	 TR0=1;			    //��������
	 while(RX);			//��RXΪ1�������ȴ�
	 TR0=0;				//�رռ���
	 time=TH0*256+TL0;
	 TH0=0;
	 TL0=0;
	 S=(time*1.87)/10;     //�������mm
   }*/

void Conut(void)
   {
	
	 StartModule();
	 while(!RX);		//��RXΪ��ʱ�ȴ�
	 TR0=1;			    //��������
	 while(RX);			//��RXΪ1�������ȴ�
	 TR0=0;				//�رռ���
	 time=TH0*256+TL0;
	 tsum += time;
	 icount++;
	 TH0=0;
	 TL0=0;
	 if(icount==10)
	{
		icount++;
		s=(time*1.87)/100;     //�������mm
	}
	
   }
		
	void xianshi()
{
  	unsigned int  t;
	t = 221;  //�����������׵ľ���
	t -= S;	   //�õ����׵ľ����ȥ������ʵ�ʲ�õþ��룬����Һλ

//����������ݽ��зֶβ������ٽ�����ʾ���������ʾ�����ֵ��С
	if(0<=t<=35)//���ݳ���������ֵ��ʵ�ʵ�ֵ���зֶ�
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

	OLED_ShowNum(0,6,t,8,16);//��ʾ	
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
	
		//��ʾ��ǰ����
		OLED_ShowString(0,2,"wight");
		OLED_ShowNum(48,2,Weight_Shiwu/1000,4,16);		
		OLED_ShowString(96,2,"g");
		
}
//****************************************************
//������
//****************************************************
void main()
{
	 
	TMOD=0x21;		   //��T0Ϊ��ʽ1��GATE=1��	 

	TH0=0;
	TL0=0;

	OLED_Init();			//��ʼ��OLED 
	
	Get_Maopi();
	Get_Maopi();
	Delay_ms(2000);		 //��ʱ2s
	Get_Maopi();
	Get_Maopi();				//��ëƤ����	//��β���������HX711�ȶ�

    //��ȡEEPROM�б���ı���ֵ
//    maxValueTable[0] = byte_read(0x2000);
//    maxValueTable[1] = byte_read(0x2001);
//    maxValueTable[2] = byte_read(0x2002);
//    maxValueTable[3] = byte_read(0x2003);
//    Max_Value = maxValueTable[0]*1000+maxValueTable[1]*100+maxValueTable[2]*10+maxValueTable[3];    //���㳬�ޱ�������ֵ
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
//        if(KEY_NUM == 1)        //����1�л�����״̬
//        {
//            state++;
//            if(state == 5)
//            {
//                state = 0;
//                SectorErase(0x2000);
//                byte_write(0x2000,maxValueTable[0]);				//����EEPROM����
//                byte_write(0x2001,maxValueTable[1]);
//                byte_write(0x2002,maxValueTable[2]);				//����EEPROM����
//                byte_write(0x2003,maxValueTable[3]);
//                Max_Value = maxValueTable[0]*1000+maxValueTable[1]*100+maxValueTable[2]*10+maxValueTable[3];    //���㳬�ޱ�������ֵ
//            }
//        }
//        if(KEY_NUM == 2)        //������
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
//        if(KEY_NUM == 3)        //������
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
//			Get_Maopi();			//ȥƤ	
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
        
        //���ޱ���
//       if(Weight_Shiwu >= Max_Value || Weight_Shiwu >= AlarmValue)	        //�����������ֵ���ߴ����������������ֵ����	
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
//����
//****************************************************
void Get_Weight()
{
	Weight_Shiwu = HX711_Read();
	Weight_Shiwu = Weight_Shiwu - Weight_Maopi;		//��ȡ����
	if(Weight_Shiwu >= 0)			
	{	
		Weight_Shiwu = (unsigned long)((float)Weight_Shiwu/GapValue); 	//����ʵ���ʵ������
	}
	else
	{
		Weight_Shiwu = 0;
	}
	
}

//****************************************************
//��ȡëƤ����
//****************************************************
void Get_Maopi()
{
	Weight_Maopi = HX711_Read();	
} 


//****************************************************
//MS��ʱ����(12M�����²���)
//****************************************************
void Delay_ms(unsigned int n)
{
	unsigned int  i,j;
	for(i=0;i<n;i++)
		for(j=0;j<123;j++);
}

//****************************************************
//����������
//****************************************************
void Buzzer_Di()
{
	Buzzer = 0;
	Delay_ms(10);
	Buzzer = 1;
	Delay_ms(10);
}

//****************************************************
//����ɨ�����
//****************************************************
unsigned char Scan_Key()
{	
    if( KEY1 == 0 )						//����ɨ��
	{
		Delay_ms(10);					//��ʱȥ��
		if( KEY1 == 0 )
		{
			Buzzer_Di();
			while(KEY1 == 0);			//�ȴ�����
			return 1;
		}
	}
    if( KEY2 == 0 )						//����ɨ��
	{
		Delay_ms(10);					//��ʱȥ��
		if( KEY2 == 0 )
		{
			Buzzer_Di();
			while(KEY2 == 0);			//�ȴ�����
			return 2;
		}
	}
    if( KEY3 == 0 )						//����ɨ��
	{
		Delay_ms(10);					//��ʱȥ��
		if( KEY3 == 0 )
		{
			Buzzer_Di();
			while(KEY3 == 0);			//�ȴ�����
			return 3;
		}
	}
	if( KEY4 == 0 )						//����ɨ��
	{
		Delay_ms(10);					//��ʱȥ��
		if( KEY4 == 0 )
		{
			Buzzer_Di();
			while(KEY4 == 0);			//�ȴ�����
			return 4;
		}
	}
    return 0;
}



