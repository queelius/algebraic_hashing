/**
 * @file benchmark_throughput.cpp
 * @brief Throughput benchmarks for hash functions
 *
 * Measures hashing throughput (hashes/sec) for various input sizes
 * comparing our implementations against std::hash baseline.
 */

#include <algebraic_hashing/hashing/fnv_hash.hpp>
#include <algebraic_hashing/algebra/xor_hash_fn_compose.hpp>

#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <string>
#include <iomanip>
#include <functional>

using namespace algebraic_hashing;
using namespace std::chrono;

// Generate random strings of specified length
std::vector<std::string> generate_random_strings(size_t count, size_t length) {
    std::vector<std::string> result;
    result.reserve(count);

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<> dis(32, 126); // Printable ASCII

    for (size_t i = 0; i < count; ++i) {
        std::string s;
        s.reserve(length);
        for (size_t j = 0; j < length; ++j) {
            s += static_cast<char>(dis(gen));
        }
        result.push_back(std::move(s));
    }

    return result;
}

// Benchmark template
template <typename HashFunc>
double benchmark_hash(const std::vector<std::string>& data, HashFunc hasher, const std::string& name) {
    // Warmup
    volatile size_t dummy = 0;
    for (int i = 0; i < 1000; ++i) {
        dummy ^= hasher(data[i % data.size()]);
    }

    // Actual benchmark
    auto start = high_resolution_clock::now();

    size_t hash_sum = 0;
    for (const auto& s : data) {
        hash_sum ^= hasher(s);
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();

    // Prevent optimization
    dummy ^= hash_sum;

    // Return millions of hashes per second
    double hashes_per_sec = (static_cast<double>(data.size()) / duration) * 1000000.0;
    return hashes_per_sec / 1000000.0; // Convert to millions
}

void run_throughput_benchmarks() {
    const size_t num_strings = 1000000; // 1M strings per test

    std::cout << "=== Hash Function Throughput Benchmark ===" << std::endl;
    std::cout << "Dataset: " << num_strings << " random strings" << std::endl;
    std::cout << std::endl;

    std::vector<size_t> sizes = {8, 64, 256, 1024};

    std::cout << std::setw(20) << "Algorithm"
              << std::setw(12) << "8B"
              << std::setw(12) << "64B"
              << std::setw(12) << "256B"
              << std::setw(12) << "1KB" << std::endl;
    std::cout << std::string(68, '-') << std::endl;

    // std::hash baseline
    {
        std::cout << std::setw(20) << "std::hash";
        for (auto size : sizes) {
            auto data = generate_random_strings(num_strings, size);
            double throughput = benchmark_hash(data, std::hash<std::string>{}, "std::hash");
            std::cout << std::setw(12) << std::fixed << std::setprecision(1) << throughput;
        }
        std::cout << std::endl;
    }

    // FNV hash
    {
        std::cout << std::setw(20) << "FNV (ours)";
        for (auto size : sizes) {
            auto data = generate_random_strings(num_strings, size);
            double throughput = benchmark_hash(data, fnv_hash{}, "FNV");
            std::cout << std::setw(12) << std::fixed << std::setprecision(1) << throughput;
        }
        std::cout << std::endl;
    }

    // FNV XOR FNV composition (with different seeds)
    {
        std::cout << std::setw(20) << "FNV⊕FNV";
        for (auto size : sizes) {
            auto data = generate_random_strings(num_strings, size);
            xor_hash_fn_compose<fnv_hash, fnv_hash> composed{fnv_hash{0x12345678}, fnv_hash{0x87654321}};
            double throughput = benchmark_hash(data, composed, "FNV⊕FNV");
            std::cout << std::setw(12) << std::fixed << std::setprecision(1) << throughput;
        }
        std::cout << std::endl;
    }


    std::cout << std::endl;
    std::cout << "Note: Values shown in millions of hashes/second" << std::endl;
}

int main() {
    run_throughput_benchmarks();
    return 0;
}
