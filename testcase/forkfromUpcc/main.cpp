#include "memcheck.h"

#include "unistd.h"

class A
{
public:
    A () {}
    ~A(){}

};

void foo()
{
    SMiniCheck check;

    char* p = new char[1];

    char* p1 = new char[1];

    delete[] p;

    A* a = new A;

    return ;
}

int main()
{
    foo();

    sleep(1);

    return 1;

}

