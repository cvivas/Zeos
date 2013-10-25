#include <utils.h>
#include <types.h>

void copy_data(void *start, void *dest, int size)
{
  DWord *p = start, *q = dest;
  while(size > 0) {
    *q++ = *p++;
    size -= 4;
  }
}

/* Copia de espacio de usuario a espacio de kernel, devuelve 0 si ok y -1 si error*/
int copy_from_user(void *start, void *dest, int size)
{
  DWord *p = start, *q = dest;
  while(size > 0) {
    *q++ = *p++;
    size -= 4;
  }
  return 0;
}

/* Copia de espacio de kernel a espacio de usuario, devuelve 0 si ok y -1 si error*/
int copy_to_user(void *start, void *dest, int size)
{
  DWord *p = start, *q = dest;
  while(size > 0) {
    *q++ = *p++;
    size -= 4;
  }
  return 0;
}

