#include <stdio.h>
#include <iostream>
#include <sstream>

#include <malloc.h>
#include <mcheck.h>

#define CALLER_FUNC_INDEX 1

static int print_count = 1;

using namespace std;
string get_callerfunc_addr();

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

    cout << "caller:" << get_callerfunc_addr() <<endl;
    printf("ip:%p\n", ptr);

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

    B* bb = new B;

    A* a = new A;
    //delete[]  b;

    //B* b = new B;
    //delete b;
    return;
}

int main(void)
{
    mtrace();
    foo (); 
    int a = 1;
    int b = 2;
    malloc(3);
    return 0;
}

string get_callerfunc_addr()
{
    void *p_ret = __builtin_return_address(CALLER_FUNC_INDEX);
    std::ostringstream oss("");
    oss << p_ret;
    
    return oss.str();
}
