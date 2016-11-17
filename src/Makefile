CXX ?= g++
CXX_MACRO = 
CXX_FLAGS = -Wall -Wshadow -Wextra  
CXX_LD_FLAGS = -fpic -shared 
CXX_RELEASE_FLAGS = -O3 -march=native
#CXX_ARCH = -m32

#CXXFLAGS += -D__MEMPOOL_RUNTIME_DETAIL 

LD_LEAK_FLAG = -lleak

HEAD_DIR = ./include
#OBJS = mempool.o lock.o log.o utility.o
SRCS = mempool.cpp lock.cpp log.cpp utility.cpp


#don't use option -pedantic please!!!
libleak.so: $(SRCS)
	$(CXX) $(CXX_LD_FLAGS) $(CXX_FLAGS) -g -rdynamic -pthread -o $@ $^ -I ./include
	mv $@ /lib/

#ar_mempool: $(SRCS)
#	$(CXX) -c $(SRCS) $(CXX_ARCH) $(CXXFLAGS)
#	ar rcs $(AR_MEMPOOL) $(OBJS)

main: main.cpp 
	$(CXX) $(CXX_FLAGS) -g -o $@ $^  $(LD_LEAK_FLAG) -I ./include

clean:
	rm main fizz.memcheck.*.log *.o 
