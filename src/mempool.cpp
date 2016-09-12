#include <malloc.h>
#include <stdlib.h>
//#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "loghandle.h"
#include "mempool.h"
#include "memutil.h"

const size_t MAX_LEAK_RECORD_NUM = 10000;
ULLONG AddrInfo::m_total_alloc_size = 0;
ULLONG AddrInfo::m_total_free_size = 0;

std::map<void*, LeakCheckAddrInfo> g_map_leak_check;

std::string get_callerfunc_addr();
void get_callerfunc_addr(std::vector<std::string>& vec_backtrace);
void insert_addrinfo_to_map(const LeakCheckAddrInfo& addrinfo);
void print_addrinfo_in_format(const LeakCheckAddrInfo& addrinfo);
//void get_caller_func(std::vector<std::string>& vec_backtrace, std::string& caller_name);

static void (*old_free)(void *ptr, const void *caller);
static void *(*old_malloc)(size_t size, const void *caller);

static void __fizz_free(void *ptr, const void *caller);
static void *__fizz_malloc(size_t size, const void *caller);

static void __fizz_hook_back()
{
    old_malloc = __malloc_hook;
    old_free = __free_hook;
}

static void __fizz_hook_init()
{
    __malloc_hook = __fizz_malloc;
    __free_hook = __fizz_free;
}

static void __fizz_hook_restore()
{
    __malloc_hook = old_malloc;
    __free_hook = old_free;
}

static void __fizz_free(void *ptr, const void *caller) {
    __fizz_hook_restore();   // hook restore is essential, otherwise it will dead loop 

#if defined __MEMPOOL_RUNTIME_DETAIL
    //std::cout << "free  :"<< ptr << std::endl;
#endif

    std::map<void*, LeakCheckAddrInfo>::iterator iter = g_map_leak_check.find(ptr);
    if (iter != g_map_leak_check.end())
    {
        g_map_leak_check.erase(ptr);
    }

    free(ptr);
    (void) caller;
    __fizz_hook_init();
    return;
}

static void *__fizz_malloc(size_t size, const void *caller)
{
     void *ptr = NULL;
    __fizz_hook_restore();        // hook restore is essential, otherwise it will dead loop
    ptr = malloc(size);
#if defined __MEMPOOL_RUNTIME_DETAIL
    if (ptr)
    {
        LeakCheckAddrInfo addrinfo;

        addrinfo.m_addr = ptr;
        addrinfo.get_callerfunc_addr();
        addrinfo.set_alloc_size(size);
        
        insert_addrinfo_to_map(addrinfo);
    }
#endif

    (void) caller;
    __fizz_hook_init();
    return ptr;
}

static void __fizz_mempool_destroy()
{
    __fizz_hook_restore();

#if defined __PRINT_LOG
    LogHeadInfo head_info;
    head_info.leak_sum = g_map_leak_check.size();
    LogHandle::get_instance().log_head_info(head_info);

    std::string line("--------------------");
    std::map<void*, LeakCheckAddrInfo>::iterator iter = g_map_leak_check.begin();
    for (; iter != g_map_leak_check.end(); ++iter)
    {
        LogHandle::get_instance().log_print(line);
        print_addrinfo_in_format(iter->second);
    }
#endif /*__PRINT_LOG */
    return ;
}

void __fizz_mempool_init()
{
    __fizz_hook_back();
    __fizz_hook_init();
    atexit(__fizz_mempool_destroy);

    return ;
}

/*
void __fizz_malloc_hook_init(void)
{
    void (* __malloc_initialize_hook) (void) =  __fizz_mempool_init;

    return ;
}
*/

void operator delete(void* p)
{
    free(p);
}

void operator delete[](void* p)
{
    free(p);
}


void insert_addrinfo_to_map(const LeakCheckAddrInfo& addrinfo)
{
    if (g_map_leak_check.size() < MAX_LEAK_RECORD_NUM)
    {
        g_map_leak_check.insert(std::make_pair(addrinfo.m_addr, addrinfo));
    }

    return;
}

void print_addrinfo_in_format(const LeakCheckAddrInfo& addrinfo)
{
#ifndef __USE_RDYNAMIC_OPTION
    LogHandle::get_instance().log_debug(addrinfo.m_caller_ip);
#else
    std::vector <std::string>::const_iterator iter = addrinfo.m_vec_backtrace.begin();
    for (; iter != addrinfo.m_vec_backtrace.end(); ++ iter)
    {
        LogHandle::get_instance().log_debug(*iter);
    }
#endif
    return;
}

void LeakCheckAddrInfo::get_callerfunc_addr(void)
{
    (void)MEMUTIL::get_callerfunc_addr(this->m_vec_backtrace);
    return ;
}

