/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

int write(int fd,char *buffer,int size);
int fork();
int getpid();
void exit () ;
int nice(int quantum);
int get_stats(int pid,int *tics);

int sem_init(int n_sem, unsigned int value);
int sem_signal(int n_sem);
int sem_destroy(int n_sem);
int sem_wait(int n_sem);
void perror();



#endif  /* __LIBC_H__ */
