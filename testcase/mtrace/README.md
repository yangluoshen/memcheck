mtrace
=======

1. Description
--------
- As we known, mtrace 能够检查malloc与free配对，若malloc与free不配对，则我们认为内存泄露;但是缺点是不能检查new/new[]与delete/delete[]配对的问题, 准确说是可以检查处此类内存泄露，但是不能指出哪个位置泄露了。
- memtrace.h 弥补mtrace的不足（互补），她能检查new与delete配对问题，但是无法检测malloc与free配对问题。
- 结合mtrace 和 memtrace.h可以比较全面的进行内存泄露检测。


2. mtrace
-----
- mtrace usage
    - mtrace要求在运行前设置环境变量`MALLOC_TRACE`
        
            export MALLOC_TRACE=trace.log

    - 要求必须有-g选项

            g++ -g main.cpp -o main

    - 运行
        
            ./main

    - 在当前目录下生成`trace.log`文件
    - 使用mtrace命令解析trace.log

            mtrace main trace.log


- Result

    yangluo@yangluoPC:~/github/memcheck/testcase/mtrace$ mtrace main trace.log 
    
    Memory not freed:
    -----------------
               Address     Size     Caller
               0x0000000000f06460      0x8  at /home/yangluo/github/memcheck/testcase/mtrace/main.cpp:40
               0x0000000000f06480      0x3  at /home/yangluo/github/memcheck/testcase/mtrace/main.cpp:68
    

3. memtrace.h
------
- Usage
         
        g++ -g testMemtrace.cpp -o test
        ./test
        vi trace.report

- Decription
    - memtrace.h 能够完成对new内存泄露的检测，但不能检查malloc，malloc的工作交给mtrace.
    - 应用程序(testMemtrace.cpp)需包含memtrace.h头文件。


4. BUG
------
- 多个cpp文件包含memtrace.h时会报重定义错误，#ifndef解决不了，待解决。
- 编译成so，操作符重写变得无效。
