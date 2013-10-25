#include <linux/types.h>
#include <linux/unistd.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <asm-i386/uaccess.h>
#include "modul1.h"
#include "modul2.h"

MODULE_AUTHOR("YO10");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Modul 2");

dev_t dev;			// estructura para registrar el dispositivo
struct cdev *my_cdev;		// estructura del dispositivo
int major = 0, minor = 0;	
int result;
int pid, pid_open;		// guardamos pids
int llamada = MY_OPEN;		// llamada a analizar (por defecto :open)
char open=0;			// booleano que indica si se ha realizado el open
/*
Inicialitzacion del parametro MAJOR del modulo.
si es 0 se elige automaticamente, sino se utilizara el insertado por el usuario.
*/
module_param (major, int, 0);
MODULE_PARM_DESC (major, "MAJOR number (default 0)");



/* llena la estructura t_info con las estadisticas del proceso actual */
void llenar_t_info(struct t_info * t)
{
	struct mythread_info * my_tf;
	my_tf = (struct mythread_info *)task_thread_info(find_task_by_pid(pid));
	
	t->num_entrades = my_tf->est[llamada].num_ent;
	t->num_sortides_ok = my_tf->est[llamada].num_sal_ok;
	t->num_sortides_error = my_tf->est[llamada].num_sal_err;
	t->durada_total = my_tf->est[llamada].dur_tot;
}

int minimo(int a, int b)
{
	if(a<b) return a;
	return b;
}

//resetea los valores estadisticos de todos los procesos.
void reset_valors(void)
{
	struct task_struct * p;
	
	for_each_process(p)
	{
		init_myths( p->pid,( struct mythread_info *)task_thread_info(p) );
	}
}

//resetea los valores estadisticos del proceso actual
void reset_valors_actuals(void)
{
	struct task_struct * p = current;
	
	init_myths(  p->pid, (struct mythread_info *)task_thread_info(p));
	
}



int mymod_read(struct file *f, char __user *buffer, size_t s, loff_t *off)
{
	struct t_info read_t_info;

	if (open == 0) return -EPERM;	// si no hemos hecho el open dara error

	if (s < 0) return -EINVAL;	// error si el tamany a llegir es negativo...

	llenar_t_info(&read_t_info);	// llenamos la estructura con las estadisticas	

	
//si copy_to_user retorna un valor positivo, entonces ha habido un error (no ha copiado algunos o todos los datos)
	if(copy_to_user(buffer,&read_t_info,minimo(s,sizeof(struct t_info)))>0)
		return -1;		

	return minimo(s,sizeof(struct t_info)); //retornamos numero de valores leidos
}

int mymod_ioctl(struct inode *i, struct file *f, unsigned int arg1, unsigned long arg2)
{
	if (open == 0) return -EPERM; // si no hemos hecho el open dara error
	
	switch (arg1)
	{
		case 0: // cambio de pid
			if ((long *)arg2 == NULL)
				pid = pid_open;
			else
				pid = *(long *)arg2;
			break;
		case 1: // cambio de llamada
			if (arg2 < MY_OPEN || arg2 > MY_CLONE) return -EINVAL;
			llamada =  arg2;
			break;
		case 2: // reset de las estadisticas actuales
			reset_valors_actuals();
			break;
		case 3: // reset de les estadisticas de todos los procesos
			reset_valors();
			break;
		case 4: // activar monotoritzacion de llamada ( o de todas )
			activar(*(long *)arg2);
			break;
		case 5: // desactivar monotoritzacion de llamada ( o de todas )
			desactivar(*(long *)arg2);
			break;
		default:
			return -EINVAL;
	}
	return 0;
}

int mymod_release (struct inode *i, struct file *f)
{
	open = 0; // liberamos el dispositivo para que otro proceso puede acceder a el
	return 0;
}
int mymod_open (struct inode *i, struct file *f)
{
	
	if (current->uid != 0) return -EACCES;	// solo puede hacer open con privilegios de root.
	
	if (open == 1) return -EPERM;  	//error si ya se ha realizado un open con anterioridad
	
	open = 1;

	pid_open = current->pid;	//guardamos el pid del proceso que ha hecho el open
	pid = pid_open;			// pid actual a analizar = pid del proceso que ha hecho el open
	
	return 0;
}




static int __init Modul2_init(void)
{

	// codigo obtenido del libro "Linux Device Drivers" (capitol 3)  http://lwn.net/Kernel/LDD3/
	if (major)
	{
    		dev = MKDEV(major, minor);
    		result = register_chrdev_region(dev, COUNT, "disp_mod2");
	}
	else
	{
		result = alloc_chrdev_region(&dev, minor, COUNT,"disp_mod2");
		major = MAJOR(dev);
	}
	if (result < 0) 
	{
    		printk(KERN_WARNING "mymodule2: can't get major %d\n", major);
    		return result;
	}
	// fin del codigo obtenido del libro.
	
	my_cdev = cdev_alloc(); // reservamos espacio para la estructura cdev
	
	my_cdev->owner = THIS_MODULE;
	my_cdev->ops = &my_fops;
	if(cdev_add(my_cdev,dev,COUNT)<0)
	{
		printk(KERN_WARNING "No se puede insertar el dispositivo\n");
		return -1;
	}
	
	pid = current->pid; // pid del proceso a consultar = pid del proceso que ha iniciado el modulo
	
	printk(KERN_EMERG "Modul2: modulo iniciado correctamente\n");
	return 0;
	
}

static void __exit Modul2_exit(void)
{
	
	cdev_del(my_cdev); 	//borramos cdev

	unregister_chrdev_region(dev,COUNT); //liberamos dispositivo
	
	printk(KERN_EMERG "Modul2: modulo eliminado correctamente\n");
}

module_init(Modul2_init);
module_exit(Modul2_exit);
