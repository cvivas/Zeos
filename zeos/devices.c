#include <io.h>

int sys_write_console(char *kbuffer,int i)
{
      int k=0;    
     while((k<i))
     {
       	printc(kbuffer[k]);//escriptura pel canal estandard
      	k++;
     }

    return k;
	
}
