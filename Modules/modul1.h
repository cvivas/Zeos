#ifndef __MODUL1_H__
#define __MODUL1_H__

#define LLAMADAS_TOTALES 5

#define MY_OPEN 0
#define MY_WRITE 1
#define MY_LSEEK 2
#define MY_CLOSE 3
#define MY_CLONE 4

#define POS_SYSCALL_WRITE 4
#define POS_SYSCALL_OPEN 5
#define POS_SYSCALL_CLOSE 6
#define POS_SYSCALL_LSEEK 19
#define POS_SYSCALL_CLONE 120

struct estadisticas {
int num_ent;
int num_sal_ok;
int num_sal_err;
unsigned long long dur_tot;

};

struct mythread_info{
struct thread_info th;
int pid;
struct estadisticas est[LLAMADAS_TOTALES];
};

//int ( * sys_call_old[LLAMADAS_TOTALES])();
void * sys_call_old[LLAMADAS_TOTALES];

void init_myths(int pidx,struct mythread_info * curr_thinfos);
void desactivar(int arg);
void activar(int arg);

#endif
