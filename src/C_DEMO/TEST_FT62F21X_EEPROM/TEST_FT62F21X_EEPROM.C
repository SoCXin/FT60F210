//********************************************************* 
/*	ÎÄ¼þÃû£ºTEST_FT62F21x_EEPROM.c
*	¹¦ÄÜ£º	 FT62F21x-EEPROM¹¦ÄÜÑÝÊ¾
*	IC:		 FT62F211 SOP8
*	¾§Õñ£º   16M/4T                    
*	ËµÃ÷£º   ¸Ã³ÌÐò¶ÁÈ¡0x12µØÖ·µÄÖµ,È¡·´ºó´æÈë0x13µØÖ·
*
*                  FT62F211 SOP8 
*                 ----------------
*  NC-----------|1(PA4)      (PA3)8|------------NC    
*  NC-----------|2(TKCAP)  (PA0)7|------------NC
*  VDD----------|3(VDD)     (PA1)6|------------NC
*  GND----------|4(VSS)     (PA2)5|------------NC
*			      ----------------
*/
//*********************************************************
#include	"SYSCFG.h";
//#include 	"FT62F21X.h";
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
//***********************ºê¶¨Òå*****************************
#define  unchar     unsigned char 
#define  unint      	unsigned int
#define  unlong     unsigned long  
 
unchar EEReadData;
 /*-------------------------------------------------
 *	º¯ÊýÃû£ºPOWER_INITIAL
 *	¹¦ÄÜ£º  ÉÏµçÏµÍ³³õÊ¼»¯
 *	ÊäÈë£º  ÎÞ
 *	Êä³ö£º  ÎÞ
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{
	OSCCON = WDT_32K|OSC_16M|0X00;	
  //OSCCON = 0B01110000;		//WDT 32KHZ IRCF=111=16MHZ/4=4MHZ,0.25US/T

	INTCON = 0;  							//ÔÝ½ûÖ¹ËùÓÐÖÐ¶Ï

	PORTA = 0B00000000;				
	TRISA = 0B00000000;				//PAÊäÈëÊä³ö 0-Êä³ö 1-ÊäÈë  
	WPUA = 0B00000000;     		//PA¶Ë¿ÚÉÏÀ­¿ØÖÆ 1-¿ªÉÏÀ­ 0-¹ØÉÏÀ­

	OPTION = 0B00001000;			//Bit3=1 WDT MODE,PS=000=1:1 WDT RATEÏ
													//Bit7(PAPU)=0 ÓÉWPUA¾ö¶¨ÊÇ·ñÉÏÀ­
	MSCON = 0B00000000;		                             
}
/*-------------------------------------------------
 *	º¯ÊýÃû£ºEEPROMread
 *	¹¦ÄÜ£º  ¶ÁEEPROMÊý¾Ý
 *	ÊäÈë£º  ÐèÒª¶ÁÈ¡Êý¾ÝµÄµØÖ· EEAddr
 *	Êä³ö£º  ¶ÔÓ¦µØÖ·¶Á³öµÄÊý¾Ý ReEEPROMread
 --------------------------------------------------*/
unchar EEPROMread(unchar EEAddr)
{
	unchar ReEEPROMread;
    EEADR = EEAddr;
    RD=1;
    NOP();
    NOP();
    NOP();
    NOP();
    ReEEPROMread =EEDAT;
    return ReEEPROMread;
}
/*-------------------------------------------------
 *	º¯ÊýÃû£ºEEPROMwrite
 *	¹¦ÄÜ£º  Ð´Êý¾Ýµ½EEPROM
 *	ÊäÈë£º  ÐèÒª¶ÁÈ¡Êý¾ÝµÄµØÖ· EEAddr
				ÐèÒªÐ´ÈëµÄÊý¾Ý		  Data
 *	Êä³ö£º  ÎÞ
 --------------------------------------------------*/
void EEPROMwrite(unchar EEAddr,unchar Data)
{
	
	while(GIE)					//µÈ´ýGIEÎª0
		{GIE = 0;	}						//Ð´Êý¾Ý±ØÐë¹Ø±ÕÖÐ¶Ï
	EEADR = EEAddr; 	 			//EEPROMµÄµØÖ·
	EEDAT = Data;		 			//EEPROMµÄÐ´Êý¾Ý  EEDATA = Data;
	EEIF = 0;
	EECON1 |= 0x34;				//ÖÃÎ»WREN1,WREN2,WREN3Èý¸ö±äÁ¿.
	WR = 1;							//ÖÃÎ»WRÆô¶¯±à³
    NOP();
    NOP();
    NOP();
    NOP();
	while(WR);      				//µÈ´ýEEÐ´ÈëÍê³É
	GIE = 1;
}
/*-------------------------------------------------
 *	º¯ÊýÃû: main 
 *	¹¦ÄÜ£º  Ö÷º¯Êý
 *	ÊäÈë£º  ÎÞ
 *	Êä³ö£º  ÎÞ
 --------------------------------------------------*/
void main()
{
	POWER_INITIAL();								//ÏµÍ³³õÊ¼»¯
 	EEPROMwrite(0x55,0xaa);	
	EEPROMwrite(0x55,0xaa);					//ÔÚÎ´Ê¹ÓÃµ½µÄËæÒâÒ»¸öµØÖ·Ð´Á½´Î0xAA
 
	EEReadData = EEPROMread(0x12); 		//¶ÁÈ¡0x12µØÖ·EEPROMÖµ 
	EEPROMwrite(0x13,~EEReadData); 		//È¡·´Ð´ÈëµØÖ·0x13
    
	while(1) 
	{
 
		NOP();
	 
	}
}
