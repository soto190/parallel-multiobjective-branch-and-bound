#Makefile for compiling in XeonPhi.

CXX = icpc
DEBUGGER = gdb-ia

INC_DIR = -I/opt/intel/compilers_and_libraries_2017.1.132/linux/tbb/include
LIB_DIR = -L/opt/intel/compilers_and_libraries_2017/linux/tbb/lib/intel64_lin/gcc4.7
LIBS_TO_LOAD = -ltbb -ltbbmalloc
LDFLAGS = -O0 -std=c++11
LDDEBUG = -O0 -g -traceback -std=c++11

src = $(wildcard *.cpp)
obj = $(src:.cpp = .o)

all: 

branchAndBound: $(obj)
	$(CXX) $(LIB_DIR) -o $@ $^ $(LDDEBUG) $(INC_DIR) $(LIBS_TO_LOAD)

.BOOM: clean

clean:
	rm -f branchAndBound
