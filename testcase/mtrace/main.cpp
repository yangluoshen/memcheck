#include <stdio.h>
#include "memtrace.h"
//#include "common.h"

//#define new new(__FILE__, __LINE__, __FUNCTION__)

extern void moo();
extern void uoo();

class A
{
public:
    void alloc()
    {
//        int* a = new int[10];
//        delete[] a;
        

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
    uoo();
    moo();
    
    //A* a = new A;
    
    //a->alloc();
    //A* aa = new A[10];
    //delete[] aa;

    //delete a;
    
    return 0;
}
