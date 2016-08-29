#include <malloc.h>
#include <stdlib.h>
//#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>

#include <ctime>

using namespace std;

const size_t MAX_TRACE_FUNC_NAME_LEN = 1000;
const size_t CALLER_FUNC_INDEX = 3;
const size_t MAX_LEAK_RECORD_NUM = 1000;

struct AddrInfo
{
    string file_name;
    string line_no;
    string func_name;
    vector<string> vec_backtrace;
    void* addr;
};

map<void*, AddrInfo> g_map_memory_addr;

//void __fizz_malloc_hook_init(void);
void get_caller_func(vector<string>& vec_backtrace, string& caller_name);

static void (*old_free)(void *ptr, const void *caller);
static void *(*old_malloc)(size_t size, const void *caller);

static void __fizz_free(void *ptr, const void *caller);
static void *__fizz_malloc(size_t size, const void *caller);


class LogHandle
{
private:
    string log_file_name;
    enum log_level
    {
       LOG_INFO,
       LOG_DEBUG,
       LOG_ERROR,
       LOG_CRITICAL,
       LOG_BUTT
    };


public:
    static LogHandle& get_instance();
    void log_debug(const string& content);
    void log_error(const string& content);
    void log_head_info();
    void log_print(const string& log);
 
private:
    LogHandle(){};
    virtual ~LogHandle(){}; 
    void log_consist(enum log_level level, const string& content, string& log);
    void get_logname(string& log_name);
    void get_systime(string& now);
   
};

LogHandle& LogHandle::get_instance()
{
    static LogHandle s_instance;
    return s_instance;
}

void LogHandle::log_consist(enum log_level level, const string& content, string& log)
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

void LogHandle::log_print(const string& log)
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

    string log_buf(log);
    log_buf.append("\r\n");

    fout.write(log_buf.c_str(), log_buf.size());

    fout.close();
    return;
}

void LogHandle::log_debug(const string& content)
{
    string log;
    log_consist(LOG_DEBUG, content, log);   
    log_print(log);
}

void LogHandle::log_error(const string& content)
{
    string log;
    log_consist(LOG_ERROR, content, log);
    log_print(log);
    return;
}

void LogHandle::get_logname(string& log_name)
{
    pid_t pid = getpid();
    ostringstream os("");
    os << pid;
    os << ".log";

    log_name.append("fizz.leak.");
    log_name.append(os.str());
    return;
}

void LogHandle::get_systime(string& now)
{
    time_t now_time;
    now_time = time(NULL);
    char time_buf[20] = {0}; //buffer of the string time ,20 is enough
    strftime(time_buf, sizeof (time_buf), "%H:%M:%S", localtime(&now_time));
    
    now.assign(time_buf);
    return;
}

void LogHandle::log_head_info()
{
    string head_info("/**************************\n");
    head_info.append("* leak num: ");

    ostringstream num_of_leak("");
    num_of_leak << g_map_memory_addr.size();
    head_info.append(num_of_leak.str());

    head_info.append("\n************************/\n\n");

    log_print(head_info);
}


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
    std::cout << "free  :"<< ptr << std::endl;
#endif

    map<void*, AddrInfo>::iterator iter = g_map_memory_addr.find(ptr);
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
#endif
    if (g_map_memory_addr.size() < MAX_LEAK_RECORD_NUM)
    {
        string caller_name;
        AddrInfo addrInfo;
        addrInfo.addr =  ptr;
        (void) get_caller_func(addrInfo.vec_backtrace, caller_name);
        addrInfo.func_name = caller_name;
        if (g_map_memory_addr.size() < MAX_LEAK_RECORD_NUM)
        {
            g_map_memory_addr.insert(make_pair(addrInfo.addr, addrInfo));
        }
        g_map_memory_addr.insert(make_pair(addrInfo.addr, addrInfo));
    }

    __fizz_hook_init();
    return ptr;
}

static void __fizz_mempool_destroy()
{
    __fizz_hook_restore();
    //std::cout << "memory pool size:"<< g_map_memory_addr.size()<< std::endl;
    LogHandle::get_instance().log_head_info();

    string line("--------------------");
    map<void*, AddrInfo>::iterator iter = g_map_memory_addr.begin();
    for (; iter != g_map_memory_addr.end(); ++iter)
    {
        LogHandle::get_instance().log_print(line);
        //std::cout << "addr:"<< iter->first<<",caller:"<<iter->second.func_name << std::endl;
        vector<string>::iterator subIter = iter->second.vec_backtrace.begin();
        for(; subIter != iter->second.vec_backtrace.end(); ++ subIter)
        {
            string leak_info(*subIter);
            LogHandle::get_instance().log_debug(leak_info);
        }
    }

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

void get_caller_func(vector<string>& vec_backtrace, string& caller_name)
{
    // while you use backtrace_symbols, the compile option "-rdynamic" is essential
    void* trace_func_buf[MAX_TRACE_FUNC_NAME_LEN] = {0};
    size_t trace_func_num = 0;
    trace_func_num = backtrace(trace_func_buf, MAX_TRACE_FUNC_NAME_LEN);

    char** symbol_list;
    symbol_list = backtrace_symbols(trace_func_buf, trace_func_num);
    
    if (trace_func_num > CALLER_FUNC_INDEX)
    {
        caller_name.assign(symbol_list[CALLER_FUNC_INDEX]);
    }

    vec_backtrace.clear();
    for (size_t i = 0; i < trace_func_num; ++i)
    {
        string func_name(symbol_list[i]);
        vec_backtrace.push_back(func_name);
    }

    // symbol_list malloced in backtrace_symbols()
    free(symbol_list);
    return;
}


