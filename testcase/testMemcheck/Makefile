CXX ?= g++
CXXFLAGS_FOR_LIB = 

LD_MEMPOOL = libleak.so
AR_MEMPOOL = libleak.a
LIB_DIR = /lib/
LD_LEAK_FLAG = -lleak
OBJS = mempool.o loghandle.o memutil.o
SRC_DIR = /home/yangluo/github/memcheck/src/
SRCS = $(SRC_DIR)mempool.cpp $(SRC_DIR)loghandle.cpp $(SRC_DIR)memutil.cpp

CXX_FLAGS = -Wall -Wshadow -Wextra
CXX_LD_FLAGS = -fpic -shared
CXX_DEBUG_FLAGS = -g
CXX_RELEASE_FLAGS = -O3 -march=native
#CXX_ARCH = -m32

CXXFLAGS_FOR_LIB += -D__MEMPOOL_RUNTIME_DETAIL 
CXXFLAGS_FOR_LIB += -D__PRINT_LOG
CXXFLAGS_FOR_LIB += -D__USE_RDYNAMIC_OPTION

#don't use option -pedantic please!!!
so_mempool: 
	$(CXX) $(CXX_LD_FLAGS) -o $(LD_MEMPOOL)  -Wall -Wshadow $(SRCS) $(CXXFLAGS_FOR_LIB)
	mv $(LD_MEMPOOL) $(LIB_DIR)
ar_mempool: $(SRCS)
	$(CXX) -c $(SRCS) $(CXXFLAGS_FOR_LIB) -I$(SRC_DIR)  #$(CXX_ARCH) 
	ar rcs $(AR_MEMPOOL) $(OBJS)

ar_mempool_test: mempool.cpp
	$(CXX) -c mempool.cpp #$(CXX_ARCH)
	ar rcs $(AR_MEMPOOL) mempool.o


#if use backtrace, -rdynamic is essential
so_main: main.cpp 
	$(CXX) $(CXX_FLAGS) -rdynamic -o main main.cpp $(LD_LEAK_FLAG) $(CXX_DEBUG_FLAGS) #$(CXX_ARCH)

ar_main: main.cpp
	$(CXX) $(CXX_FLAGS) -o main main.cpp -L. $(LD_LEAK_FLAG) $(CXX_DEBUG_FLAGS) -I$(SRC_DIR) #$(CXX_ARCH)

clean:
	rm $(LD_MEMPOOL) main fizz.leak.* *.o *.a
