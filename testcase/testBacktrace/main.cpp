#include <stdio.h>
#include <execinfo.h>

void print_ret_backtrace()
{
    void* p_ret = __builtin_return_address(0); 
    printf("ret:   %p\n", p_ret);
    p_ret = __builtin_return_address(1); 
    printf("ret:   %p\n", p_ret);
    p_ret = __builtin_return_address(2); 
    printf("ret:   %p\n", p_ret);
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

    void* p_ret = __builtin_return_address(2); 
    return 1;
}
