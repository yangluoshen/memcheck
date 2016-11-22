/**
 * Decription: Dump memory infomation allocated or freed to specific files
 * Author: Fizz
 * Points:
 *     1. 2016-9-20  (Fizz): First draft.
 *     2. 2016-10-15 (Fizz): Make it thread safe.
 *     3. 2016-10-29 (Fizz): First version. 
 *
 * Note:
 *     1. mempool.h must be included by other file while using this cppfile
 *     2. -pthread is essential
 *
 * */

//#include <fstream>
//#include <sstream>
//#include <ctime>
//#include <unistd.h>

#include <vector>

#include <stdlib.h>
#include <malloc.h>
#include <map>
#include <climits>
#include <cstring>

#include "mempool.h"
#include "log.h"
#include "lock.h"
#include "utility.h"

typedef unsigned long long ULLONG;

const size_t MAX_LEAK_RECORD_NUM = 10000;


/**********************Address Info Declearation begin*************************************/

namespace
{

class AddrInfo
{
public:
    
    explicit AddrInfo():m_addr(0), m_alloc_size(0)
    {
    }
    
    size_t get_alloc_size(){return m_alloc_size;}

    void set_alloc_size(size_t size)
    {
        this->m_alloc_size = size;
        if (m_total_alloc_size > (ULLONG_MAX - size)) 
        {
            //TODO: total size to large
        }
        else
        {
            m_total_alloc_size += size;
        }
    }

    virtual ~AddrInfo(){};
     
public:

    size_t m_addr;

    static ULLONG m_total_alloc_size;

    //static ULLONG m_total_free_size;
    
protected:

    size_t m_alloc_size;

};

class LeakCheckAddrInfo: public AddrInfo
{
public:
    
    explicit LeakCheckAddrInfo()
    {
        m_addr = 0;
        m_vec_backtrace.clear();
    }
    
    void get_callerfunc_addr();
    
public:

    std::vector<std::string> m_vec_backtrace;
    
};


class BoundaryCheckAddrInfo: public AddrInfo
{
public:

    explicit BoundaryCheckAddrInfo()
    {
        m_addr = 0;
    }
};

ULLONG AddrInfo::m_total_alloc_size = 0;
//ULLONG AddrInfo::m_total_free_size = 0;

}
/**********************Address Info Declearation end******************************************/

// global declearation
namespace
{

CMutexLock g_leak_map_mutex;

CMutexLock g_aquire_switch_mutex; 

CMutexLock g_file_dump_mutex;

std::map<size_t, LeakCheckAddrInfo> g_leak_addrinfo_map;


}

/**********************Memutil begin******************************************/

/*write an addrinfo to specific file*/
void print_addrinfo_in_format(const LeakCheckAddrInfo& addrinfo)
{
    const size_t MAX_BACKTRACE_BUF_LEN = 1024 * 5;
    char trace_buf[MAX_BACKTRACE_BUF_LEN] = {0};

    std::vector <std::string>::const_iterator iter = addrinfo.m_vec_backtrace.begin();

    size_t trace_len = 0;
    for (; (iter != addrinfo.m_vec_backtrace.end()) && (trace_len + iter->size() + 2 < MAX_BACKTRACE_BUF_LEN); trace_len+=iter->size() + 2, ++iter)
    {
        strncat(trace_buf, iter->c_str(), iter->size());
        strcat(trace_buf, "\r\n");
    }

    LogHandle::get_instance(FILE_TYPE_ALLOC).log_print(trace_buf);
    return;
}

/*write all addrinfos in map to specific file*/
void dump_memory_record(const std::map<size_t, LeakCheckAddrInfo>& mem_records)
{

    MutexGuard guard(g_file_dump_mutex);

    std::string line("------------------------");
    std::map <size_t, LeakCheckAddrInfo>::const_iterator iter = mem_records.begin();
    for (; iter != mem_records.end(); ++iter)
    {
        // filter STL
        if (MEMUTIL::is_stl_backtrace(iter->second.m_vec_backtrace))
        {
            continue;
        }

        LogHandle::get_instance(FILE_TYPE_ALLOC).log_debug(line);
        print_addrinfo_in_format(iter->second);
    }

    //LogHandle::get_instance(FILE_TYPE_ALLOC).log_print(line);

    return;
}

void* dump_map(void* para)
{
    /* TODO: Lock */
    {
        MutexGuard guard(g_leak_map_mutex);
        dump_memory_record(g_leak_addrinfo_map);
        g_leak_addrinfo_map.clear();
    }

    return (void*) 0;
}

/**********************Memutil end******************************************/

/**********************Memory Record begin******************************************/
// leak records operate interface
namespace
{

class SMemoryRecordHandler
{
public:

    static SMemoryRecordHandler& getInstance()
    {
        static SMemoryRecordHandler s_instance;

        return s_instance;
    }

    void insert(const LeakCheckAddrInfo& addrinfo);

    void erase(size_t ptr);

private:

    SMemoryRecordHandler(){}
};

void SMemoryRecordHandler::insert(const LeakCheckAddrInfo& addrinfo)
{
    /*TODO:  Lock */
    MutexGuard guard(g_leak_map_mutex);
    if (g_leak_addrinfo_map.size()  < MAX_LEAK_RECORD_NUM)
    {
        g_leak_addrinfo_map.insert(std::make_pair(addrinfo.m_addr, addrinfo));
    }

    return;
}

void SMemoryRecordHandler::erase(size_t ptr)
{
    /*TODO:  Lock */
    MutexGuard guard(g_leak_map_mutex);

    std::map<size_t, LeakCheckAddrInfo>::const_iterator it = g_leak_addrinfo_map.find(ptr);
    if (it != g_leak_addrinfo_map.end())
    {
        g_leak_addrinfo_map.erase(ptr);
    }

    return;
}


}
/**********************Memory Record end***********************************************/

/**********************Hooks Declearation begin******************************************/
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

static void __fizz_free(void *ptr, const void *caller) 
{
    __fizz_hook_restore();   // hook restore is essential, otherwise it will dead loop 

    if (GMemoryRecordSwitch::getInstance().getSwitch())
    {
        SMemoryRecordHandler::getInstance().erase((size_t)ptr);
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

    if (GMemoryRecordSwitch::getInstance().getSwitch() && ptr) 
    {
        LeakCheckAddrInfo addrinfo;

        addrinfo.m_addr = (size_t)ptr;
        addrinfo.get_callerfunc_addr();
        addrinfo.set_alloc_size(size);

        SMemoryRecordHandler::getInstance().insert(addrinfo);
    }

    (void) caller;
    __fizz_hook_init();
    return ptr;
}


void __fizz_mempool_init()
{
    __fizz_hook_back();

    __fizz_hook_init();

    return ;
}

/**********************Hooks Declearation end******************************************/

void LeakCheckAddrInfo::get_callerfunc_addr(void)
{
    (void)MEMUTIL::get_callerfunc_addr(this->m_vec_backtrace);

    return ;
}

void GMemoryRecordSwitch::setSwitch(bool bSwitch)
{

    /* TODO: Lock */
    {
        MutexGuard guard(g_aquire_switch_mutex);
        if (bSwitch)
        {
            /* TODO: Lock */
            this->m_aquire_count ++;
        }
        else
        {
            this->m_aquire_count --;
        }

        if (this->m_aquire_count > 0)
        {
            this->m_switch = true;
            return;
        }

        this->m_switch = false;
    }

    pthread_t dump_file_thread;
    pthread_create(&dump_file_thread, NULL, dump_map, NULL);
    
    return;
}




