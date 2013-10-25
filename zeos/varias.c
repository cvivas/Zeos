#include <varias.h>
#include <errno.h>
#include <mm_address.h>

void itoa (int num, char *buffer){

buffer[1]=num%10 + '0';
buffer[0] = num/10 + '0';


}

int comprueba_lec(int fd, int operacio){

if (operacio==ESCRITURA){

	if (fd!=1) return -EBADF;
}
else { /*LECTURA*/

      if (fd!=0) return -EBADF;
}

return 0;
}

int comprueba_punt(void *punt_buffer){

/*comprobamos que el puntero del buffer esta dentro del espacio de datos del usuario*/
if ( ((unsigned int)punt_buffer < L_DATA_USER_START) || ((unsigned int)punt_buffer > L_DATA_USER_START+NUM_PAG_DATA*PAGE_SIZE) ) return -EFAULT;
else return 0;
}








