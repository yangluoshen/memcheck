Rengar
======
---

1. Usage
-----
    
    make libleak.so
    make main
    ./main
    
2. Description
--------
- GMemoryRecordSwitch 可以理解为一个开关，用于开闭此工具。
- SMiniCheck 的用法类似RAII，SMiniCheck是对 GMemoryRecordSwitch的封装.支持检查所嵌套的函数（即作用域内，所调用的函数，以及下一层函数）.
- GMemoryRecordSwitch表面上支持的范围比较广，比较灵活，但是及容易犯错，尽量使用SMiniCheck，SMiniCheck很安全。
- 线程安全。

3. Attention
----------
- 尽量避免在头文件中包含memcheck.h
- 同一个工程中，有且只有一个地方（最好是.cpp文件）中包含memcheck.h
- 不用担心不包含memcheck.h用不到SMiniCheck，请包含mempool.h
    

4. Todo List
---------
- 过滤对STL的检查 (done) 
- 分离源文件，做成动态库 (done)
- 改进memcheck.h的奇葩限制
- 改进dump file 时CPU飙升(doing) 
- 扩充检查粒度


5. Solution
----
- 过滤对STL的检查
  - 统计STL可能使用的内存操作，记录被Rengar跟踪到的关键字
  - 根据关键字生成一个STL黑名单，下次Rengar忽略该关键字
  - \* 设计接口时，开放一个钩子，方便扩展

- 改进dump file 时CPU飙升 
  - 原实现频繁打开关闭文件. 调用栈(backtrace)的每一帧都会开闭一次文件,一个调用栈就开闭十来次.<br>
    尝试改进：一次性读取所有调用栈信息，一次性写入文件。<br>
  - 改进1：
    每一个batrace只写一次文件.



