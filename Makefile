CXX = g++

ARCH := core2 # Replace this with your CPU architecture.

LDFLAGS := -march=$(ARCH) -O3 -fopenmp -m64 -std=c++11

src = $(wildcard *.cpp)
obj = $(src:.cpp = .o)

branchAndBound: $(obj)
	$(CXX) -o $@ $^ $(LDFLAGS)

.BOOM: clean

clean:
	rm -f branchAndBound
