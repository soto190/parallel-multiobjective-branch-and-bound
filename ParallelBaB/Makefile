#Makefile for compiling with ICPC.

CXX = icpc
DEBUGGER = gdb-ia

LIBS = -tbb -ltbbmalloc -ltbbmalloc_proxy
LIBS_DEBUG = -tbb -ltbbmalloc_debug -ltbbmalloc_proxy_debug

#TODO: Some day investigate why the -debug parallel gets and segmentation fault when debugging.
#TODO: Test the flag -fast to optimize for speed in intel compiler.
LDFLAGS = -O0 -shared-intel -std=c++11
LDFLAGS_DEBUG = -O0 -shared-intel -g -traceback -debug all -std=c++11

src = $(wildcard *.cpp)
obj = $(src:.cpp = .o)

all: branchAndBound

debug: LIBS = $(LIBS_DEBUG)
debug: LDFLAGS = $(LDFLAGS_DEBUG)
debug: branchAndBound

branchAndBound: $(obj)
	$(CXX) $(LIBS) $(LDFLAGS) -o $@ $^

.BOOM: clean

clean:
	rm -f branchAndBound
