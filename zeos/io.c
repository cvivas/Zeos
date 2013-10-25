/*
 * io.c - 
 */

#include <io.h>

/**************/
/** Screen  ***/
/**************/

#define NUM_COLUMNS 80
#define NUM_ROWS    25

Byte x, y=15,scro=0,ara=0;

/* Read a byte from 'port' */
Byte inb (unsigned short port)
{
  Byte v;

  __asm__ __volatile__ ("inb %w1,%0":"=a" (v):"Nd" (port));
  return v;
}
//Y altura
void printc(char c)
{

  if (c =='\n'){
	x = 0;
	if (y==(NUM_ROWS-1)) scroll();
	//if (y>NUM_ROWS) y=0;
	if (y<NUM_ROWS-1) y++;
	//else y++;
	}
	
  else { 

  Word ch = (Word) (c & 0x00FF) | 0x0200;
  DWord screen = 0xb8000 + (y * NUM_COLUMNS + x) * 2;
  if (++x >= NUM_COLUMNS)
  {
    x = 0;
    ara=1;	


	if (y==NUM_ROWS) {scroll();scro=1;ara=0;y=NUM_ROWS-1;}
	if (y<NUM_ROWS && scro==0) y++;
   // if (++y >= NUM_ROWS) y = 0;
  }

  asm("movw %0, (%1)" : : "g"(ch), "g"(screen));
    if (scro==1 && ara==1) {scroll();ara=0;} 
}
}

void printk(char *string)
{
  int i;
  for (i = 0; string[i]; i++)
    printc(string[i]);
}

void printc_xy(int x, int y, char c){
  Word ch = (Word) (c & 0x00FF) | 0x0200;
  DWord screen = 0xb8000 + (y * NUM_COLUMNS + x) * 2;
	  asm("movw %0, (%1)" : : "g"(ch), "g"(screen));
}

void printk_xy(int x, int y, char *string)
{
  int i;
  for (i=0;string[i];i++)
    {
      printc_xy(x,y,string[i]);
      x++;
    }
}

void scroll (){
int x1,y1;
DWord screen,screen2,dch;

for (y1=1;y1<=NUM_ROWS;y1++)
{
	for (x1=0;x1<NUM_COLUMNS;x1++){
	if (y1==NUM_ROWS){
	
	dch = (Word) (' ' & 0x00FF) | 0x0200;
	}

	else{
	screen = 0xb8000+ (y1*NUM_COLUMNS +x1) * 2; 

	asm("movl (%1), %0":"=g"(dch) :  "g"(screen));
	}
	 screen2 = 0xb8000+ ((y1-1)*NUM_COLUMNS +x1) * 2;
	//dch = (screen & 0x00FF) | 0x0200;
	asm("movl %0, (%1)" : : "g"(dch), "g"(screen2));
	
	}
}

}
