/*
 * libc.c 
 */

#include <libc.h>
#include <errno.h>

int errno;
/* Wrapper of  write system call*/
int write(int fd,char *buffer,int size)
{

  int res;

  __asm__ __volatile__(
		       "movl 8(%%ebp), %%ebx\n"
		       "movl 12(%%ebp),%%ecx\n"
		       "movl 16(%%ebp),%%edx\n"
		       "movl $4, %%eax\n"
		       "int $0x80\n"
		       "movl %%eax,%0\n"
		       :"=g" (res) 
		       :
		       :"bx","cx","dx","memory"
		       );

  if(res<0)//hi ha error, actualitzem errno
    {
      errno=res;/*valor absolut de res*/
      return -1;
    }
  else return res;

}
int fork(){
  int res;

  __asm__ __volatile__(
		       "movl $2, %%eax\n"
		       "int $0x80\n"
		       "movl %%eax,%0\n"
		       :"=g" (res) 
		       :
		       :"ax", "memory"
		       );

  if(res<0)//hi ha error, actualitzem errno
    {
      errno=res;/*valor absolut de res*/
      return -1;
    }
  else return res;
}

int getpid(){
  int res;

  __asm__ __volatile__(
		       "movl $20, %%eax\n"
		       "int $0x80\n"
		       "movl %%eax,%0\n"
		       :"=g" (res) 
		       :
		       :"ax", "memory"
		       );

  if(res<0)//hi ha error, actualitzem errno
    {
      errno=res;/*valor absolut de res*/
      return -1;
    }
  else return res;
}

void exit () {
  int res;

  __asm__ __volatile__(
		       "movl $1, %%eax\n"
		       "int $0x80\n"
		       "movl %%eax,%0\n"
		       :"=g" (res) 
		       :
		       :"ax", "memory"
		       );

  if(res<0)//hi ha error, actualitzem errno
    {
      errno=res;/*valor absolut de res*/
    }
}

int nice(int quantum){
  int res;

  __asm__ __volatile__(
		       "movl 8(%%ebp), %%ebx\n"
		       "movl $34, %%eax\n"
		       "int $0x80\n"
		       "movl %%eax,%0\n"
		       :"=g" (res) 
		       :
		       :"ax","bx", "memory"
		       );

  if(res<0)//hi ha error, actualitzem errno
    {
      errno=res;/*valor absolut de res*/
      return -1;
    }
  else return res;
}

int get_stats(int pid,int *tics){
  int res;

  __asm__ __volatile__(
		       "movl 8(%%ebp), %%ebx\n"
		       "movl 12(%%ebp),%%ecx\n"
		       "movl $35, %%eax\n"
		       "int $0x80\n"
		       "movl %%eax,%0\n"
		       :"=g" (res) 
		       :
		       :"ax","bx","cx", "memory"
		       );

  if(res<0)//hi ha error, actualitzem errno
    {
      errno=res;/*valor absolut de res*/
      return -1;
    }
  else return res;
}

int sem_init(int n_sem, unsigned int value)
{
  int res;

  __asm__ __volatile__(
  		       "movl 8(%%ebp), %%ebx\n"
  		       "movl 12(%%ebp), %%ecx\n"
		       "movl $21, %%eax\n"
		       "int $0x80\n"
		       "movl %%eax,%0\n"
		       :"=g" (res) 
		       :
		       :"ax","bx","cx","memory"
		       );

  if(res<0)//hi ha error, actualitzem errno
    {
      errno=res;
      return -1;
    }
  else return res;
}

int sem_wait(int n_sem)
{
  int res;

  __asm__ __volatile__(
  		       "movl 8(%%ebp), %%ebx\n"
		       "movl $22, %%eax\n"
		       "int $0x80\n"
		       "movl %%eax,%0\n"
		       :"=g" (res) 
		       :
		       :"ax","bx","memory"
		       );

  if(res<0)//hi ha error, actualitzem errno
    {
      errno=res;
      return -1;
    }
  else return res;
}

int sem_signal(int n_sem)
{
  int res;

  __asm__ __volatile__(
  		       "movl 8(%%ebp), %%ebx\n"
		       "movl $23, %%eax\n"
		       "int $0x80\n"
		       "movl %%eax,%0\n"
		       :"=g" (res) 
		       :
		       :"ax","bx","memory"
		       );

  if(res<0)//hi ha error, actualitzem errno
    {
      errno=res;
      return -1;
    }
  else return res;
}

int sem_destroy(int n_sem)
{
  int res;

  __asm__ __volatile__(
  		       "movl 8(%%ebp), %%ebx\n"
		       "movl $24, %%eax\n"
		       "int $0x80\n"
		       "movl %%eax,%0\n"
		       :"=g" (res) 
		       :
		       :"ax","bx","memory"
		       );

  if(res<0)//hi ha error, actualitzem errno
    {
      errno=res;
      return -1;
    }
  else return res;
}


void perror()
{

  switch(-errno)
    {
    case 0: break;
    case EPERM:write(1,"Operation not permited", 22); break;
    case ESRCH:write(1,"No such process", 15); break;
    case EAGAIN:write(1,"Try again", 10); break;
    case EBADF:write(1,"Bad file number", 15);break;
    case EFAULT:write(1,"Bad address", 11);break;
    case EFBIG:write(1,"File too large", 14);break;
    case EINTR:write(1,"Interrupted system call", 25);break;
    case EINVAL:write(1,"Invalid argument", 16);break;
    case EIO:write(1,"I/O error", 9);break;
    case ENOSPC:write(1,"No space left on device", 23);break;
    case EPIPE:write(1,"Broken pipe", 11);break;
    case ENOSYS:write(1,"Function not implemented",24);break;	
    case EACCES:write(1,"Permission denied",17);break;
    case ENOENT:write(1,"No such file or directory",25);break;
    case ENFILE:write(1,"File table overflow",19);break;
    case EMFILE:write(1,"Too many open files",19);break;
    case ENOMEM:write(1,"Out of memory",13);break;
    default: write(1,"Codi desconegut",16);break;
    }
}
