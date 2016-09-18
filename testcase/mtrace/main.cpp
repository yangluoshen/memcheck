#include <stdio.h>
#include <iostream>

#include <malloc.h>
#include <mcheck.h>

static int print_count = 1;

using namespace std;

class A
{
public:
    explicit A()
    {
        cout << print_count++ << ":constructor A" << endl;    
    }

    virtual ~A()
    {
        cout << print_count ++ << ":destructor A" << endl;
    }
};

class B : public A
{
public:
    explicit B() {
        cout << print_count++ << ":constructor B" << endl;
    }

    ~B()
    {
        cout << print_count++  << ":destructor B" << endl;
    }
};

void *operator new(size_t size, const char* file, int line)
{
    void * ptr = malloc(size);
    return ptr;
}

void *operator new[](size_t size, const char* file, int line)
{
    void* ptr = malloc(size);
    return ptr;
}

#define new new(__FILE__, __LINE__)

void foo()
{
    B* b = new B;
    //delete[]  b;

    //B* b = new B;
    //delete b;
    //
    return;
}

int main(void)
{
    mtrace();
    foo (); 
    malloc(3);
    return 0;
}
