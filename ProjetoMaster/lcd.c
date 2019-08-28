/********************************************************************/
/*  FUNCOES CONTROLE DE DISPLAY LCD 16X2                            */
/* DEVICE:MSP430G2553 (TEXAS INSTRUMENTS)                           */
/*                                                                  */
/* INTERFACE: IAR EMBEDDED WORKBENCH IDE FOR MSP430 V.6             */
/*                                                                  */
/* HARDWARE :                                             	    */
/* LCD_D4 = P2.0                                                    */
/* LCD_D5 = P2.1                                                    */
/* LCD_D6 = P2.2                                                    */
/* LCD_D7 = P2.3                                                    */
/* LCD_RS = P2.4                                                    */
/* LCD_EN = P2.5                                                    */
/********************************************************************/

#include "io430.h" 
#include "lcd.h"

//display LCD
#define LCD_D0  BIT0		//D0 = D4
#define LCD_D1  BIT1		//D1 = D5
#define LCD_D2  BIT2		//D2 = D6
#define LCD_D3  BIT3		//D3 = D7
#define LCD_RS  BIT4
#define LCD_EN  BIT5
#define LCD_EN_ON       P2OUT=P2OUT|LCD_EN
#define LCD_EN_OFF      P2OUT=P2OUT&(~LCD_EN)
#define LCD_RS_ON       P2OUT=P2OUT|LCD_RS
#define LCD_RS_OFF      P2OUT=P2OUT&(~LCD_RS)
#define LCD_D0_ON       P2OUT=P2OUT|LCD_D0
#define LCD_D0_OFF      P2OUT=P2OUT&(~LCD_D0)
#define LCD_D1_ON       P2OUT=P2OUT|LCD_D1
#define LCD_D1_OFF      P2OUT=P2OUT&(~LCD_D1)
#define LCD_D2_ON       P2OUT=P2OUT|LCD_D2
#define LCD_D2_OFF      P2OUT=P2OUT&(~LCD_D2)
#define LCD_D3_ON       P2OUT=P2OUT|LCD_D3
#define LCD_D3_OFF      P2OUT=P2OUT&(~LCD_D3)
#define LCD_DATA_OFF    P2OUT=P2OUT&(~(LCD_D0+LCD_D1+LCD_D2+LCD_D3))
#define LCD_DATA_DIR    P2DIR=P2DIR|(LCD_D0+LCD_D1+LCD_D2+LCD_D3+LCD_EN+LCD_RS)

/********************************************************************/

/*******************************FUNCOES******************************/
/****************ENVIA UM COMANDO PARA O DISPLAY*********************/
void LCDCmd (unsigned char cmd)
{
  LCD_RS_OFF;
  sendnibble(cmd>>4);           //uso de 4 bits apenas
  sendnibble(cmd&0x0F);         //escreve 4 bits
}
/********************************************************************/
/***********ROTINA PARA TRABALHAR COM 4 BITS NO DISPLAY**************/
void sendnibble(unsigned char dado)
{
  LCD_DATA_OFF;
  if ((dado&0x01)==0x01) LCD_D0_ON;   //atualiza valor de dado
  if ((dado&0x02)==0x02) LCD_D1_ON;   //atualiza valor de dado
  if ((dado&0x04)==0x04) LCD_D2_ON;   //atualiza valor de dado
  if ((dado&0x08)==0x08) LCD_D3_ON;   //atualiza valor de dado
  LCD_EN_ON;
  delayLCD();
  LCD_EN_OFF;
}
/********************************************************************/
/******************INICIALIZA O DISPLAY******************************/
void InitLCD (void)
{
  LCD_DATA_DIR;
  LCD_EN_OFF;
  LCD_RS_OFF;
  sendnibble(0x30>>4);
  delayLCD();
  sendnibble(0x30>>4);
  delayLCD();
  sendnibble(0x30>>4);
  delayLCD();
  sendnibble(0x20>>4);
  delayLCD();
  LCDCmd(0x28);
  delayLCD();
  LCDCmd(0x08);
  delayLCD();
  LCDCmd(0x0C);
  delayLCD();
  LCDCmd(0x01);
  delayLCD();
}
/********************************************************************/
/*******************ATRASO PARA CONFIGURAR O DISPLAY*****************/
void delayLCD (void)
{
  unsigned char i;
  for (i=0;i<200;i++)
  {
    i=i;
  }
  return;
}
/********************************************************************/
/********ENVIA UMA MENSAGEM PARA O DISPLAY NA POSICAO X,Y************/
void LCDPrintXYStr (unsigned char x, unsigned char y,unsigned char *dado)
{
  unsigned char pos;
  pos=x-1;
  if (y==1) 
  {
    pos=pos+0x80;
    LCDCmd(pos);
  }
  else if(y==2)  
  {
    pos=pos+0xc0;
    LCDCmd(pos);
  }
  LCDPrintStr(dado);              
}
/********************************************************************/
/*******************ENVIA UMA MENSAGEM AO DISPLAY********************/
void LCDPrintStr (unsigned char *dado)
{
  while (*dado != 0)  
  {
    LCDChar(*dado);
    dado++;
  }
}
/********************************************************************/
/*****************ENVIA UM INTEIRO PARA O DISPLAY********************/
void LCDPrintVal (unsigned int dado)
{
  if(dado>=10000) LCDChar((dado/10000)+0x30);
  if(dado>=1000) LCDChar(((dado%10000)/1000)+0x30);
  if(dado>=100)  LCDChar((((dado%10000)%1000)/100)+0x30);
  if(dado>=10)   LCDChar(((((dado%10000)%1000)%100)/10)+0x30);
  LCDChar(((((dado%10000)%1000)%100)%10)+0x30);
}
/********************************************************************/
/*********ENVIA UM INTEIRO PARA O DISPLAY NAS POSICOES X e Y*********/
void LCDPrintXYVal (unsigned char x,unsigned char y,unsigned int dado)
{
  unsigned char pos;
  pos=x-1;
  if (y==1) 
  {
    pos=pos+0x80;
    LCDCmd(pos);
  }
  else      
  {
    pos=pos+0xc0;
    LCDCmd(pos);
  }
  LCDPrintVal(dado);
}

/**********************ENVIA UM DADO AO DISPLAY**********************/
void LCDChar (unsigned char dado)
{
  LCD_RS_ON;
  sendnibble(dado>>4);
  sendnibble(dado&0x0F);
}
/********************************************************************/
/**********************ENVIA UM COMANDO AO DISPLAY**********************/
void LCD (unsigned char dado)
{
  LCD_RS_ON;
  sendnibble(dado>>4);
  sendnibble(dado&0x0F);
}
/********************************************************************/
/**********************ESCOLHE MODO CURSOR **********************/
void LCDCursorMode (unsigned char modo)
{
  if(modo==0) LCDCmd(0x0c);   //apagado
  else if(modo==1) LCDCmd(0x0F);     //ligado
}
/********************************************************************/
/**********************ESCOLHE POSICAO CURSOR **********************/
void LCDCursorPos (unsigned char x,unsigned char y)
{
  unsigned char pos;
  pos=x-1;
  if (y==1) 
  {
    pos=pos+0x80;
    LCDCmd(pos);
  }
  else      
  {
    pos=pos+0xc0;
    LCDCmd(pos);
  }  
}
/********************************************************************/