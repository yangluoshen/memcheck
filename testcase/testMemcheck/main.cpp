#include <iostream>
#include "memcheck.h"

using namespace std;

void foo()
{
    char* p = new char[1];
    
    return ;
}

int main(int argc, char** argv)
{
    
    foo();
    foo();
    foo();
    return 1;
}

