# Makefile for algebraic_hashing benchmarks

CXX = g++
CXXFLAGS = -std=c++20 -O3 -march=native -I../include -pthread -Wall
TARGETS = benchmark_throughput benchmark_statistical_quality benchmark_perfect_hash

.PHONY: all clean run

all: $(TARGETS)

benchmark_throughput: benchmark_throughput.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

benchmark_statistical_quality: benchmark_statistical_quality.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

benchmark_perfect_hash: benchmark_perfect_hash.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

run: all
	@echo "========================================"
	@echo "Running Throughput Benchmark"
	@echo "========================================"
	./benchmark_throughput
	@echo ""
	@echo "========================================"
	@echo "Running Statistical Quality Benchmark"
	@echo "========================================"
	./benchmark_statistical_quality
	@echo ""
	@echo "========================================"
	@echo "Running Perfect Hash Benchmark"
	@echo "========================================"
	./benchmark_perfect_hash

clean:
	rm -f $(TARGETS)
