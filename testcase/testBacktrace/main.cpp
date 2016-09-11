#include <stdio.h>
#include <sstream>
#include <execinfo.h>
//#include <iostream>

using namespace std;

void print_ret_backtrace()
{
    void* p_ret = __builtin_return_address(0); 
    printf("ret:   %p\n", p_ret);
    p_ret = __builtin_return_address(1); 
    printf("ret:   %p\n", p_ret);
    p_ret = __builtin_return_address(2); 
    printf("ret:   %p\n", p_ret);
}


string get_caller_ip()
{
    void* p_ret = __builtin_return_address(0);
    ostringstream oss("");
    oss << p_ret;
    
    return oss.str();
}

void coo()
{
    //void* p_caller = __builtin_frame_address(1);
    //printf("caller:%p\n", p_caller);

    print_ret_backtrace();

    return;
}

void doo()
{
    coo();
}

void foo ()
{
    doo();
}

int main(int argc, char** argv)
{
    //foo();

    get_caller_ip();
    //void* p_ret = __builtin_return_address(0); 
    //cout << get_caller_ip() << endl;

    return 1;
}
