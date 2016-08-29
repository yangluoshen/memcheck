#include <execinfo.h>
#include <vector>
#include <string>
#include <malloc.h>

const size_t MAX_TRACE_FUNC_NAME_LEN = 1000;
const size_t CALLER_FUNC_INDEX = 3;

void get_caller_func(std::vector<std::string>& vec_backtrace, std::string& caller_name)
{
    //Attention: while you use backtrace_symbols, the compile option "-rdynamic" is essential
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
        std::string func_name(symbol_list[i]);
        vec_backtrace.push_back(func_name);
    }

    // symbol_list malloced in backtrace_symbols()
    free(symbol_list);
    return;
}

