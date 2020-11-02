//********************************************************* 
/*	文件名：TEST_FT62F21x_INT.c
*	功能：	 FT62F21x-INT功能演示
*	IC:		 FT62F211 SOP8
*	晶振：   16M/4T                    
*	说明：   程序中DemoPortOut(PA4)输出100帧50HZ的占空比为50%的方波后, MCU进入睡眠, 等待外部中断的发生；
*		  		 当外部中断触发后，重复以上流程;
*
*                  FT62F211 SOP8 
*                 ----------------
*  DemoPortOut-----|1(PA4)     (PA3)8|------------NC    
*  NC----------------|2(TKCAP)  (PA0)7|------------NC
*  VDD--------------|3(VDD)     (PA1)6|------------NC
*  GND--------------|4(VSS)     (PA2)5|------------INT
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
#define  DemoPortOut	RA4   
 
unchar FCount;
/*-------------------------------------------------
 *	函数名：interrupt ISR
 *	功能：  中断处理函数
 *	输入：  无
 *	输出：  无
 --------------------------------------------------*/
void interrupt ISR(void)//PIC_HI-TECH使用
{ 
  //PA2外部中断处理**********************
	if(INTE && INTF)	
	{
		INTF = 0;  		//清PA2 INT 标志位
		INTE = 0;  		//暂先禁止PA2中断
	}
}
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
	TRISA = 0B00000100;				//PA输入输出 0-输出 1-输入  
	WPUA = 0B00000100;     		//PA端口上拉控制 1-开上拉 0-关上拉

	OPTION = 0B01001000;			//Bit3=1 WDT MODE,PS=000=1:1 WDT RATE
													//Bit6(INTEDG)=1 PA2/INT上升沿产生中断
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
 *	函数名: INT_INITIAL 
 *	功能：  中断初始化函数
 *	输入：  无
 *	输出：  无
 --------------------------------------------------*/
void INT_INITIAL(void)
{
	TRISA2 =1; 						//SET PA2 INPUT
	INTEDG = 1; 					//OPTION,INTEDG=1;PA2 INT 为上升沿触发 
	INTF =0;   						//清PA2 INT中断标志位
	INTE =1;   						//使能PA2 INT中断
	//GIE =1;    						//使能全局中断
}
/*-------------------------------------------------
 *	函数名: main 
 *	功能：  主函数
 *	输入：  无
 *	输出：  无
 --------------------------------------------------*/
void main()
{
	POWER_INITIAL();				//系统初始化
	while(1)
	{
		for(FCount=0;FCount<100;FCount++)//输出100次波形	
		{
			DemoPortOut = 1; 		
			DelayMs(10);  			     //10ms 
			DemoPortOut = 0;
			DelayMs(10); 
		}
		INT_INITIAL();				     //初始化外部中断
		GIE = 1;					     	 //开总中断
		SLEEP(); 					         //睡眠
	}
}
