#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "pruebas.h"
#include <linux/kdev_t.h>

char buff[256];

void print_est(struct module_info *est)
{
	printf("Numero ejecuciones: %d \n", est->ini); 
	printf("Ejecuciones correctas: %d \n", est->num_exit);
	printf("Ejecuciones erroneas: %d \n", est->num_no_exit);
	printf("Tiempo total de ejecucion: %lld \n", est->temps);//long long integer
}

void imprimir_all_est(int fd, struct module_info * tinf)
{
	int i;
	for (i=0; i<5; i++)
	{
		if ( ioctl(fd,1,i) < 0 ) perror(0);
		if ( read(fd,tinf,1024) < 0 ) perror(0);
		print_est(tinf);
		
	}
}

int initmod(){

	int pid;
	char buff[256];

	printf("\n\n#########JUEGO DE PRUEBAS DE LA SEGUNDA ENTREGA DE PROSO#########\n\n");
	pid=getpid();
	printf("PID: %d\n",pid);
	printf("Pulsar 'enter' para comenzar la instalacion de los modulos\n");
	read(0,buff,1);
	switch(fork())
	{
		case 0:	sprintf(buff,"pid=%d",pid);
			printf("Instalando el modulo 1\n");
			execlp("insmod","insmod","modul1.ko",buff,(char *)0);
			printf("Error en el exec\n");
			exit(0);
		default:break;
	}

	//printf("Modulo 1 intalado correctamente\n");
	printf("Continuando con la instalacion...\n");
	switch(fork())
	{
		case 0:	
			sprintf(buff,"major=%d",250);
			printf("Instalando el modulo 2\n");
			execlp("insmod","insmod","modul2.ko",buff,(char *)0);
			printf("Error en l'exec\n");
			exit(0);
		default:break;
	}
	//printf("Modulo 2 intalado correctamente\n");
	printf("Pulsar 'enter' para continuar con el juego de pruebas\n");
	read(0,buff,1);
	
	return pid;
}


void endmod(){

	int pid;
	char buff[256];

	printf("\nProcediendo a la desintalacion de los modulos...\n");
	printf("Pulsar 'enter' para continuar\n");
	read(0,buff,1);
	switch(fork())
	{
		case 0:	printf("Desinstando el modulo 2\n");
			execlp("rmmod","rmmod","modul2",(char *)0);
			printf("Error en l'exec\n");
			exit(0);
		default:break;
	}

	switch(fork())
	{
		case 0:	printf("Desinstando el modulo 1\n");
			execlp("rmmod","rmmod","modul1",(char *)0);
			printf("Error en l'exec\n");
			exit(0);
		default:break;
	}
	printf("Pulsar 'enter' para acabar el juego de pruebas\n");
	read(0,buff,1);

	printf("\n#########FIN DEL JUEGO DE PRUEBAS#########\n\n");
}

void bucle_lseek(int fd)
{
int i,num;
for(i=0;i<10;++i)
	{
		//printf("Vuelta: %d / 10 \n",i);
		num=lseek(fd,0,SEEK_SET);
		//printf("Valor del num: %d  \n",num);
		if(num==-1){perror(buff);}
	}
}

void mon_lseek(int pid)
{
int i,fd,fd2,num;
int menus1=-1;
dev_t my_dev;
struct module_info est;

	fd=open("/home/alumne/Desktop/modulos/jocproves.c",O_RDONLY);
	printf("Resultado primer open: %d\n",fd);

	bucle_lseek(fd);

	fd2=dup(fd);
	close(fd);

	my_dev = MKDEV(250,0);
	i=mknod("disp_mod2",S_IFCHR,my_dev);

	fd=open("disp_mod2",O_RDONLY);

	printf("Resultado open disp : %d\n",fd);
	if(fd==-1){perror(buff);}

	printf("\nCada vez que monitorizamos lseek haremos 10 ejecuciones del mismo. \n");

	printf("\nMonitorizamos los LSEEK's del proceso con PID %d\n",pid);
	i=ioctl(fd,CAMBIO_PROCESO,&pid);
	if(i<0){perror(buff);}
	i=ioctl(fd,CAMBIO_SYSCALL,JP_LSEEK);
	if(i<0){perror(buff);}

	i=read(fd,&est,sizeof(struct module_info));
	print_est(&est);
	printf("Pulsa 'enter' para continuar\n");
	i=read(0,buff,1);

	printf("Desactivamos la monitorizacion de las llamadas\n");
	i=ioctl(fd,DESACTIVAR_SYS_CALL,&menus1);
	if(i==-1){perror(buff);}
	printf("Pulsa 'enter' para continuar\n");
	i=read(0,buff,1);

	bucle_lseek(fd);

	i=read(fd,&est,sizeof(struct module_info));
	print_est(&est);
	
	printf("\nReactivamos la monitorizacion de las llamadas\n");
	printf("Pulsa 'enter' para continuar\n");
	i=read(0,buff,1);

	i=ioctl(fd,ACTIVAR_SYS_CALL,&menus1);
	if(i==-1){perror(buff);}

	bucle_lseek(fd);

	i=read(fd,&est,sizeof(struct module_info));
	print_est(&est);

	printf("\nReseteamos las estadisticas\n");
	printf("Pulsa 'enter' para continuar\n");
	i=read(0,buff,1);
	i=ioctl(fd,RESET_VALORES,pid);
	if(i<0){perror(buff);}

	i=read(fd,&est,sizeof(struct module_info));
	print_est(&est);

	close(fd);

}

//////////////////////////////////////////////////////////////////////////////////////
void test_llamadas()
/*
Cada vez que llamemos a esta funcion efectuaremos 2 open's, 2 write's, 2 lseek's, 2 close's y 1 clone.
Habra una llamada correcta a la system call y otra incorrecta (excepto para el clone)
*/
{
	int fd_test;

	//printf("Open correcto y incorrecto\n");
	fd_test = open("test_1",O_CREAT | O_TRUNC | O_RDWR);
	open("potatoe",O_RDONLY);

	//printf("Write correcto y incorrectp\n");
	write(fd_test,"aaa",3);
	write(-1,"aaa",3);

	//printf("Lseek correcto y incorrecto\n");
	lseek(fd_test,1,SEEK_END);
	lseek(-1,1,SEEK_CUR);

	//printf("Close correcto y incorrecto\n");
	close(fd_test);
	close(-1);

	//printf("Clone correcto\n");
	if(fork() == 0) exit(0);
	
	printf("TEST_LLAMADAS\n");
}



void jp1()
{
	dev_t my_dev;
	struct module_info info;
	char buff[1024];
	int size,pid,fd,fd1,i;
	int arg2;
	int status, status2;
	
	pid = getpid();
	
	test_llamadas();

	my_dev = MKDEV(250,0);
	i=mknod("disp_mod2",S_IFCHR,my_dev);

	fd=open("disp_mod2",O_RDONLY);

	printf("Resultado open disp : %d\n",fd);
	if(fd==-1){perror(buff);}

	printf("Reseteamos todos los valores estadisticos\n");
	
	if ( ioctl(fd,RESET_VALORES,&arg2) < 0 ) error();	
	
	printf("Hacemos un open, escribimos y despues lo cerramos con un close\n");
	
	fd1 = open("fich_jp",O_CREAT|O_TRUNC | O_RDWR);
	if ( write(fd1,"aaa",3) < 0) error(); 
	close(fd1);
	
	printf("Hacemos un read del dispositivo y consultamos las estadisticas del OPEN\n");

	if ( read(fd,&info,1024) < 0 ) error();
	print_est(&info);
	
	printf("Cambiamos la llamada a WRITE\n");
	if ( ioctl(fd,CAMBIO_SYSCALL,JP_WRITE) < 0 ) error();
	
	printf("Leemos las estadisticas del WRITE\n");
	
	if ( read(fd,&info,1024) < 0 ) error();
	print_est(&info);
	
	printf("Canbiamos la llamada a LSEEK\n");
	
	arg2=3;
	if ( ioctl(fd,CAMBIO_SYSCALL,JP_LSEEK) < 0 ) error();
	
	printf("Leemos las e(Hijo)stadisticas del LSEEK\n");
	
	if ( read(fd,&info,1024) < 0 ) error();
	print_est(&info);
	
	printf("Cambiamos la llamada a CLOSE\n");
	
	arg2=2;
	if ( ioctl(fd,CAMBIO_SYSCALL,JP_CLOSE) < 0 ) error();
	
	printf("Lemos las estadisticas del CLOSE\n");
	
	if ( read(fd,&info,1024) < 0 ) error();
	print_est(&info);
	
	printf("Cambiamos la llamada a CLONE\n");
	
	arg2=4;
	if ( ioctl(fd,CAMBIO_SYSCALL,JP_CLONE) < 0 ) error();
	
	printf("Leemos las estadisticas del CLONE\n");
	
	if ( read(fd,&info,1024) < 0 ) error();
	print_est(&info);
	
	printf("Hacemos un fork\n");
	
	int fk = fork();
	
	if( fk == 0 )
	{
		struct module_info inf;
		
		arg2 = getpid();
		
		printf("(Hijo) PID = %d\n", arg2);
		
		printf("(Hijo) Hago que el dispositivo me monitorice\n");
		
		if ( ioctl(fd,CAMBIO_PROCESO,&arg2) < 0 ) error();
		
		printf("(Hijo) Activo la monotorizacion del open\n");
		
		arg2=1;
		if ( ioctl(fd,CAMBIO_SYSCALL,JP_OPEN) < 0 ) error();
		
		printf("(Hijo) Hacemos dos open's incorrectos\n");
		
		open("mongeta",O_RDONLY);
		open("llobarro",O_RDONLY);
		
		printf("(Hijo) Leemos las estadisticas del OPEN\n");
		
		if ( read(fd,&inf,1024) < 0 ) error();
		print_est(&inf);
		
		printf("(Hijo) me muero\n");
					
		exit(status);
	}
	
	waitpid(fk,&status,0);
	
	printf("Monotorizamos el proceso padre\n");
	
	arg2 = getpid();
	if ( ioctl(fd,CAMBIO_PROCESO,&arg2) < 0 ) error();
	
	printf("Seleccionamos la llamada OPEN\n");
	
	arg2=1;
	if ( ioctl(fd,CAMBIO_SYSCALL,JP_OPEN) < 0 ) error();
	
	printf("Reseteamos los valores\n");
	
	if ( ioctl(fd,RESET_VALORES,&arg2) < 0 ) error();
	
	printf("Imprimimos todas las estadisticas del proceso (los write's no estan a 0 debido a los mensajes)\n");
	
	imprimir_all_est(fd,&info);
	
	printf("Hacemos un test de llamadas (para comprobar despues que se borra con la opcion 3 del ioctl)\n");
	
	test_llamadas();
	
	printf("Creamos un hijo; el padre espera a que muera\n");
	
	int f = fork();
	
	if ( f == 0)
	{
		struct module_info info_fill;
		
		printf("(Hijo) Hacemos un test de llamadas\n");
		
		test_llamadas();
		
		printf("(Hijo) Imprimimos las estadisticas\n");
		
		arg2 = getpid();	// monotoritzacio del fill
		if ( ioctl(fd,CAMBIO_PROCESO,&arg2) < 0 ) error();
		
		imprimir_all_est(fd,&info_fill);
		
		printf("(Hijo) borramos las estadisticas de todos los procesos\n");
		
		if ( ioctl(fd,RESET_VALORES_TODOS_PROCESOS,0) < 0 ) error();
		
		printf("(Hijo) volvemos a imprimir las estadisticas\n");
		
		imprimir_all_est(fd,&info_fill);
		
		printf("(Hijo) me muero\n");
		
		exit(status2);
		
	}
	
	waitpid(f,&status2,0);
	
	printf("Monotoritzamos al padre\n");
	
	arg2 = getpid();	// monotorizamos el padre
	if ( ioctl(fd,CAMBIO_PROCESO,&arg2) < 0 ) error();
		
	printf("Imprimimos las estadisticas\n");
	
	imprimir_all_est(fd,&info);
	
	printf("Intentamos hacer otro open del dispositivo\n");
	
	if ( open("disp_mod2", O_RDONLY) < 0 ) printf("ERROR ABRIENDO EL DISPOSITIVO (OK)\n");
	
	printf("Liberamos el dispositivo\n");
		
	close(fd);
	

}
////////////////////////////////////////////////////////////////////////////////////


/* 
ESTRUCUTURA DEL JOC DE PROVES:
 
Inici del joc de proves. Imprimir missatge d’inici.
 1) Imprimir PID procés actual i bloquejar el procés fins que es premi una tecla.
 2) (Carregar el mòdul amb el PID del joc de proves)
 3) Prémer una tecla i continuar el joc de proves.
 4) Provar totes les llamadas a sistema monitoritzades.
 5) (Descarregar el mòdul, imprimint les estadístiques del procés)
 6) Imprimir missatge de finalització del joc de proves.
 7) Comprovar que els resultats són coherents amb el joc de proves.
 8) Finalitzar joc de proves.
*/

main(int argc, char *argv[])
{
int pid;

	pid=initmod();
	jp1();
	mon_lseek(pid);
	endmod();

}
