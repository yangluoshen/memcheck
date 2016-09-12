#ifndef _MEMCHECK_H
#define _MEMCHECK_H

void __fizz_mempool_init();
void (* __malloc_initialize_hook) (void) =  __fizz_mempool_init;

#endif
