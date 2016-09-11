#include <malloc.h>
#include <stdlib.h>
//#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "loghandle.h"
//#include "memutil.h"

const size_t MAX_LEAK_RECORD_NUM = 10000;

#if defined __USE_RDYNAMIC_OPTION
struct AddrInfo
{
    void* addr;
    std::vector<std::string> vec_backtrace;
};
#else
struct AddrInfo
{
    void* addr;
    std::string caller_ip;
};
#endif

std::map<void*, AddrInfo> g_map_memory_addr;

std::string get_callerfunc_addr();
void get_callerfunc_addr(std::vector<std::string>& vec_backtrace);
void insert_addrinfo_to_map(const AddrInfo& addrinfo);
void print_addrinfo_in_format(const AddrInfo& addrinfo);
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

    std::map<void*, AddrInfo>::iterator iter = g_map_memory_addr.find(ptr);
    if (iter != g_map_memory_addr.end())
    {
        g_map_memory_addr.erase(ptr);
        //std::cout << "erase addr successfully:" << ptr << std::endl;
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
        AddrInfo addrinfo;
        addrinfo.addr = ptr;
#if defined __USE_RDYNAMIC_OPTION
        get_callerfunc_addr(addrinfo.vec_backtrace);
#else
        addrinfo.caller_ip = get_callerfunc_addr();
#endif
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
    head_info.leak_sum = g_map_memory_addr.size();
    LogHandle::get_instance().log_head_info(head_info);

    std::string line("--------------------");
    std::map<void*, AddrInfo>::iterator iter = g_map_memory_addr.begin();
    for (; iter != g_map_memory_addr.end(); ++iter)
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


void insert_addrinfo_to_map(const AddrInfo& addrinfo)
{
    if (g_map_memory_addr.size() < MAX_LEAK_RECORD_NUM)
    {
        g_map_memory_addr.insert(std::make_pair(addrinfo.addr, addrinfo));
    }

    return;
}

void print_addrinfo_in_format(const AddrInfo& addrinfo)
{
#ifndef __USE_RDYNAMIC_OPTION
    LogHandle::get_instance().log_debug(addrinfo.caller_ip);
#else
    std::vector <std::string>::const_iterator iter = addrinfo.vec_backtrace.begin();
    for (; iter != addrinfo.vec_backtrace.end(); ++ iter)
    {
        LogHandle::get_instance().log_debug(*iter);
    }
#endif
    return;
}

