/*
 * interrupt.c -
 */
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <entry.h>
#include <io.h>
#include <varias.h>
#include <sched.h>
#include <sys.h>



Gate idt[IDT_ENTRIES];
Register    idtR;
int tics, segons, minuts,alterne,horas;
char char_map[] =
{
  '\0','\0','1','2','3','4','5','6',
  '7','8','9','0','\'','¡','\0','\0',
  'q','w','e','r','t','y','u','i',
  'o','p','`','+','\0','\0','a','s',
  'd','f','g','h','j','k','l','ñ',
  '\0','º','\0','ç','z','x','c','v',
  'b','n','m',',','.','-','\0','*',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','7',
  '8','9','-','4','5','6','+','1',
  '2','3','0','\0','\0','\0','<','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0'
};

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE INTERRUPTION GATE FLAGS:                          R1: pg. 5-11  */
  /* ***************************                                         */
  /* flags = x xx 0x110 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE TRAP GATE FLAGS:                                  R1: pg. 5-11  */
  /* ********************                                                */
  /* flags = x xx 0x111 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);

  //flags |= 0x8F00;    /* P = 1, D = 1, Type = 1111 (Trap Gate) */
  /* Changed to 0x8e00 to convert it to an 'interrupt gate' and so
     the system calls will be thread-safe. */
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}
void setIdt()
{
  /* Program interrups/exception service routines */
  idtR.base  = (DWord)idt;
  idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;
  /* ADD INITIALIZATION CODE FOR INTERRUPT VECTOR */
	/*EXCEPCIONES*/
  setInterruptHandler(0,divide_error_handler, 0);
  setInterruptHandler(1,debug_handler , 3);
  setInterruptHandler(2,nm1_handler ,0);
  setInterruptHandler(3,breakpoint_handler , 3);
  setInterruptHandler(4,overflow_handler , 0);
  setInterruptHandler(5,bounds_check_handler , 0);
  setInterruptHandler(6,invalid_opcode_handler , 0);
  setInterruptHandler(7,device_not_available_handler , 0);
  setInterruptHandler(8,double_fault_handler , 0);
  setInterruptHandler(9,coprocessor_segment_overrun_handler ,0);
  setInterruptHandler(10,invalid_tss_handler , 0);
  setInterruptHandler(11,segment_not_present_handler , 0);
  setInterruptHandler(12,stack_exception_handler , 0);
  setInterruptHandler(13,general_protection_handler, 0);
  setInterruptHandler(14,page_fault_handler, 0);
  setInterruptHandler(16,floating_point_error_handler , 0);
  setInterruptHandler(17,alignment_check_handler , 0);

	/*INTERRUPCIONES*/
  setInterruptHandler(32,clock_handler, 0);
  setInterruptHandler(33,keyboard_handler, 0);
  
  /*CRIDES A SISTEMA: */

  setTrapHandler(0x80,system_call,3);

  set_idt_reg(&idtR);
}


void divide_error_routine()
{
  printk("excepcio divide_error");
  while (1);
}

void debug_routine()
{
  printk("excepcio debug");
  while (1);
}

void nm1_routine()
{
  printk("excepcio nm1");
  while (1);
}

void breakpoint_routine()
{
  printk("excepcio breakpoint");
  while (1);
}

void overflow_routine()
{
  printk("excepcio overflow");
  while (1);
}

void bounds_check_routine()
{
  printk("excepcio bounds_check");
  while (1);
}

void invalid_opcode_routine()
{
  printk("excepcio invalid_opcode");
  while (1);
}

void device_not_available_routine()
{
  printk("excepcio device_not_available");
  while (1);
}

void double_fault_routine()
{
  printk("excepcio double_fault");
  while (1);
}

void coprocessor_segment_overrun_routine()
{
  printk("excepcio coprocessor_segment_overrun");
  while (1);
}

void invalid_tss_routine()
{
  printk("excepcio invalid_tss");
  while (1);
}

void segment_not_present_routine()
{
  printk("excepcio segment_not_present");
  while (1);
}

void stack_exception_routine()
{
  printk("excepcio stack_exception");
  while (1);
}

void general_protection_routine()
{
  printk("excepcio general_protection");
  while (1);
}

void page_fault_routine()
{

	struct task_struct * ts = current();
	char buffer[2];
	
	printk("\n Page_fault exception from proces with pid: ");
	itoa(ts->pid, buffer);
	printk(buffer);
	
	if (ts->pid == 0) 
	{
		while(1);
	}
	else sys_exit();
  /*printk("excepcio page_fault");
  while (1);	*/
	
}

void floating_point_error_routine()
{
  printk("excepcio floating_point_error");
  while (1);
}

void alignment_check_routine()
{
  printk("excepcio alignment_check");
  while (1);
}

/*INTERRUPCIONS: */

void clock_routine()
{
char buffer [9];
tics++;

	if (tics%18==0){
		tics -=18;
		segons ++;
		if (segons%60==0){
		minuts++;
		segons =0;
		
		if (minuts%60==0){
		horas++;
		minuts =0;
		}
		}
	itoa (horas,buffer);
	itoa(minuts, &buffer[3]);
	itoa(segons, &buffer[6]);
	buffer[2]=':';
	buffer[5]=':';
	buffer[8]='\0';
		printk_xy(71,0,buffer);
	}
   roundrobin();
}

void keyboard_routine(){

unsigned char codi = inb(0x60);
  unsigned char caracter;
  int make = (codi&0x80)>>7;
   
if (make == 0)
    {

      caracter=char_map[codi&0x7F];
	if (caracter != '\0') {
	  printk_xy(73,24,"      ");
	  printc_xy(79,24,caracter);
	}	  
 	else {
	
 	  printk_xy(73,24,"control");
	
 	}
}



}
 
