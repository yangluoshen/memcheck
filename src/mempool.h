
#ifndef _FIZZ_MEMPOOL_H
#define _FIZZ_MEMPOOL_H
//void __fizz_malloc_hook_init();
void __fizz_mempool_init();

void (* __malloc_initialize_hook) (void) =  __fizz_mempool_init;

//extern map<unsigned int, AddrInfo> g_map_memory_addr;
#endif /*_FIZZ_MEMPOOL_H*/

