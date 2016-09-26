#ifndef __MEMTRACE_H
#define __MEMTRACE_H

#include <stdio.h>

void before_main_procedure() __attribute__((constructor(101)));
void exit_main_procedure();

void* operator new(size_t size, const char* file, int line, const char* func_name) throw();
//void* operator new(size_t size) throw(std::bad_alloc);
void* operator new[](size_t size, const char* file, int line, const char* func_name) throw();

//void operator delete(void* ptr) throw();
//void operator delete[](void* ptr) throw();



#define new new(__FILE__, __LINE__, __FUNCTION__)
#endif
