//Declaracao das funcoes do LCD

/*****************PROTÓTIPO DAS FUNÇÕES******************************/
void InitLCD(void);
void LCDChar(unsigned char dado);
void LCDPrintStr(unsigned char *dado);
void LCDPrintXYStr(unsigned char x, unsigned char y,unsigned char *dado);
void LCDCmd(unsigned char cmd);
void LCDPrintVal(unsigned int dado);
void LCDPrintXYVal(unsigned char x, unsigned char y,unsigned int dado);
void LCDCursorMode(unsigned char modo);
void LCDCursorPos(unsigned char x,unsigned char y);
void delayLCD(void);
void sendnibble(unsigned char dado);
/********************************************************************/