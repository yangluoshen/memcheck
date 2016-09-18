#include <stdio.h>
#include <iostream>

#include <malloc.h>

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
    cout << print_count++ << ":ENTER "<< __FUNCTION__ << endl;
    cout << print_count++ << ":alloc size :"<< size << endl;
    cout << print_count++ << ":file name  :"<< file << endl;
    cout << print_count++ << ":line no    :" << line << endl;
    cout << print_count++ << ":in function:" << __FUNCTION__<< endl;

    void * ptr = malloc(size);

    cout << print_count++ << ":EXIT " << __FUNCTION__ << endl;
    return ptr;
}

void *operator new[](size_t size, const char* file, int line)
{
    cout << print_count++ << ":ENTER "<< __FUNCTION__ << endl;
    cout << print_count++ << ":alloc size :"<< size << endl;
    cout << print_count++ << ":file name  :"<< file << endl;
    cout << print_count++ << ":line no    :" << line << endl;

    void* ptr = malloc(size);
    cout << print_count++ << ":EXIT " << __FUNCTION__ << endl;
    return ptr;
}

void operator delete(void* ptr)
{
    cout << print_count++ << ":ENTER "<< __FUNCTION__ << endl;
    free(ptr);
    ptr = NULL;

    cout << print_count++ << ":EXIT " << __FUNCTION__ << endl;
    return ;
}

void operator delete[](void* ptr)
{
    cout << print_count++ << ":ENTER "<< __FUNCTION__ << endl;
    free(ptr);
    ptr = NULL;

    cout << print_count++ << ":EXIT " << __FUNCTION__ << endl;
    return ;
}
#define new new(__FILE__, __LINE__)

void foo()
{
    B* b = new B[2];
    //B* b = new B;

    //delete b;
    
    delete[]  b;

    return;
}

int main(void)
{
    cout <<print_count++ << ":sizeof (B) = " << sizeof (B) << endl;
    foo (); 
    return 0;
}
