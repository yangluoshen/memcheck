mtrace
=======

1. Description
--------
- As we known, mtrace 能够检查malloc与free配对的问题，若malloc与free不配对，则会导致内存泄露;但是缺点是不能检查new/new[]与delete/delete[]配对的问题。
- 此测试用例用于检验通过重写new/new[]操作符能否解决mtrace的这个不足。


2. Usage
-----
- mtrace用法
    - mtrace要求在运行前设置环境变量`MALLOC_TRACE`
        
        export MALLOC_TRACE=trace.log

    - 要求-g选项

        g++ -g main.cpp -o main

    - 运行
        
        ./main

    - 在当前目录下生成`trace.log`文件
    - 使用mtrace 解析trace.log

        mtrace main trace.log


3. Result
-----

    yangluo@yangluoPC:~/github/memcheck/testcase/mtrace$ mtrace main trace.log 
    
    Memory not freed:
    -----------------
               Address     Size     Caller
               0x0000000000f06460      0x8  at /home/yangluo/github/memcheck/testcase/mtrace/main.cpp:40
               0x0000000000f06480      0x3  at /home/yangluo/github/memcheck/testcase/mtrace/main.cpp:68
    

4. Conclustion
------
- 重写new操作符之后，mtrace确实能够检查到malloc申请内存的位置
- 从结果来看，malloc始终位于operator new 函数体内， 因此单纯这个结果意义不大。


5. More
----
- 尝试换取operator new的调用者, 使用gcc内建函数__builtin_return_addr.
