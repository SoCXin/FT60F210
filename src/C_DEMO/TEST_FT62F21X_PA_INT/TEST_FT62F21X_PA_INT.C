//********************************************************* 
/*	文件名：TEST_FT62F21x_PA_INT.c
*	功能：	 FT62F21x-PA口电平中断功能演示
*	IC:		 FT62F211 SOP8
*	晶振：   16M/4T                    
*	说明：   程序中DemoPortOut(PA4)输出100帧50HZ的占空比为50%的方波后,MCU进入睡眠,等待中断的发生;
*		   		 当每次PA2电平变化中断触发后，重复以上流程;
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
#define  DemoPortOut	RA4   
 
unchar FCount;

unchar ReadAPin;
/*-------------------------------------------------
 *	函数名：interrupt ISR
 *	功能：  中断处理函数
 *	输入：  无
 *	输出：  无
 --------------------------------------------------*/
void interrupt ISR(void)	//PIC_HI-TECH使用
{ 
  //PA电平变化中断**********************
	 if(PAIE && PAIF)		
    {
		ReadAPin = PORTA; 	//读取PORTA数据清PAIF标志
		PAIF = 0;  			//清PAIF标志位
		PAIE = 0;  			//暂先禁止PA0中断
		IOCA2 =0;  			//禁止PA0电平变化中断
		 
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
 *	函数名: PA2_Level_Change_INITIAL
 *	功能：  PA端口(PA2)电平变化中断初始化
 *	输入：  无
 *	输出：  无
--------------------------------------------------*/
void PA2_Level_Change_INITIAL(void)
{
 
	TRISA2 =1; 			     //SET PA2 INPUT
	ReadAPin = PORTA;	 //清PA电平变化中断
	PAIF =0;   			     //清PA INT中断标志位
    IOCA2 =1;  			     //使能PA2电平变化中断
	PAIE =1;   			     //使能PA INT中断
    //GIE =1;    				 //使能全局中断
}
/*-------------------------------------------------
 *	函数名: main 
 *	功能：  主函数
 *	输入：  无
 *	输出：  无
 --------------------------------------------------*/
void main()
{
	POWER_INITIAL();						//系统初始化
 
	while(1)
	{
		for(FCount=0;FCount<100;FCount++)	//输出100次波形	
		{
			DemoPortOut = 1; 				
			DelayMs(10);  					
			DemoPortOut = 0;
			DelayMs(10); 
		}
		PA2_Level_Change_INITIAL();			//初始化PA电平变化中断
		GIE = 1;											//开总中断
        NOP();
        NOP();
        
		SLEEP(); 											//睡眠
        NOP();
        NOP();
	}
}
