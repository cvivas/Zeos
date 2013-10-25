#include <libc.h>
#define PERROR()	perror();
#define FINJP	while(1)
#define CERROR(x)	if (x<0){ PERROR();FINJP;}
#define PRINTF(x)	CERROR(write(1,x,strlen(x)))

int strlen( char *buffer ) {
    int i=0;
    for(;buffer[i];i++);
    return i;
}
