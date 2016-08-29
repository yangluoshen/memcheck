#include <malloc.h>
#include <stdlib.h>
//#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "loghandle.cpp"
#include "memutil.cpp"

const size_t MAX_LEAK_RECORD_NUM = 1000;

struct AddrInfo
{
    std::string file_name;
    std::string line_no;
    std::string func_name;
    std::vector<std::string> vec_backtrace;
    void* addr;
};

std::map<void*, AddrInfo> g_map_memory_addr;

//void __fizz_malloc_hook_init(void);
void get_caller_func(std::vector<std::string>& vec_backtrace, std::string& caller_name);

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
    //std::cout<<"malloc:"<< ptr << std::endl;
    if (ptr)
    {
        std::string caller_name;
        AddrInfo addrInfo;
        addrInfo.addr =  ptr;
        (void) get_caller_func(addrInfo.vec_backtrace, caller_name);
        addrInfo.func_name = caller_name;
        if (g_map_memory_addr.size() < MAX_LEAK_RECORD_NUM)
        {
            g_map_memory_addr.insert(std::make_pair(addrInfo.addr, addrInfo));
        }
    }
#endif

    (void) caller;
    __fizz_hook_init();
    return ptr;
}

static void __fizz_mempool_destroy()
{
    __fizz_hook_restore();
    //std::cout << "memory pool size:"<< g_map_memory_addr.size()<< std::endl;
#if defined __PRIND_LOG
    LogHeadInfo head_info;
    head_info.leak_sum = g_map_memory_addr.size();
    LogHandle::get_instance().log_head_info(head_info);

    std::string line("--------------------");
    std::map<void*, AddrInfo>::iterator iter = g_map_memory_addr.begin();
    for (; iter != g_map_memory_addr.end(); ++iter)
    {
        LogHandle::get_instance().log_print(line);
        //std::cout << "addr:"<< iter->first<<",caller:"<<iter->second.func_name << std::endl;
        std::vector<std::string>::iterator subIter = iter->second.vec_backtrace.begin();
        for(; subIter != iter->second.vec_backtrace.end(); ++ subIter)
        {
            std::string leak_info(*subIter);
            LogHandle::get_instance().log_debug(leak_info);
        }
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


