#include <libc.h>
#define PERROR()	perror();
#define FINJP	while(1)
#define COMPERROR(x)	if (x<0){PERROR ();}
#define CERROR(x)	if (x<0){ PERROR();FINJP;}
#define PRINTF(x)	CERROR(write(1,x,strlen(x)))


int strlen( char *buffer ) {
    int i=0;
    for(;buffer[i];i++);
    return i;
}

void jp_sem2()
{
int n,i,res;
PRINTF("\n ---En esta prueba se trataran los errores (6) de los semaforos por parametros mal pasados---\n");

n=sem_init(-1,0); PRINTF("\nError por iniciar semaforo en pos '-1': ");COMPERROR(n);
n=sem_init(31,0); PRINTF("\nError por iniciar semaforo en pos '31': ");COMPERROR(n);
sem_init(1,0);
n=sem_init(1,0); PRINTF("\nError por iniciar semaforo en pos '1' dos veces : ");COMPERROR(n);

PRINTF("\nlas comprobaciones de posiciones iniciales son identicas por tanto solo las comprobaremos para el sem_init");
n=sem_wait(2); PRINTF("\nError por hacer wait en una posicion no inicializada : ");COMPERROR(n);
n=sem_wait(1); PRINTF("\nError por hacer wait al proceso 0 : ");COMPERROR(n);
sem_init(2,0);

     for (i=1;i<3;i++)
     {
          if (getpid()==0) res=fork();
          if (res==-1) 
          {
               perror();
               
          }
     }


if (getpid()==0) {

	}

if (getpid()==1){
	sem_wait(1);
	n=sem_destroy(2);
	PRINTF("\nError por intentar destruir un semaforo que tiene a un proceso bloqueado : \n");
	COMPERROR(n);
	sem_signal(2);
	exit();
	}

if (getpid()==2) {
	sem_signal(1);
	sem_wait(2);
	PRINTF("\n---fin de pruebas de paso de parametros a semaforos---");
	}

}


void jp_sem1()
{
PRINTF("\n\n --- aparecera en pantalla P: 1, 2, 2, 3, 3, 1, 1,destruir(1)(2),3 ---");
char buf[1],buf2[2];
int i,res,n,tics;
sem_init(1,0);sem_init(2,0);
res=0;

     for (i=1;i<4;i++)
     {
          if (getpid()==0) res=fork();
          if (res==-1) 
          {
               perror();
               
          }
     }

switch (getpid()){

	case 0: break;

	case 1:{

		perror();
		PRINTF("\nP1 se esperara");
		sem_wait(1);
		perror();
		PRINTF("\nP1 esta esperando el wait2");
		sem_wait(2);
		sem_signal(1);
		perror();
		perror();
		PRINTF("\nP1 termina de esperar");
		exit();
		break;
		}

	case 2:	{
		PRINTF("\nP2 aparece");
		sem_signal(1);
		perror();
		PRINTF("\nP2 se va");
		exit();break;
		}
	case 3: {
		PRINTF("\nP3 aparece");
		sem_signal(2);
		perror();
		PRINTF("\nP3 espera al P1");
		sem_wait(1);
		perror();
		PRINTF("\ndestruimos sem1 y sem2");
		sem_destroy(1);
		perror();
		n=sem_destroy(2);
		COMPERROR(n);
		PRINTF("\nP3 dice: test completado con exito");
		exit();break;


	}
		//sem_signal(1);
buf[0]=getpid()+'0';
write(1,buf,1);


}

}

void jp_fork()
{

PRINTF("\n\n --- Este juego de pruebas tendra como salida nice:30, g_s(4):0X, Page fault 5, g_s(5)error, pid(9)=9----");
char buf[1],buf2[2];
int i,res,n,tics;
res=0;

     for (i=1;i<10;i++)
     {
          if (getpid()==0){ res=fork();}
          if (res==-1) 
          {
               perror();
               
          }
     }


switch (getpid()){

	case 0: break;
	case 1: exit(); break;
	case 2: exit();break;
	case 3: {
		write(1,"\nnice: ",7);
		n=nice(40);
		buf2[1]=n%10 + '0';
		buf2[0] = n/10 + '0';
		write(1,buf2,2);
		exit();break;
		}
	case 4: {
		i=0;
		while(i<100000){i++;}
		get_stats(4,&tics);
		write(1,"\nget_Stats(4): ",14);
		buf2[1]=tics%10 + '0';buf2[0] = tics/10 + '0';write(1,buf2,2);
		break;
		}
	case 5:{
		int pfault;
	  	__asm__ __volatile__(
			       "movl $0, %%eax\n"
			       "movl (%%eax),%%ebx\n"
			       : 
			       :
			       :"ax","bx","memory"
			       );
	
	
		exit();break;
		}
	case 6:{
		i=0;
		while(i<100000){i++;}
		n=get_stats(5,&tics);
		PRINTF("\nget_Stats(5): ");
		if(n==-1)perror();
	 	else {buf2[1]=tics%10 + '0';buf2[0] = tics/10 + '0';
		PRINTF(buf2);}
		exit();break;
		}
	case 7: exit();break;
	case 8: exit();break;
	case 9: write(1,"\ngetpid(9): ",12);buf[0]=getpid()+'0';write(1,buf,1);exit();
	}
perror();

}

void jp_fork2()
{

PRINTF("\n\n --- Este juego de pruebas tendra como salida los pids de los procesos que mueran y los mensajes de error por no poder superar el limite de procesos ---");
char buf[3];
int i,res;
nice(1);
/*El proceso 0 intentara crear hijos sin parar; en caso de superar el max de proceos del zeos empezaremos a ver mensajes de error asta que el padre salga de run por culpa del round robin. En ese caso los hijos moriran (mostrandonos antes su pid) y permitiran al proceso 0 volver a crear hijos cuando pase a ejecutarse.*/

     for (i=1;i<1000;i++) //intentamos crear 1000 procesos
     {
          if (getpid()==0) res=fork();
          if (res==-1) 
          {
               perror();
               
          }
     }


	if (getpid!=0) //mataremos todos los procesos menos el padre 
		{
		write(1,"\nproceso: ",10);
		buf[2]=getpid()%10 +'0';
		buf[1]=(getpid()/10)%10 +'0';
		buf[0]=getpid()/100 +'0';
		write(1,buf,3);
		exit();
		}

}

void jp_fork3()
{
PRINTF("\n\n --- Este juego de pruebas tendra como salida 0123456789 ---");
/*En este caso lo que intentaremos ver es que tanto el padre como los hijos pueden accepder a variables "comunes". Para esto haremos que los 9 hijos escriban su PID en posiciones consecutivas de un mismo vector creado por el proceso 0*/
char buf[10];
int i,res;


     for (i=1;i<10;i++)
     {
          if (getpid()==0) res=fork();
          if (res==-1) 
          {
              perror();
               
          }
     }


switch (getpid()){

	case 0: buf[getpid()]=getpid()+'0';break;
	case 1: buf[getpid()]=getpid()+'0';break;
	case 2: buf[getpid()]=getpid()+'0';break;
	case 3: buf[getpid()]=getpid()+'0';break;
	case 4: buf[getpid()]=getpid()+'0';break;
	case 5: buf[getpid()]=getpid()+'0';break;
	case 6: buf[getpid()]=getpid()+'0';break;
	case 7: buf[getpid()]=getpid()+'0';break;
	case 8: buf[getpid()]=getpid()+'0';break;
	case 9: buf[getpid()]=getpid()+'0';break;


	}

 write(1,buf,10);

}



int __attribute__ ((__section__(".text.main")))
  main(void)
{

 write(1,"\n",2);

//jp_sem2();	//prueba las inicializaciones de los semaforos
//jp_sem1();	//prueba los diferentes semaforos
//jp_fork();	//prueba del fork basica
//jp_fork2();	//prueba el fork a lo bestia (creacion de hijos masiva)
jp_fork3();	//prueba que los hijos y el padre comparten recursos al hacer fork


/*
char buf[1];

int res=fork();
if (getpid()==1){
write(1,"\nwe kill:",9);buf[0]=getpid()+'0';write(1,buf,1);
exit();
}
write(1,"\npid p0            :",9);buf[0]=getpid()+'0';write(1,buf,1);
**********************/


/*char buf[1];
int i,res;


     for (i=1;i<10;i++)
     {
          if (getpid()==0) res=fork();
          if (res==-1) 
          {
               perror();
               
          }
     }


switch (getpid()){

case 0: write(1,"ffff",4);break;
case 1:  buf[0]=getpid()+'0';write(1,buf,1);write(1,"ssss",4); break;
case 2: buf[0]=getpid()+'0';write(1,buf,1);write(1,"aaaaa",4);break;
case 3: exit();
case 4 : buf[0]=getpid()+'0';write(1,buf,1);write(1,"nnnn",4); break;
case 5:exit();
case 6:exit();
case 7:exit();
case 8:exit();
case 9:buf[0]=getpid()+'0';write(1,buf,1);write(1,"uuuu",4);exit();


}*/



  while(1);
}

void jp_fork0(){
int i,rest;
rest=0;


     for (i=0;i<10;i++)
     {
          if (getpid()==0) rest=fork();
          if (rest==-1) 
          {
               perror();
               
          }
     }

}
