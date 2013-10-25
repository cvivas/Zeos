#define JP_OPEN 0
#define JP_WRITE 1
#define JP_LSEEK 2
#define JP_CLOSE 3
#define JP_CLONE 4

#define CAMBIO_PROCESO	0
#define CAMBIO_SYSCALL	1
#define RESET_VALORES	2
#define RESET_VALORES_TODOS_PROCESOS	3
#define ACTIVAR_SYS_CALL	4
#define DESACTIVAR_SYS_CALL	5

struct module_info {
  int ini;
  int num_exit;
  int num_no_exit;
  unsigned long long temps;
};
