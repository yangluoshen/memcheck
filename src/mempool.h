#ifndef _FIZZ_MEMPOOL_H
#define _FIZZ_MEMPOOL_H
#include <climits>

typedef unsigned long long ULLONG;

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
            //todo: total size to large
        }
        else
        {
            m_total_alloc_size += size;
        }
    }
     
public:
    void* m_addr;
    static ULLONG m_total_alloc_size;
    static ULLONG m_total_free_size;
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


#endif /*_FIZZ_MEMPOOL_H*/

