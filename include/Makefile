CXX ?= /opt/upcc_gcc4.9/install/bin/gcc
#CXX ?= g++
CXXFLAGS = 
CXX_FLAGS = -Wall -Wshadow -Wextra -rdynamic 
CXX_LD_FLAGS = -shared -fPIC
CXX_DEBUG_FLAGS = -g
CXX_RELEASE_FLAGS = -O3 -march=native
CXX_ARCH = -m32

LD_MEMPOOL = libleak.so
AR_MEMPOOL = libleak.a
LIB_DIR = /lib64/
LD_LEAK_FLAG = -lleak

#don't use option -pedantic !!!
so_mempool: mempool.cpp
	$(CXX) $(CXX_LD_FLAGS) -o $(LD_MEMPOOL) mempool.cpp -Wall -Wshadow $(CXX_ARCH)
	mv $(LD_MEMPOOL) $(LIB_DIR)
ar_mempool: mempool.cpp
	$(CXX) -c mempool.cpp $(CXX_ARCH)
	ar rcs $(AR_MEMPOOL) mempool.o

clean:
	rm $(LD_MEMPOOL) main fizz.leak.*
