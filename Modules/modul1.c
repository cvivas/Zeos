#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include "modul1.h"



#define proso_rdtsc(low,high) \
__asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))

static inline unsigned long long proso_get_cycles (void) 
{
  unsigned long eax, edx;
  proso_rdtsc(eax, edx);
  return ((unsigned long long) edx << 32) + eax;
}

int pid=1;
module_param(pid,int,0);
MODULE_PARM_DESC (pid, "Process ID to monitor (default 1)");
MODULE_AUTHOR("YO10");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("ProSo Driver");

EXPORT_SYMBOL(init_myths);
EXPORT_SYMBOL(activar);
EXPORT_SYMBOL(desactivar);


/*
* Initialize the module.
*/


extern void * sys_call_table[];




void init_myths(int pidx,struct mythread_info * curr_thinfos)
{
int i;
curr_thinfos->pid=pidx;

	for (i=0;i<LLAMADAS_TOTALES;i++)
	{
	curr_thinfos->est[i].num_ent=0;
	curr_thinfos->est[i].num_sal_ok=0;
	curr_thinfos->est[i].num_sal_err=0;
	curr_thinfos->est[i].dur_tot=0;
	}
}

void init_statistics(void)
{
	struct task_struct * p;
	int call;
	
	for_each_process(p)
	{
		for (call=0; call<5; call++)
		{
			init_myths( p->pid, (struct mythread_info *)task_thread_info(p) );
		}
	}
}

void print_stats(int llamada)
{
	struct mythread_info * my_thread;
	
	struct task_struct *task;
	
	if (find_task_by_pid(pid) == NULL) return;
	
	task = find_task_by_pid (pid);
	
	my_thread = (struct mythread_info *)task_thread_info(task); 

	if(llamada==MY_OPEN) printk(KERN_EMERG "Open: \n");
	if(llamada==MY_CLOSE) printk(KERN_EMERG "Close: \n");
	if(llamada==MY_CLONE) printk(KERN_EMERG "Clone: \n");
	if(llamada==MY_LSEEK) printk(KERN_EMERG "Lseek: \n");
	if(llamada==MY_WRITE) printk(KERN_EMERG "Write: \n");
	
	printk("PID proceso %d", my_thread->pid);
	
	printk("\nNumero de llamadas: %d", my_thread->est[llamada].num_ent);
	
	printk("\nFinalizadas correctamente: %d", my_thread->est[llamada].num_sal_ok);
	
	printk("\nFinalitzadas con error: %d", my_thread->est[llamada].num_sal_err);
	
	printk("\nTiempo total de la llamada: %lld", my_thread->est[llamada].dur_tot);
	printk("\n");
}


/*
COMENTAREMOS SOLO UNA FUNCION LOCAL PORQUE SON TODAS MUY PARECIDAS
*/
ssize_t sys_write_local (unsigned int fd, const char __user * buf, size_t count)
{
unsigned long long antes,despues;
struct mythread_info * curr_thinfo;
int pids;
ssize_t rets;

	try_module_get(THIS_MODULE); 	// aumentamso el contador de uso del modulo

	antes = proso_get_cycles();	// obtenemos los ciclos

	// llamamos a la funcion original de kernel guardada en sys_call_old
	rets= ( (ssize_t(*) (unsigned int, const char * , size_t)) (sys_call_old[MY_WRITE]) ) (fd,buf,count);	

	despues = proso_get_cycles();	// obtenemos los ciclos y los restamos para obtener el tiempo de llamada
	despues -= antes;
	
	curr_thinfo= (struct mythread_info *) current_thread_info(); 	// buscamos el thread_info del proceso actual
	pids=current->pid;

	if (pids != curr_thinfo->pid) init_myths(pids,curr_thinfo);	// si el pid de las estadisticas no coincide con el pid actual inicializamos la estructura de estadistics
	
	curr_thinfo->est[MY_WRITE].num_ent++;	// completamos las estadisticas

	if (rets<0) curr_thinfo->est[MY_WRITE].num_sal_err++;

	else curr_thinfo->est[MY_WRITE].num_sal_ok++;

	curr_thinfo->est[MY_WRITE].dur_tot+=despues;

	module_put(THIS_MODULE);	// decrementamos el contador de uso del modulo

return rets;	// retornamos el resultado de la llamada original del kernel

}


long sys_open_local(const char __user * filename, int flags, int mode)
{
unsigned long long antes,despues;
struct mythread_info * curr_thinfo;
long rets;
int pids;

	try_module_get(THIS_MODULE);

	antes = proso_get_cycles();
	//rets= ( * sys_call_old[MY_OPEN]) (filename,flags,mode);
	rets= ( (long(*) (const char *, int, int))(sys_call_old[MY_OPEN]) ) (filename,flags,mode);

	despues=proso_get_cycles();
	despues -=antes;

	curr_thinfo= (struct mythread_info *) current_thread_info();
	pids=current->pid;

	if (pids != curr_thinfo->pid) init_myths(pids,curr_thinfo);
	
	curr_thinfo->est[MY_OPEN].num_ent++;

	if (rets<0) curr_thinfo->est[MY_OPEN].num_sal_err++;

	else curr_thinfo->est[MY_OPEN].num_sal_ok++;

	curr_thinfo->est[MY_OPEN].dur_tot+=despues;

	module_put(THIS_MODULE);

return rets;
}


long sys_close_local(unsigned int fd)
{
unsigned long long antes,despues;
long rets;
int pids;
struct mythread_info * curr_thinfo;

	try_module_get(THIS_MODULE);

	antes = proso_get_cycles();	
	//rets= ( * sys_call_old[MY_CLOSE]) (fd);
	rets= ((long (*) (unsigned int))(sys_call_old[MY_CLOSE])) (fd);

	despues=proso_get_cycles();
	despues -=antes;

	curr_thinfo= (struct mythread_info *) current_thread_info();
	pids=current->pid;

	if (pids != curr_thinfo->pid) init_myths(pids,curr_thinfo);

	curr_thinfo->est[MY_CLOSE].num_ent++;

	if (rets<0) curr_thinfo->est[MY_CLOSE].num_sal_err++;

	else curr_thinfo->est[MY_CLOSE].num_sal_ok++;

	curr_thinfo->est[MY_CLOSE].dur_tot+=despues;

	module_put(THIS_MODULE);

return rets;
}

off_t sys_lseek_local(unsigned int fd, off_t offset, unsigned int origin)
{
unsigned long long antes,despues;
off_t rets;
int pids;
struct mythread_info * curr_thinfo;

	try_module_get(THIS_MODULE);

	antes = proso_get_cycles();
	//rets= ( * sys_call_old[MY_LSEEK] ) (fd,offset,origin);
	rets= ((off_t(*)(int, off_t,unsigned int))(sys_call_old[MY_LSEEK])) (fd,offset,origin);
	
	despues=proso_get_cycles();
	despues -=antes;

	curr_thinfo= (struct mythread_info *) current_thread_info();
	pids=current->pid;

	if (pids != curr_thinfo->pid) init_myths(pids,curr_thinfo);

	curr_thinfo->est[MY_LSEEK].num_ent++;

	if (rets<0) curr_thinfo->est[MY_LSEEK].num_sal_err++;

	else curr_thinfo->est[MY_LSEEK].num_sal_ok++;

	curr_thinfo->est[MY_LSEEK].dur_tot+=despues;
	
	module_put(THIS_MODULE);

return rets;

}

int sys_clone_local(struct pt_regs regs)
{
unsigned long long antes,despues;
int rets,pids;
struct mythread_info * curr_thinfo;

	try_module_get(THIS_MODULE);
	
	antes = proso_get_cycles();
	//rets= ( * sys_call_old[MY_CLONE]) (regs);
	rets= ( (int(*) (struct pt_regs)) (sys_call_old[MY_CLONE]) ) (regs);

	despues=proso_get_cycles();
	despues -=antes; 

	curr_thinfo= (struct mythread_info *) current_thread_info();
	pids=current->pid;

	if (pids != curr_thinfo->pid) init_myths(pids,curr_thinfo);

	curr_thinfo->est[MY_CLONE].num_ent++;
	if (rets<0){
	curr_thinfo->est[MY_CLONE].num_sal_err++;
	}
	else {
	curr_thinfo->est[MY_CLONE].num_sal_ok++;
	}
	curr_thinfo->est[MY_CLONE].dur_tot+=despues;
	module_put(THIS_MODULE);
	return rets;

}


void desactivar(int arg)
{
	if (arg == MY_OPEN || arg < 0) sys_call_table[POS_SYSCALL_OPEN] = sys_call_old[MY_OPEN];
	if (arg == MY_CLOSE || arg < 0) sys_call_table[POS_SYSCALL_CLOSE] = sys_call_old[MY_CLOSE];
	if (arg == MY_CLONE || arg < 0) sys_call_table[POS_SYSCALL_CLONE] = sys_call_old[MY_CLONE];
	if (arg == MY_WRITE || arg < 0) sys_call_table[POS_SYSCALL_WRITE] = sys_call_old[MY_WRITE];
	if (arg == MY_LSEEK || arg < 0) sys_call_table[POS_SYSCALL_LSEEK] = sys_call_old[MY_LSEEK];
}

void activar(int arg)
{
	if (arg == MY_WRITE || arg < 0) sys_call_table[POS_SYSCALL_WRITE] =  sys_write_local;
	if (arg == MY_OPEN || arg < 0) sys_call_table[POS_SYSCALL_OPEN] =  sys_open_local;
	if (arg == MY_CLOSE || arg < 0) sys_call_table[POS_SYSCALL_CLOSE] =  sys_close_local;
	if (arg == MY_CLONE || arg < 0) sys_call_table[POS_SYSCALL_CLONE] =  sys_clone_local;
	if (arg == MY_LSEEK || arg < 0) sys_call_table[POS_SYSCALL_LSEEK] =  sys_lseek_local;
}


static int __init Modul1_init(void)
{

  if (find_task_by_pid(pid) == NULL) return -1;

  init_statistics(); // inicializamos las estadisticas de todos los procesos

  sys_call_old[MY_WRITE]=sys_call_table[POS_SYSCALL_WRITE];
  sys_call_old[MY_OPEN]=sys_call_table[POS_SYSCALL_OPEN];
  sys_call_old[MY_CLOSE]=sys_call_table[POS_SYSCALL_CLOSE];
  sys_call_old[MY_LSEEK]=sys_call_table[POS_SYSCALL_LSEEK];
  sys_call_old[MY_CLONE]=sys_call_table[POS_SYSCALL_CLONE];

  activar(-1);

  printk(KERN_ALERT "Modulo1 cargado con exito\n");


  return 0;
  // This function returns 0 is everything is OK
  // and < 0 in case of error
}

/*
* Unload the module.
*/

static void __exit Modul1_exit(void)
{
/* Finalization code*/

	print_stats(MY_OPEN);
	print_stats(MY_WRITE);
	print_stats(MY_CLOSE);
	print_stats(MY_CLONE);
	print_stats(MY_LSEEK);

	desactivar(-1);

	printk(KERN_EMERG "Modul1: desinstalado correctamente\n");

}

module_init(Modul1_init);
module_exit(Modul1_exit);
