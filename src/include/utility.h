
#ifndef __UTILITY_H
#define __UTILITY_H

#include <vector>
namespace MEMUTIL{

void get_callerfunc_addr(std::vector<std::string>& vec_backtrace);
bool is_stl_backtrace(const std::vector<std::string>& vec);
bool is_stl_memory_operate(const std::string& str);

}


#endif
