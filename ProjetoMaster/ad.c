#include "io430.h"
#include "ad.h"

/****************************** FUNCOES DO AD ********************************/
/* Configura o canal AD
  - Entradas utilizadas na funcao analógica
  - Clock do conversor = SMCLK
  - Sem parametros ou retornos
*/
void ConfigAD (unsigned int entradas)
{
  ADC10AE0=entradas;     
  ADC10CTL1=ADC10SSEL_3;       //Clock utilizado = SMCLK
}

/* Realiza a leitura do canal AD utilizando media
  - Modo de leitura do AD = canal simples, conversao unica
  - Parametro entrada = canal a ser lido (INCH_x x=0,1,2,3...,7)
  - Parametro saida = valor lido em 10 bits (0 a 1023)
  - Adotado metodo de media para obtencao de um valor final
*/
#define AMOSTRAS 10
unsigned int LeAD (unsigned int canal)
{
  unsigned int valor,amostras;
  unsigned long int soma=0;
  //Ajusta o canal a ser lido
  ADC10CTL0=ADC10CTL0&~(ADC10ON);       //desliga o ad para ajustar o canal
  ADC10CTL0=ADC10CTL0&~(ENC+ADC10SC);   
  ADC10CTL1=ADC10CTL1&~(INCH_7);        //limpa a configuracao do canal
  ADC10CTL1=ADC10CTL1|canal;            //ajusta o novo canal
  //100 leituras do ADC10 equivalem a um periodo de 20ms, aproximadamente
  ADC10CTL0=ADC10CTL0|ADC10ON;          //liga o conversor
  for(amostras=0;amostras<AMOSTRAS;amostras++)
  {
    ADC10CTL0 = ADC10CTL0 | (ENC + ADC10SC);    //dispara a conversao
    while ((ADC10CTL0&ADC10IFG)==0);            //espera converter (ADC10IFG=1)
    soma=soma+ADC10MEM;                         //acumula a leitura feita
  }
  //retira a media
  soma=soma/AMOSTRAS;
  valor=(unsigned int) soma;
  return (valor);
}
/*****************************************************************************/
