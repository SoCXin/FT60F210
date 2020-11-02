//********************************************************* 
/*  文件名：Test_62F21X_Timer2.c
*	功能：  FT62F21X_Timer2功能演示
*   IC:    FT62F21X SOP8
*   晶振：  16M/4T                    
*   说明：  当DemoPortIn悬空或者高电平时,
*		   DemoPortOut输出2.5KHz占空比50%的波形-Tm2实现
*		   当DemoPortIn接地时,DemoPortOut输出高电平.关定时器
* Memory: Flash 1KX14b, EEPROM 128X8b, SRAM 64X8b
*                  							FT62F21X SOP8 
*                 								----------------
*  DemoPortOut ------------|1(PA4)    		 	(PA3)8 |-------------NC 
*  NC------------|2(TKCAP)   	 					(PA0)7 |-------------NC
*  NC------------|3(VDD)								(PA1)6 |-------------NC 
*  NC------------|4(VSS)   	 						(PA2)5	|-------------DemoPortIn
*			     								 ----------------
*/
//===========================================================
//===========================================================
#include	"SYSCFG.h";
#include 	"FT62F21X.h";

#define  unchar     unsigned char 
#define  unint      unsigned int
#define  unlong     unsigned long

//===========================================================
//
//	系统时钟
//===========================================================
#define OSC_16M  0X70
#define OSC_8M   0X60
#define OSC_4M   0X50
#define OSC_2M   0X40
#define OSC_1M   0X30
#define OSC_500K 0X20
#define OSC_250K 0X10
#define OSC_32K  0X00

#define  DemoPortOut	RA4  
#define  DemoPortIn		RA2
//===========================================================
//变量定义
//===========================================================

//===========================================================
//Funtion name：interrupt ISR
//parameters：无
//returned value：无
//===========================================================
void interrupt ISR(void)
{
  //定时器2的中断处理**********************
	if(TMR2IE && TMR2IF)			//200us中断一次 = 2.5KHz
	{
		DemoPortOut = ~DemoPortOut; //翻转电平
        TMR2IF = 0;
	} 
}

/*-------------------------------------------------
 * 函数名：POWER_INITIAL
 *	功能：  上电系统初始化
 * 输入：  无
 * 输出：  无
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{ 
	OSCCON = OSC_16M;	//	bit7	Timer2 选择LIRC为时钟源时 LIRC的频率选择  0:32KHz	1:256KHz
										//bit[6:4]	系统频率选择	
										//bit[2]		高速内部时钟状态	1:ready	0:not ready
										//bit[1]		低速内部时钟状态	1:ready	0:not ready

	INTCON = 0;  				//暂禁止所有中断
	OPTION = 0;
    TRISA	= 1<<2;			//1:输入	0:输出		
    PSRCA 	= 0;					//00:	4mA		01/10:	8mA		11:	28mA	bit[3:2]控制PA5源电流	bit[1:0]控制PA4源电流
    PSINKA	= 0;					//bit[1:0]	控制PA5和PA4		0:灌电流最小	1:灌电流最大
    PORTA 	= 0;					//1:PAx输出高电平	0:PAx输出低电平
 	WPUA 	= 1<<2;			//1:	使能PA口上拉	0:关闭PA口上拉   
}
/*-------------------------------------------------
 * 函数名称：  TIMER2_INITIAL
 * 功能：     初始化设置定时器1 
 * 相关寄存器：T2CON TMR2 PR2 TMR2IE TMR2IF PEIE GIE  
 -------------------------------------------------*/
void TIMER2_INITIAL (void) 
{
	T2CON0 = 0B00000001; 	//Bit[1,0]=01,T2时钟分频 1:4
											//Bit[6-3]=0000,T2输出时钟分频1:1 
    T2CON1 = 0B00001000;	//Bit[2:0] 000:	指令周期	100:HIRC		Timer2时钟源选择	
    TMR2H = 0;
	TMR2L = 0;  					//TMR2赋初值
    PR2H =0;
	PR2L = 200; 					//设置TMR2输出比较值定时200us=(1/16000000)*4*4*200(PR2)
											//16M-4T-4分频 
	TMR2IF = 0;						//清TIMER2中断标志
	TMR2IE = 1;						//使能TIMER2的中断
	TMR2ON = 1;					//使能TIMER2启动
	PEIE=1;    						//使能外设中断
	GIE = 1;   						//使能全局中断
}
/*-------------------------------------------------
 * 函数名: main 
 *	功能：  主函数
 * 输入：  无
 * 输出：  无
 --------------------------------------------------*/
void main()
{
	POWER_INITIAL();			//系统初始化
    
	TIMER2_INITIAL();  			//初始化T2
	
	while(1)
	{ 
		if(DemoPortIn == 1)	//判断输入是否为高电平 
		{
			TMR2IE = 1; 			//开定时器2 
		}
		else
		{
			TMR2IE = 0; 			//关定时器2
			DemoPortOut = 1;
		}  
	}
}
//===========================================================
