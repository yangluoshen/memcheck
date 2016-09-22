#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <string>
#include <mcheck.h>

namespace{

class AllocatedMemInfo
{
public:
    explicit AllocatedMemInfo(void* ptr, size_t size, std::string file_name, int line_no, const char* func_name)
    :m_pointer(ptr), m_size(size), m_line_no(line_no),m_file_name(file_name), m_func_name(func_name)
    {
    }

    explicit AllocatedMemInfo(){}

    virtual ~AllocatedMemInfo(){};

    void format(char* buf)
    {
        sprintf(buf, "%-20s  %-10d  %-30s  %-10lu\n", \
                m_file_name.c_str(), m_line_no, m_func_name.c_str(), m_size);
    }

private:
    void * m_pointer;
    size_t m_size;
    int m_line_no;
    std::string m_file_name;
    std::string m_func_name;
};

static std::map<void*, AllocatedMemInfo> g_allocated_mem_info_map;
}

void before_main_procedure() __attribute__((constructor));
void exit_main_procedure();


void* operator new(size_t size, const char* file, int line, const char* func_name)
{
    void* ptr = malloc(size);
    if (ptr)
    {
        std::string str_file(file);
        AllocatedMemInfo mem_info(ptr, size, str_file, line, func_name);
        g_allocated_mem_info_map.insert(std::make_pair(ptr, mem_info));
    }
    
    return ptr;
}

void* operator new[](size_t size, const char* file, int line, const char* func_name)
{
    void* ptr = malloc(size);
    if (ptr)
    {
        std::string str_file(file);
        AllocatedMemInfo mem_info(ptr, size, str_file, line, func_name);
        g_allocated_mem_info_map.insert(std::make_pair(ptr, mem_info));
    }
    
    return ptr;
}

#define new new(__FILE__, __LINE__, __FUNCTION__)

void operator delete(void* ptr) throw()
{
    std::map<void*, AllocatedMemInfo>::const_iterator mem_iter = g_allocated_mem_info_map.find(ptr);
    if (mem_iter != g_allocated_mem_info_map.end())
    {
        g_allocated_mem_info_map.erase(ptr);
    }

    free(ptr);
    return ;
}

void operator delete[](void* ptr) throw()
{
    std::map<void*, AllocatedMemInfo>::const_iterator mem_iter = g_allocated_mem_info_map.find(ptr);
    if (mem_iter != g_allocated_mem_info_map.end())
    {
        g_allocated_mem_info_map.erase(ptr);
    }

    free(ptr);
    return ;
}

void before_main_procedure()
{
    setenv("MALLOC_TRACE", "trace.log", 1);
    mtrace();

    atexit(exit_main_procedure);
    return;
}

void exit_main_procedure() 
{
    FILE* fp_out = fopen("trace.report", "w");
    if (!fp_out)
    {
        return;
    }

    fprintf(fp_out, "Note: This file only records memory details which is alloced by \"new\" but never \"delete\"\n\n");
    fprintf(fp_out, "%-20s  %-10s  %-30s  %-10s\n\n", "File", "Line", "Function", "Size");

    for (std::map<void*, AllocatedMemInfo>::iterator mem_iter = g_allocated_mem_info_map.begin(); mem_iter != g_allocated_mem_info_map.end(); ++mem_iter)
    {
         char buf[256] = {0};
         mem_iter->second.format(buf);

         fprintf(fp_out, "%s", buf);
    }

    fclose(fp_out);
    return;
}

