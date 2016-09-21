#include <stdio.h>
#include "memtrace.h"

class A
{
public:
    void alloc()
    {
        new int[10];
    }

private:
    int m_data;

};

void foo ()
{
    new char;
}

int main(void)
{
    foo();
    
    A* a = new A;
    
    a->alloc();
    
    return 0;
}
