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
 *     2. -lpthread is essential
 *
 * */

#include <fstream>
#include <sstream>
#include <ctime>
#include <unistd.h>

#include <execinfo.h>
#include <vector>
#include <string>

#include <stdlib.h>
#include <malloc.h>
#include <iostream>
#include <map>
#include <climits>

#include <pthread.h>

#include "mempool.h"

typedef unsigned long long ULLONG;

const size_t MAX_LEAK_RECORD_NUM = 10000;

/**********************Lock begin ****************************************/

namespace
{

class CMutexLock 
{
public:

    CMutexLock();

    ~CMutexLock();

    void lock();

    void unlock();

private:

    pthread_mutex_t m_mutex;
};

CMutexLock::CMutexLock()
{
    pthread_mutex_init(&this->m_mutex, NULL);
}

CMutexLock::~CMutexLock()
{
    pthread_mutex_destroy(&this->m_mutex);
}

void CMutexLock::lock()
{
    pthread_mutex_lock(&m_mutex);

    return;
}

void CMutexLock::unlock()
{
    pthread_mutex_unlock(&m_mutex);

    return;
}

template <typename LOCK>
class FGuard
{
public:

    FGuard(LOCK& lock);

    ~FGuard();

private:

    FGuard(const FGuard&);

    FGuard& operator=(const FGuard&);

private:

    LOCK& m_lock;
};

template <typename LOCK>
inline
FGuard<LOCK>::FGuard(LOCK& lock)
    :m_lock(lock)
{
    m_lock.lock();
}

template <typename LOCK>
inline
FGuard<LOCK>::~FGuard()
{
    m_lock.unlock();
}

typedef FGuard<CMutexLock> MutexGuard;

}


/**********************Lock end ****************************************/

/**********************Loghandler begin******************************************/
namespace
{

enum 
{
    FILE_TYPE_ALLOC = 0,
    FILE_TYPE_FREE,
    FILE_TYPE_BUTT
};

struct LogHeadInfo
{
    size_t leak_sum;

    std::string description;
};

class LogHandle
{
private:

    std::string log_file_name;

    enum log_level
    {
       LOG_INFO,
       LOG_DEBUG,
       LOG_ERROR,
       LOG_CRITICAL,
       LOG_BUTT
    };

public:

    static LogHandle& get_instance(unsigned int type);

    void log_debug(const std::string& content);

    void log_error(const std::string& content);

    void log_head_info(LogHeadInfo head_info);
    
    void log_print(const std::string& log);
 
private:

    LogHandle(unsigned int type);

    virtual ~LogHandle(){}; 

    void log_consist(enum log_level level, const std::string& content, std::string& log);

    void get_logname(std::string& log_name);

    void get_systime(std::string& now);
    
    unsigned int m_type;
};

LogHandle::LogHandle(unsigned int type)
:m_type(type)
{
}

LogHandle& LogHandle::get_instance(unsigned int type)
{
    switch(type)
    {
        case FILE_TYPE_ALLOC:
            static LogHandle s_alloc_instance(type);
            return s_alloc_instance;
        case FILE_TYPE_FREE:
            static LogHandle s_free_instance(type);
            return s_free_instance;
        default:
            static LogHandle s_unknown_instance(FILE_TYPE_BUTT);
            return s_unknown_instance;
    }
}

void LogHandle::log_consist(enum log_level level, const std::string& content, std::string& log)
{
    (void)get_systime(log);

    switch(level)
    {
        case LOG_INFO:
            log.append(":[I] ");
            break;

        case LOG_DEBUG:
            log.append(":[D] ");
            break;
            
        case LOG_ERROR:
            log.append(":[E] ");
            break;

        default:
            break;        

    }

    log.append(content);

    return;
}

void LogHandle::log_print(const std::string& log)
{
    if (log_file_name.empty())
    {
        get_logname(this->log_file_name); 
    }
    
    std::ofstream fout(this->log_file_name.c_str(), std::ofstream::out | std::ofstream::app);
    if (!fout.is_open())
    {
        return;
    }

    std::string log_buf(log);

    log_buf.append("\r\n");

    fout.write(log_buf.c_str(), log_buf.size());

    fout.close();

    return;
}

void LogHandle::log_debug(const std::string& content)
{
    std::string log;
    log_consist(LOG_DEBUG, content, log);   
    log_print(log);
    return;
}

void LogHandle::log_error(const std::string& content)
{
    std::string log;
    log_consist(LOG_ERROR, content, log);
    log_print(log);
    return;
}

void LogHandle::get_logname(std::string& log_name)
{
    pid_t pid = getpid();
    std::ostringstream os("");
    os << pid;
    os << ".log";

    log_name.append("fizz.memcheck.");
    if (FILE_TYPE_ALLOC == m_type)
    {
        log_name.append("alloc.");
    }
    else if (FILE_TYPE_FREE == m_type)
    {
        log_name.append("free.");
    }
    else
    {
        log_name = "";
        return;
    }

    log_name.append(os.str());

    return;
}

void LogHandle::get_systime(std::string& now)
{
    time_t now_time;
    now_time = time(NULL);

    char time_buf[20] = {0}; //buffer of the string time ,20 is enough
    strftime(time_buf, sizeof (time_buf), "%H:%M:%S", localtime(&now_time));
    
    now.assign(time_buf);

    return;
}

void LogHandle::log_head_info(LogHeadInfo head_info)
{
    std::string output("/**************************\n");
    output.append("* leak num: ");

    std::ostringstream num_of_leak("");
    num_of_leak << head_info.leak_sum;
    output.append(num_of_leak.str());

    output.append("\n************************/\n\n");

    log_print(output);
}


}

/**********************Loghandler end******************************************/

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
namespace MEMUTIL
{

const size_t MAX_TRACE_FUNC_NAME_LEN = 1024;

/*Attension: -rdynamic option is essential*/
/*get backtrace*/
void get_callerfunc_addr(std::vector<std::string>& vec_backtrace)
{
    //Attention: while you use backtrace_symbols, the compile option "-rdynamic" is essential
    void* trace_func_buf[MAX_TRACE_FUNC_NAME_LEN] = {0};
    size_t trace_func_num = 0;
    trace_func_num = backtrace(trace_func_buf, MAX_TRACE_FUNC_NAME_LEN);

    char** symbol_list;
    symbol_list = backtrace_symbols(trace_func_buf, trace_func_num);
    
    vec_backtrace.clear();
    for (size_t i = 0; i < trace_func_num; ++i)
    {
        std::string func_name(symbol_list[i]);
        vec_backtrace.push_back(func_name);
    }

    // symbol_list alloced in backtrace_symbols()
    free(symbol_list);
    symbol_list = NULL;

    return;
}

/*write an addrinfo to specific file*/
void print_addrinfo_in_format(const LeakCheckAddrInfo& addrinfo)
{
    std::vector <std::string>::const_iterator iter = addrinfo.m_vec_backtrace.begin();

    for (; iter != addrinfo.m_vec_backtrace.end(); ++iter)
    {
        LogHandle::get_instance(FILE_TYPE_ALLOC).log_debug(*iter);
    }

    return;
}

/*write all addrinfos in map to specific file*/
void dump_memory_record(const std::map<size_t, LeakCheckAddrInfo>& mem_records)
{
    LogHeadInfo head_info;
    head_info.leak_sum = mem_records.size();

    MutexGuard guard(g_file_dump_mutex);

    LogHandle::get_instance(FILE_TYPE_ALLOC).log_head_info(head_info);
    
    std::string line("------------------------");
    std::map <size_t, LeakCheckAddrInfo>::const_iterator iter = mem_records.begin();
    for (; iter != mem_records.end(); ++iter)
    {
        LogHandle::get_instance(FILE_TYPE_ALLOC).log_print(line);
        print_addrinfo_in_format(iter->second);
    }

    LogHandle::get_instance(FILE_TYPE_ALLOC).log_print(line);

    return;
}

void* dump_map(void* para)
{
    /* TODO: Lock */
    {
        MutexGuard guard(g_leak_map_mutex);
        MEMUTIL::dump_memory_record(g_leak_addrinfo_map);
        g_leak_addrinfo_map.clear();
    }
}

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

        if(this->m_aquire_count > 0)
        {
            this->m_switch = true;
            return;
        }

        this->m_switch = false;
    }

    pthread_t dump_file_thread;
    pthread_create(&dump_file_thread, NULL, MEMUTIL::dump_map, NULL);
    
    return;
}




