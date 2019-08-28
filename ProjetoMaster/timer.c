//Arquivo Timer.c 
#include <msp430g2553.h>
#include "timer.h"
//Variaveis globais que podem ser usadas no main.c
char hora,minuto,segundo,milisegundo;    //declaradas em timer.c
unsigned int tempo_limite=30;            //declarada em timer.c
unsigned char cont_100ms=0;
/**************************** FUNCOES DO TIMER ********************************/
void ConfigTimer0(unsigned int intervalo,unsigned char divisor)
{
  TA0CTL=TA0CTL|TASSEL_2;
  switch(divisor)
  {
  case 1: TA0CTL=TA0CTL|ID_0;
          break;
  case 2: TA0CTL=TA0CTL|ID_1;
          break;
  case 4: TA0CTL=TA0CTL|ID_2;
          break;
  case 8: TA0CTL=TA0CTL|ID_3;
          break;          
  }
  TA0CCR0=intervalo-1;
  TA0CCTL0=CCIE;
}

void StartTimer0(void)
{
  TA0CTL=TA0CTL|MC_1;     //inicia contagem modo crescente
}

void StopTimer0(void)
{
  TA0CTL=TA0CTL&(~MC_3);  //zera o campo MC para parar o timer
}

/*#pragma vector=TIMER0_A0_VECTOR
__interrupt void timer (void)
{
  TA0CCTL0&=~CCIFG;
  //O que deve ser feito dentro da funcao pode mudar de programa para programa
  if(tempo_limite>0) tempo_limite--;    //decrementa timeout
  
  cont_100ms++;             //implementa o relogio
  if(cont_100ms==10)        //10x100ms
  {
    cont_100ms=0;
    segundo++;
    if(segundo==60)
    {
      segundo=0;
      minuto++;
      if(minuto==60)
      {
        minuto=0;
        hora++;
        if(hora==24) hora=0;
      }
    }
  }  
}*/
