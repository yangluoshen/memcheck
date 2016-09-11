CXX ?= g++
CXXFLAGS = 
CXX_FLAGS = -Wall -Wshadow -Wextra -rdynamic 
CXX_LD_FLAGS = -fpic -shared 
CXX_DEBUG_FLAGS = -g
CXX_RELEASE_FLAGS = -O3 -march=native
#CXX_ARCH = -m32

CXXFLAGS += -D__MEMPOOL_RUNTIME_DETAIL 
CXXFLAGS += -D__PRIND_LOG
CXXFLAGS += -D__USERDYNAMIC_OPTIONS

LD_MEMPOOL = libleak.so
AR_MEMPOOL = libleak.a
LIB_DIR = /lib/
LD_LEAK_FLAG = -lleak
OBJS = mempool.o loghandle.o memutil.o
SRCS = mempool.cpp loghandle.cpp memutil.cpp

#don't use option -pedantic please!!!
so_mempool: 
	$(CXX) $(CXX_LD_FLAGS) -o $(LD_MEMPOOL)  -Wall -Wshadow $(SRCS) $(CXXFLAGS)
	mv $(LD_MEMPOOL) $(LIB_DIR)
ar_mempool: $(SRCS)
	$(CXX) -c $(SRCS) $(CXX_ARCH) $(CXXFLAGS)
	ar rcs $(AR_MEMPOOL) $(OBJS)

#if use backtrace, -rdynamic is essential
main: main.cpp 
	$(CXX) $(CXX_FLAGS) -o main main.cpp $(LD_LEAK_FLAG) $(CXX_DEBUG_FLAGS) $(CXX_ARCH)

ar_main: main.cpp
	$(CXX) $(CXX_FLAGS) -o main main.cpp -L. $(LD_LEAK_FLAG) $(CXX_ARCH)

clean:
	rm $(LD_MEMPOOL) main fizz.leak.* *.o *.a
