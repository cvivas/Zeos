/*
 * sched.c - initializes struct for task 0
 */

#include <sched.h>
#include <mm_address.h>
#include <types.h>
#include <varias.h>
#include <mm.h>
#include <list.h>


int cuantspid;

int quantums_restantes;


struct list_head pos_libres;

//struct sem_struct sems[NUM_SEMS];


union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

struct task_struct* current(){	//&inicio struct actual

struct task_struct *tas_st;
  __asm__ __volatile__(
			"movl %%esp,%%eax\n"
			"andl $0xFFFFF000, %%eax\n"
			"movl %%eax, %0\n"
			:"=g" (tas_st)
			:
			: "memory"
			);
return tas_st;
}

struct task_struct * list_head_to_task_struct(struct list_head *l)
{
  /*struct task_struct *ts;
  struct task_struct t;
  struct list_head li;
	ts=list_entry(l,t, li);*/
return (struct task_struct *)l;
}

void init_task0(int first_ph)
{
cuantspid = 0;
task[0].task.pid = 0 ;
cuantspid++;
numtasks=1;
task[0].task.quantum=30;
task[0].task.frame=first_ph+8;
task[0].task.tics_totales=0;
  quantums_restantes = task[0].task.quantum;
/*INICIAR LISTa posiciones libres.*/
//LIST_HEAD_INIT(runqueue)
INIT_LIST_HEAD(&runqueue);
list_add(&task[0].task.lhtask,&runqueue);
//list_add(&task[0].task.lhtask,&task[0].task.lhtask);
//__list_add(&runqueue, &task[0].task.lhtask, &task[0].task.lhtask);

init_freebird();

}

void init_freebird(){
int i;
INIT_LIST_HEAD(&freebird);
//for(i=9;i>0;i--) list_add(&task[i].task.lhtask,&freebird);
for(i=1;i<10;i++) list_add_tail(&task[i].task.lhtask,&freebird);
}

void init_task(int poslib, int frame_l)
{
task[poslib].task.pid = cuantspid ;
cuantspid++;
numtasks++;
task[poslib].task.quantum=INI_QUANTUMS;
task[poslib].task.frame=frame_l;
task[poslib].task.tics_totales=0;

}
void init_taskf(union task_union *t, int frame_l)
{
t->task.pid = cuantspid ;
cuantspid++;
numtasks++;
t->task.quantum=INI_QUANTUMS;
t->task.frame=frame_l;
t->task.tics_totales=0;

}

/*devuelve la posicon del vector de procesos donde podemos insertar al hijo*/
int hay_sitio_en_vp()
{
  int i=1;
  int trobat=Falso;

  while ( (i< NR_TASKS)&&(trobat==Falso) )
  {
	if (task[i].task.quantum==0) trobat=Cierto;
	else i++;
  }

  if (trobat==Falso) return -1;
  else return i;

}

void mod_valor_ret_hijo(union task_union * t)
{
/*utilizamos el registro ebx para poner en el eax del hijo el valor 0 en su pila
  este registro esta en 4096-(4*10)=4056 + @ t
 */
//hijo->stack[kernelsize-10]=0;

   __asm__ __volatile__(
			"movl %0, %%ebx\n"
			"movl $0, 4056(%%ebx)\n"
			:
			:"g" (t)
			:"bx"
			);
}

//actualiza el orden de la runqueue. ll es un puntero al proceso que deja de estar en running
void actualitzar_runqueue(struct list_head *ll)
{
     list_del(ll);
     list_add_tail(ll, &runqueue); 
}

void task_switch(union task_union *t, int es_int){

/*printk("...pid del hijo:");
 int sss=t->task.pid;
	sss+='0';
  printc(sss);*/

	DWord esp;
	DWord esp0 = (DWord)&t->stack[KERNEL_STACK_SIZE];
struct task_struct *ts = list_head_to_task_struct(list_first(&runqueue));
    // struct task_struct *ts = current();
	

   //cambiamos el orden de la runqueue si venimos de una interrupcion.
    if( es_int == INT )  actualitzar_runqueue(&ts->lhtask); 

  
   //la tss apuntara a la pila de sistema del nuevo proceso
     actualitzar_TSS(esp0); 

   //las paginas logicas de usuario apuntaran a las del nuevo proceso
	actualitzar_TP(t->task.frame);  
  
	esp = (DWord) &t->stack[KERNEL_STACK_SIZE - 16 ]; 

   //cambiamos a la pila de sistema del nuevo proceso
      __asm__ __volatile__(
			"movl %0, %%esp\n"
			:
			:"g" (esp)
		     );

	if (es_int == INT) 
	{	      
			__asm__ __volatile__(
   	                "movb $0x20,%al\n" 
      			 "outb %al,$0x20\n"
				);
	}

	//quantums_restantes = ts->quantum; //la hacemos en el RR

 // struct task_struct * p_actual = current();/*punter al inicio del proceso actual*/



	/*printk("\n pid task que entrara a run:");

	printc(p_actual->pid+'0');*/

      __asm__ __volatile__(
			      "popl %ebx\n"
      		       	      "popl %ecx\n"
			      "popl %edx\n"
			      "popl %esi\n"
			      "popl %edi\n"
			      "popl %ebp\n"
			      "popl %eax\n"
			      "popl %ds\n"
			      "popl %es\n"
			      "popl %fs\n"
			      "popl %gs\n"
		       	      "iret\n"
				);


}

void roundrobin()
{  
   struct task_struct *ts = current();
	quantums_restantes--;
	ts->tics_totales++;

	if (quantums_restantes<=0 )
	{	 
		quantums_restantes = ts->quantum;
		if (numtasks>1){ 

		task_switch( (union task_union *)( list_first(&runqueue)->next), INT );//canvi de context al segon proces de la llista
          	}
	}

}



int buscar_task(int pid_incognito){

int i=0;

while (i<10) {
	if(task[i].task.pid==pid_incognito && (task[i].task.quantum != 0) ) return i;
	else i++;
	}
return -1;
}

void init_sem(){

	int i;
	for(i=0;i<NUM_SEMS;i++) sems[i].libre=1;
}

void desbloquear_proceso(struct list_head * l)
{
	struct list_head * lh = list_first(l);   
	list_del(lh);
	list_add_tail(lh,&runqueue);
}

void bloquear_proceso(struct list_head * l)
{
	struct list_head * lh = list_first(&runqueue);    
	list_del(lh);
	list_add_tail(lh,l);
	task_switch( (union task_union *)list_first(&runqueue) , NO_INT );
}


/*
//FUNCION RARA :S
void vomitarq(){

printk("el numtASks vale:");
printc(numtasks+'0');

int i;
for (i=0;i<10;i++){
	if (task[i].task.quantum==0) {
	printk("\n esta libre:");
	printc(task[i].task.pid+'0');

	}
}


}
*/
