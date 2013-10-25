/*
 * mm.c - Memory Management: Paging & segment memory management
 */

#include <types.h>
#include <mm.h>
#include <segment.h>
#include <hardware.h>
#include <sched.h>
#include <utils.h>

/* SEGMENTATION */
/* Memory segements description table */
Descriptor  *gdt = (Descriptor *) GDT_START;
/* Register pointing to the memory segments table */
Register    gdtR;

/* PAGING */
/* Variables containing the page directory and the page table */
  
page_table_entry dir_pages[TOTAL_PAGES]
  __attribute__((__section__(".data.task")));

page_table_entry pagusr_table[TOTAL_PAGES]
  __attribute__((__section__(".data.task")));

/* TSS */
TSS         tss; 



/***********************************************/
/************** PAGING MANAGEMENT **************/
/***********************************************/

/* Init page table directory */
  
void init_dir_pages()
{
  dir_pages[ENTRY_DIR_PAGES].entry = 0;
  dir_pages[ENTRY_DIR_PAGES].bits.pbase_addr = (((unsigned int)&pagusr_table) >> 12);
  dir_pages[ENTRY_DIR_PAGES].bits.user = 1;
  dir_pages[ENTRY_DIR_PAGES].bits.rw = 1;
  dir_pages[ENTRY_DIR_PAGES].bits.present = 1;
}

/* Initializes the page table (kernel pages only) */
void init_table_pages()
{
  int i;
  /* reset all entries */
  for (i=0; i<TOTAL_PAGES; i++)
    {
      pagusr_table[i].entry = 0;
    }
  /* Init kernel pages */
  for (i=0; i<NUM_PAG_KERNEL; i++)
    {
      // Logical page equal to physical page (frame)
      pagusr_table[i].bits.pbase_addr = i;
      pagusr_table[i].bits.rw = 1;
      pagusr_table[i].bits.present = 1;
    }
}

/* Initialize pages for initial process (user pages) */
void set_user_pages( int first_ph_page)
{
 int pag; 
  /* CODE */
  for (pag=PAG_LOG_INIT_CODE_P0;pag<PAG_LOG_INIT_DATA_P0;pag++){
  	pagusr_table[pag].entry = 0;
  	pagusr_table[pag].bits.pbase_addr = first_ph_page;
  	pagusr_table[pag].bits.user = 1;
  	pagusr_table[pag].bits.present = 1;
        first_ph_page++;
  }
  
  /* DATA */ 
  for (pag=PAG_LOG_INIT_DATA_P0;pag<PAG_LOG_INIT_DATA_P0+NUM_PAG_DATA;pag++){
  	pagusr_table[pag].entry = 0;
  	pagusr_table[pag].bits.pbase_addr = first_ph_page;
  	pagusr_table[pag].bits.user = 1;
  	pagusr_table[pag].bits.rw = 1;
  	pagusr_table[pag].bits.present = 1;
	first_ph_page++;
  }
}

/* Writes on CR3 register producing a TLB flush */
void set_cr3()
{
 	asm volatile("movl %0,%%cr3": :"r" (dir_pages));
}

/* Macros for reading/writing the CR0 register, where is shown the paging status */
#define read_cr0() ({ \
         unsigned int __dummy; \
         __asm__( \
                 "movl %%cr0,%0\n\t" \
                 :"=r" (__dummy)); \
         __dummy; \
})
#define write_cr0(x) \
         __asm__("movl %0,%%cr0": :"r" (x));
         
/* Enable paging, modifying the CR0 register */
void set_pe_flag()
{
  unsigned int cr0 = read_cr0();
  cr0 |= 0x80000000;
  write_cr0(cr0);
}

/* Associates logical page 'page' with physical page 'frame' */
void set_ss_pag(unsigned page,unsigned frame)
{
	pagusr_table[page].entry=0;
	pagusr_table[page].bits.pbase_addr=frame;
	pagusr_table[page].bits.user=1;
	pagusr_table[page].bits.rw=1;
	pagusr_table[page].bits.present=1;

}
/* Initializes paging an the process 0 address space */
int init_mm()
{
	int first_ph;
  init_table_pages();
	init_frames();
	first_ph = initialize_P0_frames();
  set_user_pages( first_ph );
  init_dir_pages();
  set_cr3();
  set_pe_flag();
  return first_ph;
}
/***********************************************/
/************** SEGMENTATION MANAGEMENT ********/
/***********************************************/
void setGdt()
{
  /* Configure TSS base address, that wasn't initialized */
  gdt[KERNEL_TSS>>3].lowBase = lowWord((DWord)&(tss));
  gdt[KERNEL_TSS>>3].midBase  = midByte((DWord)&(tss));
  gdt[KERNEL_TSS>>3].highBase = highByte((DWord)&(tss));

  gdtR.base = (DWord)gdt;
  gdtR.limit = 256 * sizeof(Descriptor);

  set_gdt_reg(&gdtR);
}

/***********************************************/
/************* TSS MANAGEMENT*******************/
/***********************************************/
void setTSS()
{
  tss.PreviousTaskLink   = NULL;
  tss.esp0               = KERNEL_ESP;
  tss.ss0                = __KERNEL_DS;
  tss.esp1               = NULL;
  tss.ss1                = NULL;
  tss.esp2               = NULL;
  tss.ss2                = NULL;
  tss.cr3                = NULL;
  tss.eip                = 0;
  tss.eFlags             = INITIAL_EFLAGS; /* Enable interrupts */
  tss.eax                = NULL;
  tss.ecx                = NULL;
  tss.edx                = NULL;
  tss.ebx                = NULL;
  tss.esp                = USER_ESP;
  tss.ebp                = tss.esp;
  tss.esi                = NULL;
  tss.edi                = NULL;
  tss.es                 = __USER_DS;
  tss.cs                 = __USER_CS;
  tss.ss                 = __USER_DS;
  tss.ds                 = __USER_DS;
  tss.fs                 = NULL;
  tss.gs                 = NULL;
  tss.LDTSegmentSelector = KERNEL_TSS;
  tss.debugTrap          = 0;
  tss.IOMapBaseAddress   = NULL;

  set_task_reg(KERNEL_TSS);
}

 
/* Initializes the ByteMap of free physical pages.
 * The kernel pages are marked as used */
int init_frames( void )
{
    int i;
    /* Mark pages as Free */
    for (i=0; i<TOTAL_PAGES; i++) {
        phys_mem[i] = FREE_FRAME;
    }
    /* Mark kernel pages as Used */
    for (i=0; i<NUM_PAG_KERNEL; i++) {
        phys_mem[i] = USED_FRAME;
    }
    return 0;
}

/* initialize_P0_frames - Initializes user code frames and user data frames for Initial Process P0.
 * Returns the First Physical frame for P0 */
int initialize_P0_frames(void)
{
    int i;
    /* Mark User Code pages as Used */
    for (i=0; i<NUM_PAG_CODE; i++) {
        phys_mem[NUM_PAG_KERNEL+i] = USED_FRAME;
    }
    /* Mark User Data pages as Used */
    for (i=0; i<NUM_PAG_DATA; i++) {
        phys_mem[NUM_PAG_KERNEL+NUM_PAG_CODE+i] = USED_FRAME;
    }
    return NUM_PAG_KERNEL;
}
/* alloc_frames - Search 'nframes' consecutive pages and marks them as USED_FRAME. 
 * Returns the initial frame number or -1 in case not enough consecutive pages available. */

int alloc_frames( int nframes )
{
int c, i,encontrado,principio;
i=FRAME_DATA_P0; //empezamos a buscar a partir de la pagina del P0
encontrado = Falso;
c=0;

while ( (i<TOTAL_PAGES) && (encontrado==Falso)){
	if (phys_mem[i]==FREE_FRAME) c++;	
	else c=0;
	if (c==nframes) encontrado=Cierto;
	else i++;	
}
 
if (encontrado == Falso) return -1;

else {
principio = i-nframes+1;
	
	while (c>=0){
	phys_mem[i]=USED_FRAME;
	i--;c--;
	}
}
return principio;
}

/* free_frames - Mark as FREE_FRAME 'nframes' consecutive pages from the initial  'frame'.*/
void free_frames( unsigned int frame, int nframes )
{
      int i = frame;
  while (i < (frame+nframes))
	{
		phys_mem[i]=FREE_FRAME;
		i++;
	} 
}

void del_ss_page (unsigned pagina_logica)
{
	pagusr_table[pagina_logica].entry=0;

}

//copia temporal de las paginas de usuario en la primera posicion libre
//mmu=pagusr_table
int cp_tmp(){
	int i = PAG_LOG_INIT_DATA_P0;
	
	while (pagusr_table[i].bits.present == 1) i++;/*buscamos una zona libre*/

	copy_data(&pagusr_table[PAG_LOG_INIT_DATA_P0],&pagusr_table[i],(i-PAG_LOG_INIT_DATA_P0)*4);
	
	return i;
}

void cp_datos_padre(int frame_h){

int i,j;
i=cp_tmp();
j = i;

/*hacemos un set_ss_pag de las paginas donde hemos copiado los datos temporalmente para asociar las paginas logicas a las paginas fisicas*/
   while (pagusr_table[i].bits.present == 1 )
   {
    	set_ss_pag(i,frame_h);
	i++; 		
	frame_h++;
   }
/*copiamos los datos de usuario (j*PAGE_SIZE = conversion del numero de pagina a la direccion de la pagina) */
copy_data((void *)L_DATA_USER_START, (void *)(j*PAGE_SIZE),(i-j)*PAGE_SIZE);
  i--;

   while (i >= j) //eliminem la copia temporal
   {
	del_ss_page(i);
	i--;
   }
  set_cr3(); //flush TLB



}

void actualitzar_TSS(DWord esp0)
{
   tss.esp0 = esp0;	
}

void actualitzar_TP(int frame)
{
   int i = PAG_LOG_INIT_DATA_P0;	 
  
   while(pagusr_table[i].bits.present == 1)	
   {	
     set_ss_pag(i,frame);
   	frame++;
   	i++;
   }
   set_cr3(); //flush TLB
}

