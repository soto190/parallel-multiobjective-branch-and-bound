#Makefile for compiling in XeonPhi.

CXX = icpc
DEBUGGER = gdb-ia

#INC_DIR = -I/opt/intel/compilers_and_libraries_2017.1.132/linux/tbb/include
#LIB_DIR = -L/opt/intel/compilers_and_libraries_2017/linux/tbb/lib/intel64_lin/gcc4.7
LIBS = -tbb -ltbbmalloc -ltbbmalloc_proxy
LIBS_DEBUG = -tbb -ltbbmalloc -ltbbmalloc_proxy_debug

LDFLAGS = -O0 -std=c++11

src = $(wildcard *.cpp)
obj = $(src:.cpp = .o)

all: branchAndBound

debug: LIBS = $(LIBS_DEBUG)
debug: LDFLAGS = -O0 -std=c++11 -g -debug parallel -traceback -shared-intel
debug: branchAndBound

branchAndBound: $(obj)
	$(CXX) $(LIBS) -o $@ $^ $(LDFLAGS)

.BOOM: clean

clean:
	rm -f branchAndBound
