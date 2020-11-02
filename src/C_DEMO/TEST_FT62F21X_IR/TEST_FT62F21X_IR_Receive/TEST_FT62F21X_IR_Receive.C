//********************************************************* 
/*	文件名：TEST_FT62F21x_IR_Receive.c
*	功能：	 FT62F21x-红外接收 功能演示
*	IC:		 FT62F211 SOP8
*	晶振：   16M/4T                    
*	说明：   演示程序中，IR红外是采用6122协议，起始信号是9ms低电平，到4.5ms高电平，再到低8位
*           	 用户识别码，到高8位的用户识别码，8位数据码，8位数据码的反码。RXIO（PA2）每次收到
*           	 遥控器发过来的数据后，如果数据合法（两对补码，不对内容判断），LED（PA4）开关状态
*		    	 就改变一次。
*
*                  FT62F211 SOP8 
*                 ----------------
*  LED-----------|1(PA4)     (PA3)8|------------NC     
*  NC-----------|2(TKCAP)  (PA0)7|------------NC
*  VDD----------|3(VDD)     (PA1)6|------------NC
*  GND----------|4(VSS)     (PA2)5|------------IRRIO
*			      ----------------
*/
//*********************************************************
#include	"SYSCFG.h";
#include 	"FT62F21X.h";
//***********************宏定义*****************************
#define  uchar		unsigned char 
#define  uint			unsigned int
#define  ulong		unsigned long

#define  LED			RA4		//LED指示灯的IO  
#define  IRRIO		RA2		//IR的接收脚 
 
uchar IRbitNum = 0;		    //用于记录接收到第几位数据了
uchar IRbitTime = 0;			//用于计时一位的时间长短
uchar IRDataTimer[4];		//存出来的4个数据
uchar bitdata=0x01;			//用于按位或的位数据
uchar ReceiveFinish = 0;	//用于记录接收完成
uchar ReadAPin = 0;			//用于读取IO口状态，电平变化中断标志清除
uchar rdata1,rdata2;
/*-------------------------------------------------
 *	函数名：interrupt ISR
 *	功能： 	 定时器0中断和PA电平变化中断
 *	输入：	 无
 *	输出： 	 无
 --------------------------------------------------*/
void interrupt ISR(void)			//PIC_HI-TECH使用
{ 
   
  //定时器0的中断处理**********************
	if(T0IE && T0IF)					//104us
	{
		TMR0 = 140;                	//注意:对TMR0重新赋值TMR0在两个周期内不变化
		 
		T0IF = 0;
        IRbitTime++;
        if(IRbitTime > 50)
        {
        	T0IE = 0;
            IRbitTime = 0;
        }
	} 
    
    //PA电平变化中断**********************
	if(PAIE && PAIF)		
    {
		ReadAPin = PORTA; 			//读取PORTA数据清PAIF标志
		PAIF = 0; 
        if(IRRIO == 0)
        {
        	T0IE = 1;
        	if(IRbitTime > 21)
            {
            	IRDataTimer[0] = 0;
                IRDataTimer[1] = 0;
                IRDataTimer[2] = 0;
                IRDataTimer[3] = 0;
                IRbitNum = 0;
                bitdata = 0x00;
            }
            else if(IRbitTime > 3)
            {
            	IRDataTimer[IRbitNum-1] |= bitdata;
            }
            IRbitTime = 0;
            bitdata<<=1;
            if(bitdata == 0)
            {
            	bitdata = 0x01;
                IRbitNum++;
            }
            if(IRbitNum > 4)
            {
            	IRbitNum = 0;
                T0IE = 0;  
                ReceiveFinish = 1;		
            }

        }
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
    OSCCON = 0B01110000;			//WDT 32KHZ IRCF=111=16MHZ/4=4MHZ,0.25US/T

	INTCON = 0;  							//暂禁止所有中断

	PORTA = 0B00000000;				
	TRISA = 0B00000100;				//PA输入输出 0-输出 1-输入  
	WPUA = 0B00000100;     		//PA端口上拉控制 1-开上拉 0-关上拉

	OPTION = 0B00001000;			//Bit3=1 WDT MODE,PS=000=1:1 WDT RATE
													//Bit7(PAPU)=0 由WPUA决定是否上拉
	MSCON = 0B00000000;		                             
}
/*----------------------------------------------------
 *	函数名称：TIMER0_INITIAL
 *	功能：初始化设置定时器
 *	相关寄存器：T0CS T0CS T0SE PSA 
 *	设置TMR0定时时长560us=(1/16000000)*4*16*140(16M-2T-PSA 1:16- TMR0=255溢出)	                      
 ----------------------------------------------------*/
void TIMER0_INITIAL (void)  
{
	OPTION = 0B00000011; 		//预分频器分配给Timer0，预分频比为1:16，上升沿   
	//Bit5 T0CS Timer0时钟源选择 
	//1-外部引脚电平变化T0CKI 0-内部时钟(FOSC/2)
	//Bit4 T0CKI引脚触发方式 1-下降沿 0-上升沿
	//Bit3 PSA 预分频器分配位 0-Timer0 1-WDT 
	//Bit2:0 PS2 8个预分频比 011 - 1:16
    						
	TMR0 = 140; 
    T0IF = 0;								//清空T0软件中断
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
    TIMER0_INITIAL();
    PA2_Level_Change_INITIAL();		//初始化PA端口电平中断
	GIE = 1;										//开总中断
	while(1)
	{
		if(ReceiveFinish)
        {
        	ReceiveFinish = 0;
            rdata1 = 0xFF - IRDataTimer[0];
            rdata2 = 0xFF - IRDataTimer[2];
            if((rdata1 == IRDataTimer[1])&&(rdata2 == IRDataTimer[3]))
            {
            	LED = ~LED; 		//翻转电平
            }
        }
	}
}