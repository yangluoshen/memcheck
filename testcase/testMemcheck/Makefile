CXX ?= g++
CXXFLAGS = 

LD_MEMPOOL = libleak.so
AR_MEMPOOL = libleak.a
LIB_DIR = /lib/
LD_LEAK_FLAG = -lleak
OBJS = mempool.o loghandle.o memutil.o
SRC_DIR = /home/yangluo/github/memcheck/src/
SRCS = $(SRC_DIR)mempool.cpp $(SRC_DIR)loghandle.cpp $(SRC_DIR)memutil.cpp

CXX_FLAGS = -Wall -Wshadow -Wextra -rdynamic 
CXX_LD_FLAGS = -shared -fPIC
CXX_DEBUG_FLAGS = -g
CXX_RELEASE_FLAGS = -O3 -march=native
CXX_ARCH = -m32

CXXFLAGS += -D__MEMPOOL_RUNTIME_DETAIL 
CXXFLAGS += -D__PRIND_LOG

#don't use option -pedantic please!!!
so_mempool: $(SRCS)
	$(CXX) $(CXX_LD_FLAGS) $(CXXFLAGS) $(SRCS) -o $(LD_MEMPOOL)  -Wall -Wshadow #$(CXX_ARCH) 
	mv $(LD_MEMPOOL) $(LIB_DIR)
ar_mempool: $(SRCS)
	$(CXX) -c $(SRCS) $(CXXFLAGS) -I$(SRC_DIR)  #$(CXX_ARCH) 
	ar rcs $(AR_MEMPOOL) $(OBJS)

ar_mempool_test: mempool.cpp
	$(CXX) -c mempool.cpp #$(CXX_ARCH)
	ar rcs $(AR_MEMPOOL) mempool.o


#if use backtrace, -rdynamic is essential
main: main.cpp 
	$(CXX) $(CXX_FLAGS) -o main main.cpp $(LD_LEAK_FLAG) $(CXX_DEBUG_FLAGS) #$(CXX_ARCH)

ar_main: main.cpp
	$(CXX) $(CXX_FLAGS) -o main main.cpp -L. $(LD_LEAK_FLAG) $(CXX_DEBUG_FLAGS) -I$(SRC_DIR) #$(CXX_ARCH)

clean:
	rm $(LD_MEMPOOL) main fizz.leak.* *.o *.a
