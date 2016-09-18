Unit Test -- new operator
=====

1. Description 
------
- 检查重写new/new[] 操作符之后，new操作是否会调用类的构造函数，以及基类(如果有的话)的构造函数。
- 检查delete/delete[] 操作符重写之后，是否会调用类的(虚)析构函数，以及基类(如果有的话)的(虚)析构函数。 


2. Usage
------
- Env: Ubuntu 14.04 x86_64, g++4.8.4
- g++ main.cpp -o main

3. Result
-----

    1:sizeof (B) = 8
    2:ENTER operator new []
    3:alloc size :24
    4:file name  :main.cpp
    5:line no    :86
    6:EXIT operator new []
    7:constructor A
    8:constructor B
    9:constructor A
    10:constructor B
    11:destructor B
    12:destructor A
    13:destructor B
    14:destructor A
    15:ENTER operator delete []
    16:EXIT operator delete []


4. Conclusion
-------
- 重写new和delete之后，依然会调用相应的构造函数和(虚)析构函数。调用时机有点不同。<br>
  由2~10可以看出，构造函数是在operator new[]函数结束之后才调用的；<br>
  由11~16可以看出，(虚)析构函数在operator delete[] 函数调用之前就调用的。<br>

- 由7~14可知重写不影响继承。



