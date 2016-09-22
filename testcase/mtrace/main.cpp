#include <stdio.h>
//#include "util.h"
#include "memtrace.h"

class A
{
public:
    void alloc()
    {
        int* a = new int[10];
        delete[] a;
        

        //doo();
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
    A* aa = new A[10];
    delete[] aa;

    delete a;
    
    return 0;
}
