#include <msp430g2553.h>
#include <stdlib.h>
#include "lcd.h"
#include "ad.h"
#include "timer.h"
#include "timerPWM.h"

#define A0 INCH_0 //canal de entrada
#define ANALOG BIT0
#define PARADO 0
#define MOV_ESQ 1
#define MOV_DIR 2
#define ATIRA 3
#define GAMEOVER 4
#define BUZZER_ON P1OUT|=BIT2
#define BUZZER_OFF P1OUT&=~BIT2

#define VELOC 20
#define GAMESPEED 7

typedef struct moveis{
  int posx;
  int posy;
  int vis;
}moveis;

unsigned int mover;
unsigned long int aux;
unsigned int tensao_mv;

unsigned char matriz [33][3];
unsigned int periodo;
int pontos = 0;
unsigned int vidas = 5;

void preencheMatriz(void);
void geraBala(int x, int y, moveis *b);
void moveBala(moveis *b);
void geraInimigo(int x, int y, moveis *e);
void moveInimigo(moveis *e);
void inicializa(int *es, int *x, int *y, moveis *b, moveis *e);
void buzz(unsigned long int tempo);
void gameOver();
int colide(moveis *b, moveis *e, int x, int y);

void movimenta(int dir, int *x, int *y);

void configAD();
void configTimer();
void LCDClear();

void main( void )
{
    
  WDTCTL = WDTPW + WDTHOLD;
  InitLCD();
  configAD();
  LCDCursorMode(0);
 
  periodo = 15;
  ConfigPWM(periodo);
  
  P1DIR |= BIT2;
  P1OUT |= BIT3;
  P1REN |= BIT3;
  
  P1OUT |= BIT5;
  P1REN |= BIT5; 
  P1IES |= BIT5;         //0 - Transição de subida, 1 - Transição de descida
                         //IES=tipo de interrupção a ser detectada
  P1IFG &= ~BIT5;        //Limpa qualquer pedido de interrupção pendente
  P1IE |= BIT5;          //Habilita interrupções no pino P1.5
  _BIS_SR(GIE); 
  
 // BCSCTL1 = CALBC1_8MHZ; // configura o clock
 // DCOCTL = CALDCO_8MHZ;
  
  int estado;// = PARADO;
  int posX;// = 2;
  int posY;// = 1;
  
  moveis bala[5];
  moveis inimigo[5];
  
  inicializa(&estado, &posX, &posY, bala, inimigo);
   
  int tempo = 0;
  int gameon = 1;
 // int velocidade = 0;
      
  for(;;){   
    switch (estado){
      case PARADO:
        if((P1IN&BIT3) == 0)
          estado = ATIRA;
          
        if(tempo >= GAMESPEED){
          geraInimigo(rand()%2+1,(rand()%2+1)*16, inimigo);
          tempo = 0;
        }
        
        mover = LeAD(A0);        
        if(mover < 457)
          estado = MOV_DIR;
        else if(mover > 473)
          estado = MOV_ESQ;
   
        moveBala(bala);
        gameon = colide(bala,inimigo, posX, posY);
        moveInimigo(inimigo);
  
        gameon = colide(bala,inimigo, posX, posY);
        if(gameon == 0 || vidas == 0)
          estado = GAMEOVER;
        break;
        
      case MOV_ESQ:
        movimenta(0, &posX, &posY);
        estado = PARADO;
        break;
        
      case MOV_DIR:
        movimenta(1, &posX, &posY);
        estado = PARADO;
        break;
        
      case ATIRA:
        geraBala(posX, posY, bala);       
        estado = PARADO;
        break;
        
      case GAMEOVER:
        gameOver();
        inicializa(&estado, &posX, &posY, bala, inimigo);
        break;        
    }
    AtualizaDCPWM(vidas*20);  
    tempo++;
    preencheMatriz();
  }
}

void inicializa(int *es, int *x, int *y, moveis *b, moveis *e){
  _BIC_SR(GIE);
  LCDClear();
  LCDPrintXYStr(1,2,"JOGO COMECANDO!");
  buzz(50000);
  __delay_cycles(50000);
  buzz(50000);
  __delay_cycles(50000);
  buzz(2000000);
  __delay_cycles(500000);
  *es = PARADO;
  *x = 2;
  *y = 1;
  
  int i, j;
  for(i = 0; i < 33; i++)
    for(j = 0; j < 3; j++)
      matriz[i][j] = ' ';    
  
  for(i = 0; i < 5; i++){
    e[i].vis = 0;
    b[i].vis = 0;
  }   
  
  matriz[*y][*x] = '>';//posiciona o player inicialmente
  _BIS_SR(GIE);
}

void movimenta(int dir, int *x, int *y){
  matriz[*y][*x] = ' ';
  
  if(dir == 0){
    if(*x == 2 && *y == 1)
        *x = 1;
    else if(*y == 17){
      if(*x == 1){
        *y = 1;        
        *x = 2;
      }
      else
        *x = 1;  
    }
  }
  else{
    if(*x == 1 && *y == 17)
      *x = 2;
    else if(*y == 1){
      if(*x == 2){
        *y = 17;        
        *x = 1;
      }
      else
        *x = 2;   
    }
  }
  matriz[*y][*x] = '>';
}

void geraBala(int x, int y, moveis *b){
  int i;
  for(i = 0; i < 5; i++){
    if(b[i].vis == 0){
      b[i].posx = x;
      b[i].posy = y+1;
      b[i].vis = 1;
      break;
    }
  }  
  matriz[b[i].posy][b[i].posx] = '-';
}

void moveBala(moveis *b){
  int i;
  for(i = 0; i < 5; i++){
    if(b[i].vis == 1){
      matriz[b[i].posy][b[i].posx] = ' ';
      b[i].posy++;
      
      if(b[i].posy == 17 ||  b[i].posy == 33)
        b[i].vis = 0;
      else
        matriz[b[i].posy][b[i].posx] = '-';
    }    
  }
}

void geraInimigo(int x, int y, moveis *e){
  int i;
  for(i = 0; i < 5; i++){
    if(e[i].vis == 0){
      e[i].posx = x;
      e[i].posy = y;
      e[i].vis = 1;
      break;
    }
  }  
  matriz[e[i].posy][e[i].posx] = '<';
}

void moveInimigo(moveis *e){
  int i;
  for(i = 0; i < 5; i++){
    if(e[i].vis == 1){
      matriz[e[i].posy][e[i].posx] = ' ';
      e[i].posy--;
      
      if(e[i].posy == 16 ||  e[i].posy == 0){
        vidas--;
        if(pontos < 0)
            pontos = 0;
        e[i].vis = 0;
      }
      else
        matriz[e[i].posy][e[i].posx] = '<';
    }    
  }
}

int colide(moveis *b, moveis *e, int x, int y){
  int i,j;
  int gameon = 1;
  for(i = 0; i < 5; i++){
    for(j = 0; j < 5; j++){
      if(b[i].posy == e[j].posy && b[i].posx == e[j].posx && b[i].vis == 1 && e[j].vis == 1){
        buzz(500);
        pontos++;
        b[i].vis = 0;
        e[j].vis = 0;
        matriz[e[j].posy][e[j].posx] = '*';
      }
    }
    if(e[i].posx == x && e[i].posy == y && e[i].vis == 1){      
      e[i].vis = 0;
      matriz[e[i].posy][e[i].posx] = '+';
      //preencheMatriz();
      //gameOver
      gameon = 0;
      break;
    }
  }   
  return gameon;
}

void gameOver(){
  _BIC_SR(GIE);
  buzz(1000000);
  LCDClear();
  LCDPrintXYStr(4,1,"GAME OVER");
  LCDCursorPos(3,2);
  LCDPrintStr("PONTUACAO: ");
  LCDPrintVal(pontos);
  LCDPrintXYStr(17,1,"PRESSIONE  ATIRA");
  LCDPrintXYStr(18,2,"PARA RECOMECAR");
  while(1){
    if((P1IN&BIT3) == 0)
      break;
  }
  pontos = 0;
  vidas = 5;
  _BIS_SR(GIE);
}

void configAD() {
  ConfigAD(ANALOG);//foi o du que fez
}

void preencheMatriz(void){
  _BIC_SR(GIE);
  int i, j;
  for(i = 1; i <= 32; i++){
    for(j = 1; j <= 2; j++){
      LCDCursorPos(i,j);
      LCDChar(matriz[i][j]);
    }
  }
  _BIS_SR(GIE);
}

void LCDClear(){
  _BIC_SR(GIE);
  int i, j;
  for(i = 1; i <= 32; i++){
    for(j = 1; j <= 2; j++){
      LCDCursorPos(i,j);
      LCDChar(' ');
    }
  }
  _BIS_SR(GIE);
}

void buzz(unsigned long int tempo){
  _BIC_SR(GIE);
  int i;
  BUZZER_ON;
  for(i = 0; i < tempo; i++)
    __delay_cycles(1);
  BUZZER_OFF;  
  _BIS_SR(GIE);
}

#pragma vector=PORT1_VECTOR
__interrupt void pause (void){  
  __delay_cycles(200000); //Evitar o efeito boucing
  _BIC_SR(GIE);
  LCDPrintXYStr(8, 2, "-");
  LCDPrintXYStr(24, 1, "-");
  __delay_cycles(20000);
  while(1)
    if((P1IN&BIT5) == 0)
      break;  
  __delay_cycles(200000);//Boucing tmb
  InitLCD();
  P1IFG &= ~BIT5; //Limpa pedido de int. se houver antes de retornar a dormir
  _BIS_SR(GIE); //Habilita a int. global
}