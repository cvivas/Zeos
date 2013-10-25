/*
 * sys.c - Syscalls implementation
 */

#include <devices.h>
#include <errno.h>
#include <varias.h>
#include <types.h>
#include <sched.h>
#include <utils.h>
#include <mm_address.h>
#include <mm.h>
#include <list.h>



int sys_write(int fd,char *buffer, int size)
{
int comp,ret,j,i;
ret=size;
char kbuffer[512];
comp=comprueba_lec(fd,ESCRITURA);
if (comp <0) return comp;
comp=comprueba_punt((void*)buffer);
if (comp <0) return comp;
if (size <0) return -EINVAL;
if (buffer!= NULL){


  j=0;
  while(size>0)
  {	
    
     if(size<=512)
     {
          copy_from_user(&buffer[j],kbuffer,size);/*copia de buffer de usuari a buffer de kernel */
		i=size;
     }
     else
     { 
		copy_from_user(&buffer[j],kbuffer,512);
		i=512;
		j=j+512;
     }
     size-=sys_write_console (kbuffer, i);      
  }

}
	   
return ret;
}


int sys_ni_syscall()
{
  return -ENOSYS;
}


int sys_getpid()
{
  struct task_struct * p_actual = current();/*punter al inicio del proceso actual*/
  return p_actual->pid;
}


int sys_fork(){

	int ent,frame_del_hijo;
	union task_union * ts_pare;
	struct task_struct *t;
	union task_union * ts_hijo;

/*	ent=hay_sitio_en_vp();
	if (ent==-1) return -EAGAIN;
*/
	if(list_empty(&freebird)) return -EAGAIN;

	t = list_head_to_task_struct(list_first(&freebird));
	list_del(list_first(&freebird));
	ts_hijo = (union task_union * ) t;
	//copiem la task_union del pare al fill
  	ts_pare = (union task_union *)current();

//  	copy_data(ts_pare, &task[ent], PAGE_SIZE);
	copy_data(ts_pare,ts_hijo,PAGE_SIZE);

	//Busca frames libes donde poner el hijo	
	frame_del_hijo = alloc_frames(NUM_PAG_DATA);
	
	if (frame_del_hijo==-1) return -ENOMEM;

	//Inicializa task struct del hijo
	//init_task(ent,frame_del_hijo);
	init_taskf( ts_hijo ,frame_del_hijo);

	//copiamos los datos del padre al hijo
	cp_datos_padre(frame_del_hijo);

	//modificamos el valor que retornara el hijo
	//mod_valor_ret_hijo(&task[ent]);
	mod_valor_ret_hijo(&ts_hijo);

	/*anadimos el proceso al final de la runqueue*/
	//list_add_tail( &(task[ent].task.lhtask) , &runqueue);
	list_add_tail( &(ts_hijo->task.lhtask) , &runqueue);
	//list_add_tail ( &(task[ent].task.lhtask),list_first(&runqueue));

//printrq();
//task_switch( (union task_union *) ((&runqueue)->prev) , INT );//canvi de context al segon proces de la llista

//task_switch ( task[ent],NO_INT);

//   return task[ent].task.pid;
//printk("el print a devolver en el return del fork es : ");printc(ts_hijo->task.pid+'0');
   return t->pid;



}

void sys_exit(){

  //struct task_struct * p_actual = current();/*punter al inicio del proceso actual*/
struct task_struct *p_actual = list_head_to_task_struct(list_first(&runqueue));

//printk("imprimimos el pid en el exit: ");
 ///////////////////////////////// printc(p_actual->pid+'0');
//vomitarq();
	if (p_actual->pid !=0){
		numtasks--;
		p_actual->quantum=0; //liberamos la entrada en el vector de procesos

//printrq();
		list_del(&p_actual->lhtask); //eliminamos el proceso de la runqueue
		list_add_tail(&p_actual->lhtask,&freebird);//anadimos el proceso a la freequeue (freebird)

//printk("despues de eliminar el elemento actual");

//printrq();
		free_frames (p_actual->frame, NUM_PAG_DATA); // liberamos los frames

/*struct task_struct * p_actual2=list_head_to_task_struct(list_first(&runqueue));
printk("imprimimos el pid2 en el exit: ");
  printc(p_actual2->pid+'0');*/
//vomitarq();
		
		task_switch ( (union task_union *) list_first(&runqueue),NO_INT);//hacemos un task switch
		}



}

int sys_nice(int quantum){

  struct task_struct * p_actual = current();/*punter al inicio del proceso actual*/
  int a=p_actual->quantum;
  p_actual->quantum=quantum;
  return a;

}

int sys_get_stats(int pid, int *tics){

int i;
int comp = comprueba_punt(tics);

if (comp<0) return comp; //ret EFAULT

i=buscar_task(pid);

if (i<0) return -ESRCH;//no existe el proceso
else {
	*tics=task[i].task.tics_totales;

} 	

return 0;

}

int sys_sem_init(int n_sem, unsigned int value)
{


   if((n_sem<0)||(n_sem>=NUM_SEMS))return -EINVAL;//identificador no valido
   if(sems[n_sem].libre == 0) return -EPERM; //semaforo ya inicialitzado

   sems[n_sem].contador=value;
   sems[n_sem].libre=0;
   INIT_LIST_HEAD(&sems[n_sem].blocklist);

return 0;
}

int sys_sem_wait(int n_sem)
{
   struct task_struct *ts = current();

   if((n_sem<0)||(n_sem>=NUM_SEMS))return -EINVAL;//identificador no valido
   if(sems[n_sem].libre == 1) return -EPERM; //semaforo no inicialitzado
   if(ts->pid == 0)return -EPERM;//no se puede bloquear el proceso 0

   if(sems[n_sem].contador<=0){
//printk("bloqueamos el proceso");
bloquear_proceso(&sems[n_sem].blocklist);
}
   else {//printk("NO bloqueamos el proceso");
sems[n_sem].contador--; 
}	

return 0;
}

int sys_sem_signal(int n_sem)
{
   if((n_sem<0)||(n_sem>=NUM_SEMS))return -EINVAL;//identificador no valido
   if(sems[n_sem].libre == 1) return -EPERM; //semaforo no inicialitzado
//printk("\ncomprobacion de blocklist vacia:");
   if(list_empty(&sems[n_sem].blocklist) != 0) {sems[n_sem].contador++;
//printk("incrementamos contador");
}
   else{
 desbloquear_proceso(&sems[n_sem].blocklist);

//printk("\nvamos a desbloquear");
}  
return 0;
}

int sys_sem_destroy(int n_sem)
{

   if((n_sem<0)||(n_sem>=NUM_SEMS))return -EINVAL;//identificador no valido
   if(sems[n_sem].libre == 1) return -EPERM; //semaforo no inicialitzado
   if(list_empty(&sems[n_sem].blocklist)==0) return -EPERM;
// printk("\nAcabamos de obliterar al semaforo: ");printc(n_sem+'0');
   sems[n_sem].libre = 1;
return 0;
} 





/*
void printrq(){
struct task_struct * p_actual=list_head_to_task_struct(list_first(&runqueue));
struct task_struct * p_actual2=list_head_to_task_struct((&runqueue)->prev);
printk("\n el primer elemento de la rq es: ");
printc(p_actual->pid+'0');
printk("\n el ultimo elemento de la rq es: ");
printc(p_actual2->pid+'0');

}

********/


