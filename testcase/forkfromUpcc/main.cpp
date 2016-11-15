#include <vector>

#include "memcheck.h"

#include "unistd.h"

using namespace std;
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
    vector <int> vec_out;
    vec_out.push_back(2);
    {
        SMiniCheck check;
        foo();
        vector <int> vec;
        vec.push_back(1);
        vec.push_back(2);
        vec_out.push_back(2);
    }
    sleep(1);

    return 1;

}

