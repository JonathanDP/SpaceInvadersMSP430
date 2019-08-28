/***************** TIMERA0 OPERANDO COMO PWM  *******************************/
#include <msp430g2553.h>
#include "timerPWM.h"
/****************************** FUNCOES DO PWM *******************************/
void ConfigPWM (unsigned int t)
{
  P1SEL=P1SEL|BIT6;             //Configurando pino P1.6 para modo saida PWM
  P1DIR=P1DIR|BIT6;
  TA0CCTL1=OUTMOD_7;            //Configurando modo RESET/SET do PWM
  TA0CCR0=t-1;                  //Configura periodo do PWM
  TA0CCR1=0;                    //Valor inicial do Ton (Th) = 0
  TA0CTL=TASSEL_2+ID_0+MC_1;    //Clock=SMCLK=1MHZ, Sem divisao, ligando o timer 
}

void AtualizaTonPWM (unsigned int valor)
{
  if(valor<=(TA0CCR0+1)) TA0CCR1=valor;
}

void AtualizaDCPWM (unsigned int dc)
{
  unsigned long int aux;
  if(dc<=100)
  {
    aux=(unsigned long)dc;
    aux=aux*(TA0CCR0+1);
    TA0CCR1=(unsigned int) (aux/100);
  }
}
/*****************************************************************************/
