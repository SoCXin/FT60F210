//********************************************************* 
/*	ÎÄ¼şÃû£ºTEST_FT62F21x_IR_Send.c
*	¹¦ÄÜ£º	 FT62F21x-ºìÍâ·¢Éä  ¹¦ÄÜÑİÊ¾
*	IC:		 FT62F211 SOP8
*	¾§Õñ£º   16M/4T                    
*	ËµÃ÷£º   ÑİÊ¾³ÌĞòÖĞ,IRºìÍâÊÇ²ÉÓÃ6122Ğ­Òé£¬ÆğÊ¼ĞÅºÅÊÇ9msµÍµçÆ½£¬µ½4.5ms¸ßµçÆ½£¬ÔÙµ½µÍ8Î»
*          	 ÓÃ»§Ê¶±ğÂë£¬µ½¸ß8Î»µÄÓÃ»§Ê¶±ğÂë£¬8Î»Êı¾İÂë£¬8Î»Êı¾İÂëµÄ·´Âë¡£SendIO£¨PA4£©¶¨Ê±
*         		£¨5ÃëÖÓ£©·¢ËÍÒ»´Î£¬½ÓÊÕ¶ËÊÕµ½Ò£¿ØÆ÷·¢¹ıÀ´µÄÊı¾İºó£¬Ğ£ÑéÊı¾İ»¥Îª²¹Âë£¬LED»á¿ª¹Ø¡£
*
*                  FT62F211 SOP8 
*                 ----------------
*  IRSendIO-----|1(PA4)     (PA3)8|------------NC     
*  NC-----------|2(TKCAP)  (PA0)7|------------NC
*  VDD----------|3(VDD)     (PA1)6|------------NC
*  GND----------|4(VSS)     (PA2)5|------------NC
*			      ----------------
*/
//*********************************************************
#include	"SYSCFG.h";
#include 	"FT62F21X.h";
//***********************ºê¶¨Òå*****************************å
#define  uchar	unsigned char 
#define  uint		unsigned int
#define  ulong	unsigned long

#define  IRSendIO		RA4  								//´®¿ÚµÄ·¢ËÍ½Å

#define IRSend_HIGH_1   1  								//560uS
#define IRSend_LOW_1    3 								//1680uS

#define IRSend_HIGH_0   1  								//560uS
#define IRSend_LOW_0    1  							//560uS
	
#define IRSend_PIN_1   T0IE = 1  					//·¢ËÍÊı¾İ  ¿ªÆô¶¨Ê±Æ÷0
#define IRSend_PIN_0   T0IE = 0 						//¹Ø±Õ¶¨Ê±Æ÷0

#define Status_NOSend	0			           			 //²»·¢ËÍµÄ×´Ì¬
#define Status_Head		1			           				//·¢ËÍÒıµ¼ÂëµÄ×´Ì¬
#define Status_Data		2			            			//·¢ËÍÊı¾İµÄ×´Ì¬

uchar IRSendStatus;											//·¢ËÍ×´Ì¬£¬ÊÇ·¢ËÍÒıµ¼Âë»¹ÊÇÊı¾İ
uchar IRSendData;											//·¢ËÍµÄÊı¾İÖĞ×ª±äÁ¿
uchar  TxBit=0,TxTime=0; 
uchar Sendbit = 0;
uchar level0,level1;											//Ò»Î»Êı¾İÀï·¢ËÍÓë¹Ø±ÕµÄÊ±¼äÖµ
bit SendLastBit = 0;
uchar SaveLastBit = 0;
uint SYSTime5S = 0;											//ÏµÍ³Ê±¼ä£¬5S·¢ËÍÒ»´Î

uchar IRData[4] = {0x00,0xff,0x40,0xBf};		//ĞèÒª·¢ËÍµÄ4¸öÊı¾İ

 /*-------------------------------------------------
 *	º¯ÊıÃû£ºPOWER_INITIAL
 *	¹¦ÄÜ£º  ÉÏµçÏµÍ³³õÊ¼»¯
 *	ÊäÈë£º  ÎŞ
 *	Êä³ö£º  ÎŞ
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{	
    OSCCON = 0B01110000;			//WDT 32KHZ IRCF=111=16MHZ/4=4MHZ,0.25US/T

	INTCON = 0;  							//Ôİ½ûÖ¹ËùÓĞÖĞ¶Ï

	PORTA = 0B00000000;				
	TRISA = 0B00000000;				//PAÊäÈëÊä³ö 0-Êä³ö 1-ÊäÈë  
	WPUA = 0B00000000;     		//PA¶Ë¿ÚÉÏÀ­¿ØÖÆ 1-¿ªÉÏÀ­ 0-¹ØÉÏÀ­

	OPTION = 0B00001000;			//Bit3=1 WDT MODE,PS=000=1:1 WDT RATE
													//Bit7(PAPU)=0 ÓÉWPUA¾ö¶¨ÊÇ·ñÉÏÀ­
	MSCON = 0B00000000;		                             
}
/*----------------------------------------------------
 *	º¯ÊıÃû³Æ£ºTIMER0_INITIAL
 *	¹¦ÄÜ£º³õÊ¼»¯ÉèÖÃ¶¨Ê±Æ÷0
 * ËµÃ÷£º38KHz·¢ÉúÆ÷£¬1000000/38000=26.3uS
 *	ÉèÖÃTMR0¶¨Ê±Ê±³¤13us=(1/16000000)*4*2*26(16M-4T-PSA 1:2- TMR0=255Òç³ö)	                     
 ----------------------------------------------------*/
void TIMER0_INITIAL (void)  
{
	OPTION = 0B00000000; 		//Ô¤·ÖÆµÆ÷·ÖÅä¸øTimer0£¬Ô¤·ÖÆµ±ÈÎª1:2£¬ÉÏÉıÑØ   
	//Bit5 T0CS Timer0Ê±ÖÓÔ´Ñ¡Ôñ 
	//1-Íâ²¿Òı½ÅµçÆ½±ä»¯T0CKI 0-ÄÚ²¿Ê±ÖÓ(FOSC/2)
	//Bit4 T0CKIÒı½Å´¥·¢·½Ê½ 1-ÏÂ½µÑØ 0-ÉÏÉıÑØ
	//Bit3 PSA Ô¤·ÖÆµÆ÷·ÖÅäÎ» 0-Timer0 1-WDT 
	//Bit2:0 PS2 8¸öÔ¤·ÖÆµ±È 011 - 1:16
    						
	TMR0 = 239; 
    T0IF = 0;								//Çå¿ÕT0Èí¼şÖĞ¶Ï
}
/*-------------------------------------------------
 * º¯ÊıÃû³Æ£ºTIMER2_INITIAL
 * ¹¦ÄÜ£º³õÊ¼»¯ÉèÖÃ¶¨Ê±Æ÷2
--------------------------------------------------*/
void TIMER2_INITIAL(void)
{ 
	T2CON0 = 0B00000011; 		//Ô¤·ÖÆµ±ÈÎª1:16£¬ºó·ÖÆµ±ÈÎª1:1  	 
    
	TMR2H = 0;  						//TMR2¸³³õÖµ
    TMR2L = 0;
        
	PR2L = 140; 						//ÉèÖÃTMR2Êä³ö±È½ÏÖµ¶¨Ê±560us=(1/16000000)*4*16*140(PR2)
	PR2H = 0;							//16M-4T-4·ÖÆµ 
	//PR2 = 60; 							//ÉèÖÃTMR2Êä³ö±È½ÏÖµ¶¨Ê±15us=(1/4000000)*60(PR2)
	TMR2IF = 0;							//ÇåTMER2ÖĞ¶Ï±êÖ¾
	TMR2IE = 1;							//Ê¹ÄÜTMER2µÄÖĞ¶Ï
	TMR2ON = 1;						//Ê¹ÄÜTMER2Æô¶¯
	PEIE=1;    							//Ê¹ÄÜÍâÉèÖĞ¶Ï
}
/*-------------------------------------------------
 *	º¯ÊıÃû£º SendCtrl
 *	¹¦ÄÜ£º   ·¢ËÍÊı¾İº¯Êı
 *	ÊäÈë£º   ÎŞ
 *	Êä³ö£º   ÎŞ
 --------------------------------------------------*/
void SendCtrl(void)
{

	if (IRSendStatus == Status_NOSend)			//²»·¢ËÍµÄ×´Ì¬
	{        
		IRSend_PIN_0;
        Sendbit = 0;
		TxTime = 0;
        
	}	 
	else if (IRSendStatus == Status_Head)		//·¢ËÍÒıµ¼Âë
	{
    	TxTime++;
		if (TxTime < 17)   						//·¢ËÍ9mSĞÅºÅ
		{
			IRSend_PIN_1;
		}
        else if (TxTime < 24)   				//4.5mS²»·¢ËÍ
		{
			IRSend_PIN_0;
		}
		else
		{
			TxTime = 0;
            IRSendStatus = Status_Data;
		}
        IRSendData = IRData[0];
        TxBit = 0x01;
	}
	else if(IRSendStatus == Status_Data)		//·¢ËÍÊı¾İ
	{
		if (IRSendData & TxBit)  				//1£¬ÊÇ1:3µÄÊ±¼ä
		{
			level1 = IRSend_HIGH_1;
			level0 = IRSend_LOW_1;
		}
		else									//0£¬ÊÇ1:1µÄÊ±¼ä
		{
			level1 = IRSend_HIGH_0;
			level0 = IRSend_LOW_0;
		}
		TxTime++;
		if (TxTime <= level1)  					//·¢ËÍĞÅºÅ
		{
			IRSend_PIN_1;
		}
		else if (TxTime <= (level0+level1)) 	//²»·¢ËÍĞÅºÅ
		{
			IRSend_PIN_0;
		}
		else if (Sendbit < 4)					//·¢ËÍ4Î»Êı¾İÎ´Íê³É
		{
			TxTime = 1;
            IRSend_PIN_1;
            SaveLastBit = IRSendData & TxBit;
			TxBit <<= 1;
			if (TxBit == 0x00)  				//·¢ËÍÍêÒ»¸ö×Ö½Ú
			{
				TxBit = 0x01;
                Sendbit++;
                IRSendData = IRData[Sendbit];
				if (Sendbit > 3)   				//×îºóÒ»Î»Òª×¢Òâ£¬ÒòÎª·¢ËÍÍêÁË»¹ÒªÓĞÒ»¸öÂö³å
				{
                    SendLastBit = 1;
				}
			}
		}
        else									//Êı¾İÍê³ÉÁË£¬Òª²¹Âö³å
        {
        	if(SendLastBit)
 		   	{
		    	TxTime++;
		    	if(SaveLastBit)
		        {
		        	if(TxTime < 3)
		            {
		            	IRSend_PIN_0;
		            }
		            else if(TxTime < 4)
		            {
		            	IRSend_PIN_1;
		            }
		            else
		            {
		            	IRSend_PIN_0;
						IRSendStatus = Status_NOSend;
		                IRSend_PIN_0;
		                SendLastBit = 0;
                        TxBit = 0;
                        TxTime = 0;
		            }
		        }
		        else
		        {
		        	if(TxTime < 5)
		            {
		            	IRSend_PIN_0;
		            }
		            else if(TxTime < 6)
		            {
		            	IRSend_PIN_1;
		            }
		            else
		            {
		            	IRSend_PIN_0;
						IRSendStatus = Status_NOSend;
		                IRSend_PIN_0;
		                SendLastBit = 0;
                        TxBit = 0;
                        TxTime = 0;
		            }
		        }
		    }
        }
	}
    
}
/*-------------------------------------------------
 *	º¯ÊıÃû£ºinterrupt ISR
 *	¹¦ÄÜ£º 	 ¶¨Ê±Æ÷0ÖĞ¶ÏºÍ¶¨Ê±Æ÷2ÖĞ¶Ï
 *	ÊäÈë£º	 ÎŞ
 *	Êä³ö£º 	 ÎŞ
 --------------------------------------------------*/
void interrupt ISR(void)			//PIC_HI-TECHÊ¹ÓÃ
{ 
   
  //¶¨Ê±Æ÷0µÄÖĞ¶Ï´¦Àí**********************
	if(T0IE && T0IF)							//13us
	{
		TMR0 = 239;							//×¢Òâ:¶ÔTMR0ÖØĞÂ¸³ÖµTMR0ÔÚÁ½¸öÖÜÆÚÄÚ²»±ä»¯	 
		T0IF = 0;    
		IRSendIO = ~IRSendIO; 			//·­×ªµçÆ½  ²úÉú38KHzĞÅºÅ
	} 
    
    //¶¨Ê±Æ÷2µÄÖĞ¶Ï´¦Àí**********************
	if(TMR2IE && TMR2IF)			//560usÖĞ¶ÏÒ»´Î ºìÍâÃ¿Ò»Î»¶¼ÊÇ560uSµÄ±¶Êı
	{
		TMR2IF = 0;
        SendCtrl();
        SYSTime5S++;
	}
}
/*-------------------------------------------------
 *	º¯ÊıÃû: main 
 *	¹¦ÄÜ£º  Ö÷º¯Êı
 *	ÊäÈë£º  ÎŞ
 *	Êä³ö£º  ÎŞ
 --------------------------------------------------*/
void main()
{
	POWER_INITIAL();						//ÏµÍ³³õÊ¼»¯
    TIMER0_INITIAL();
    TIMER2_INITIAL();
	GIE = 1;										//¿ª×ÜÖĞ¶Ï
	while(1)
	{
		if(SYSTime5S >10000)				//¶¨Ê±5S
        {
        	SYSTime5S = 0;
            IRSendStatus = Status_Head;
        }
	}
}
