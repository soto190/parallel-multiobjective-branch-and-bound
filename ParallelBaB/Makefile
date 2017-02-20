#Makefile for compiling in XeonPhi.

CXX = /opt/intel/compilers_and_libraries_2017/linux/bin/intel64/icpc

INC_DIR = /opt/intel/compilers_and_libraries_2017.1.132/linux/tbb/include
LIB = /opt/intel/compilers_and_libraries_2017.1.132/linux/tbb/lib/intel64_lin/gcc4.7
LIBS = -ltbb -ltbbmalloc
LIB_DIR = -L$(LIB) -Wl,-rpath=$(LIB)
LDFLAGS = -O3 -std=c++11 -I$(INC_DIR)

src = $(wildcard *.cpp)
obj = $(src:.cpp = .o)

branchAndBound: $(obj)
    $(CXX) $(LIB_DIR) -o $@ $^ $(LDFLAGS) $(LIBS)

.BOOM: clean

clean:
    rm -f branchAndBound
