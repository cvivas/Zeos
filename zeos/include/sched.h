/*
 * sched.h - Estructures i macros pel tractament de processos
 */
#include <list.h>
#ifndef __SCHED_H__
#define __SCHED_H__

#define NR_TASKS      10
#define KERNEL_STACK_SIZE	1024
#define INI_QUANTUMS 30
#define INT 0 /*si es 0, el task switch sabra si venimos de una interrupcion*/
#define NO_INT 1

#define NUM_SEMS 30

int numtasks;

struct task_struct {

 struct list_head lhtask;
 int pid;
 int quantum;
 int frame;	//&MEM FISICA
 int total_frames;
 int tics_totales;


};

//Semaforos:

struct sem_struct {

  struct list_head blocklist;
  int contador;
  int libre;

};



struct sem_struct sems[NUM_SEMS];

struct list_head runqueue;
struct list_head freebird;


union task_union {
  struct task_struct task;
  unsigned long stack[KERNEL_STACK_SIZE];    /* pila de sistema, per procés */
};

extern union task_union task[NR_TASKS];

/* Inicialitza les dades del proces inicial */
void init_task0(int ph_frame);
int hay_sitio_en_vp();
void mod_valor_ret_hijo(union task_union * t);
void init_task(int poslib, int frame_l);
struct task_struct* current();
struct task_struct * list_head_to_task_struct(struct list_head *l);
struct list_head * list_first( struct list_head *list );
int buscar_task(int pid_incognito);
void task_switch(union task_union *t, int es_int);
void roundrobin();

/*semaforos*/
void init_sem();
void desbloquear_proceso(struct list_head * l);
void bloquear_proceso(struct list_head * l);

#endif  /* __SCHED_H__ */
