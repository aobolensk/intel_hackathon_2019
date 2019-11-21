CXX = g++
CXXFLAGS = -O3 -std=c++17 -fopenmp

.PHONY: all clean test_100k test_1m test_10m test_all

all: executor.out
test_all: test_100k test_1m test_10m

executor.out: executor.cpp task.o
	$(CXX) $(CXXFLAGS) $^ -o$@

task.o: task.cpp task.h
	$(CXX) -c $(CXXFLAGS) $< -o$@

test_100k: executor.out
	@echo "[100K queries]"
	./$< 10000 10000 100000 128 1 ./output_10k_10k_100k_128_1
	@echo

test_1m: executor.out
	@echo "[1M queries]"
	./$< 10000 10000 1000000 128 1 ./output_10k_10k_1m_128_1
	@echo

test_10m: executor.out
	@echo "[10M queries]"
	./$< 10000 10000 10000000 128 1 ./output_10k_10k_10m_128_1
	@echo

clean:
	rm -f task.o
	rm -f executor.out
