
#include <execinfo.h>
#include <string>
#include <malloc.h>

#include "utility.h"

const size_t MAX_TRACE_FUNC_NAME_LEN = 1024;

/*Attension: -rdynamic option is essential*/
/*get backtrace*/
void MEMUTIL::get_callerfunc_addr(std::vector<std::string>& vec_backtrace)
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

bool MEMUTIL::is_stl_memory_operate(const std::string& str)
{
    static std::vector<std::string> stl_memory_oper_black_list ;
    
    if (stl_memory_oper_black_list.empty())
    {
        stl_memory_oper_black_list.push_back("9__gnu_cxx13new_allocator");
        stl_memory_oper_black_list.push_back("4_Rep9_S_create");
    }

    std::vector <std::string>::const_iterator iter = stl_memory_oper_black_list.begin();
    for (; iter != stl_memory_oper_black_list.end(); iter++)
    {
        size_t pos = str.find(*iter);
        if( std::string::npos != pos)
        {
            return true;
        }
    }

    return false;
}

bool MEMUTIL::is_stl_backtrace(const std::vector<std::string>& vec)
{
    std::vector<std::string>::const_iterator iter = vec.begin();
    for (; iter != vec.end(); ++iter)
    {
        if (is_stl_memory_operate(*iter))
        {
            return true;
        }
    }

    return false;
}

