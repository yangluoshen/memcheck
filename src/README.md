Rengar
======
---

一个可以使用的版本.


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
- 过滤对STL的检查 
- 分离源文件，做成动态库 (done)
- 改进memcheck.h的奇葩限制
- 改进dump file 时CPU飙升 
- 扩充检查粒度
