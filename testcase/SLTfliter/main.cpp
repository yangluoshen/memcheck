#include "memcheck.h"

#include <unistd.h>

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <deque>
#include <set>

using namespace std;

void string_check()
{

    string s1 = string("s1");
    string s2 = string("s2");
    SMiniCheck check;
    
    s2.assign(s1);

}

void vector_check()
{
    vector <int> vec;

    SMiniCheck check;

    vec.push_back(2);
    vec.push_back(2);
    
}

void list_check()
{
    list <int> l;
    
    SMiniCheck check;
    
    l.push_back(1);
    l.push_back(1);

}

void map_check()
{
    map <int ,int> m;

    SMiniCheck check;
    m.insert(make_pair(1,1));

}

int main()
{

    {
    //SMiniCheck check;
    
    string_check();
    vector_check();
    list_check();
    map_check();
    }

    {
        SMiniCheck check;
        new char[1];
    }

    sleep(1);
    return 0;
}

