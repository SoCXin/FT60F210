//********************************************************* 
/*	文件名：TEST_FT62F21x_SLEEP.c
*	功能：	 FT62F21x-SLEEP功能演示
*	IC:		 FT62F211 SOP8
*	晶振：   16M/4T                    
*	说明：   此程序为FT62F21x-SLEEP睡眠演示程序，上电之后PA3、PA4同时置高约4s，
*				 然后置低，进入睡眠。测试FT62F21x的睡眠功耗： 
*
*                  FT62F211 SOP8 
*                 ----------------
*  led1----------|1(PA4)     (PA3)8|------------led2     
*  NC-----------|2(TKCAP)  (PA0)7|------------NC
*  VDD----------|3(VDD)     (PA1)6|------------NC
*  GND----------|4(VSS)     (PA2)5|------------NC
*			      ----------------
*/
//*********************************************************
#include	"SYSCFG.h";
#include 	"FT62F21X.h";
//*********************************************************
#define OSC_16M		0X70
#define OSC_8M		0X60
#define OSC_4M		0X50
#define OSC_2M		0X40
#define OSC_1M		0X30
#define OSC_500K	0X20
#define OSC_250K	0X10
#define OSC_32K		0X00

#define WDT_256K	0X80
#define WDT_32K		0X00
//**********************************************************
//***********************宏定义*****************************
#define  unchar     unsigned char 
#define  unint      	unsigned int
#define  unlong     unsigned long
#define  led1			RA4
#define  led2			RA3
 /*-------------------------------------------------
 *	函数名：POWER_INITIAL
 *	功能：  上电系统初始化
 *	输入：  无
 *	输出：  无
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{
	OSCCON = WDT_32K|OSC_16M|0X00;	
  //OSCCON = 0B01110000;		//WDT 32KHZ IRCF=111=16MHZ/4=4MHZ,0.25US/T

	INTCON = 0;  							//暂禁止所有中断

	PORTA = 0B00000000;				
	TRISA = 0B00000000;				//PA输入输出 0-输出 1-输入  
	WPUA = 0B00000000;     		//PA端口上拉控制 1-开上拉 0-关上拉

	OPTION = 0B00001000;			//Bit3=1 WDT MODE,PS=000=1:1 WDT RATE
													//Bit7(PAPU)=0 由WPUA决定是否上拉
	MSCON = 0B00000000;		                             
}
 /*-------------------------------------------------
 *  函数名称：DelayUs
 *  功能：    短延时函数 --16M-4T--大概快1%左右.
 *  输入参数：Time 延时时间长度 延时时长Time*2 Us
 * 	返回参数：无 
 -------------------------------------------------*/
void DelayUs(unsigned char Time)
{
	unsigned char a;
	for(a=0;a<Time;a++)
	{
		NOP();
	}
}                  
/*------------------------------------------------- 
 * 	函数名称： DelayMs
 * 	功能：    短延时函数
 * 	输入参数：Time 延时时间长度 延时时长Time ms
 * 	返回参数：无 
 -------------------------------------------------*/
void DelayMs(unsigned char Time)
{
	unsigned char a,b;
	for(a=0;a<Time;a++)
	{
		for(b=0;b<5;b++)
		{
		 	DelayUs(98);               //快1%
		}
	}
}
/*------------------------------------------------- 
 * 	函数名称：DelayS
 * 	功能：   短延时函数
 * 	输入参数：Time 延时时间长度 延时时长Time S
 * 	返回参数：无 
 -------------------------------------------------*/
void DelayS(unsigned char Time)
{
	unsigned char a,b;
	for(a=0;a<Time;a++)
	{
		for(b=0;b<10;b++)
		{
		 	DelayMs(100); 
		}
	}
}
/*-------------------------------------------------
 *	函数名: main 
 *	功能：  主函数
 *	输入：  无
 *	输出：  无
 --------------------------------------------------*/
void main(void)
{
	POWER_INITIAL();		//系统初始化
	led1 = 1;
    led2 = 1;
    DelayS(4);
    led1 = 0;
    led2 = 0;

    while(1)
	{
    	CLRWDT();  		    //清看门狗
		NOP();
     	SLEEP(); 
		NOP();
        led1 =~led1;
	}
}
