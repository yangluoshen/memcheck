#include <execinfo.h>
#include <sstream>

#include <vector>
#include <string>
#include <malloc.h>

#define CALLER_FUNC_INDEX  1
const size_t MAX_TRACE_FUNC_NAME_LEN = 1024;

std::string get_callerfunc_addr()
{
    void *p_ret = __builtin_return_address(CALLER_FUNC_INDEX);
    std::ostringstream oss("");
    oss << p_ret;
    
    return oss.str();
}

/*Attension: -rdynamic option is essential*/
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

    // symbol_list malloced in backtrace_symbols()
    free(symbol_list);
    symbol_list = NULL;
    return;
}

