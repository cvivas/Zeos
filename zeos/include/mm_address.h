#include <segment.h>

#ifndef MM_ADDRESS_H
#define MM_ADDRESS_H
#define ENTRY_DIR_PAGES       0

#define TOTAL_PAGES 1024
#define NUM_PAG_KERNEL 256

#define L_DATA_USER_START (L_USER_START + NUM_PAG_CODE*PAGE_SIZE) // @ logica de los datos usuario

#define PAG_LOG_INIT_CODE_P0 (L_USER_START>>12)
#define FRAME_INIT_CODE_P0 (PH_USER_START>>12)
#define NUM_PAG_CODE 8
#define FRAME_INIT_TEMP_P0 (NUM_PAG_CODE * PAGE_SIZE)
#define PAG_LOG_INIT_DATA_P0 (PAG_LOG_INIT_CODE_P0+NUM_PAG_CODE)
#define NUM_FRAME_DATA_P0 (FRAME_INIT_TEMP_P0+FRAME_INIT_CODE_P0) //num frame on comença dades+pila del p0
#define FRAME_DATA_P0 (PAG_LOG_INIT_CODE_P0 + NUM_PAG_CODE)

#define NUM_PAG_DATA 2
#define PAGE_SIZE 0x1000

//NO SE COMO HACER LA MACRO :S
#define MIMACRO(_X) _X>>12
#endif

