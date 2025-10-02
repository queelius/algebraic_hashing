/**
 * @file benchmark_perfect_hash.cpp
 * @brief Perfect hash function construction and query benchmarks
 *
 * Measures:
 * 1. Construction time for various set sizes
 * 2. Query time (average and worst-case)
 * 3. Space overhead (bytes per key)
 */

#include <algebraic_hashing/perfect_hashing/rd_phf_lvl2.hpp>
#include <algebraic_hashing/perfect_hashing/rd_phf_lvl2_builder.hpp>
#include <algebraic_hashing/hashing/fnv_hash.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <iomanip>
#include <set>

using namespace algebraic_hashing;
using namespace std::chrono;

// Generate unique random strings
std::vector<std::string> generate_unique_strings(size_t count, size_t length) {
    std::set<std::string> unique_set;
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<> dis(32, 126);

    while (unique_set.size() < count) {
        std::string s;
        s.reserve(length);
        for (size_t j = 0; j < length; ++j) {
            s += static_cast<char>(dis(gen));
        }
        unique_set.insert(std::move(s));
    }

    return std::vector<std::string>(unique_set.begin(), unique_set.end());
}

void benchmark_perfect_hash() {
    std::cout << "=== Perfect Hash Function Benchmark ===" << std::endl;
    std::cout << std::endl;

    std::vector<size_t> set_sizes = {100, 1000, 10000, 100000};

    std::cout << std::setw(12) << "Set Size"
              << std::setw(15) << "Build (ms)"
              << std::setw(15) << "Query (ns)"
              << std::setw(15) << "Space (B/key)" << std::endl;
    std::cout << std::string(57, '-') << std::endl;

    for (size_t set_size : set_sizes) {
        // Generate test data
        auto keys = generate_unique_strings(set_size, 16);

        // Measure construction time
        auto build_start = high_resolution_clock::now();

        rd_phf_lvl2_builder<fnv_hash> builder;
        auto phf = builder.build(keys.begin(), keys.end(), 42); // 42 is initial seed

        auto build_end = high_resolution_clock::now();
        auto build_duration = duration_cast<milliseconds>(build_end - build_start).count();

        // Measure query time
        auto query_start = high_resolution_clock::now();

        volatile size_t dummy = 0;
        for (const auto& key : keys) {
            dummy ^= phf(key);
        }

        auto query_end = high_resolution_clock::now();
        auto query_duration = duration_cast<nanoseconds>(query_end - query_start).count();
        double avg_query_ns = static_cast<double>(query_duration) / keys.size();

        // Estimate space usage
        // For rd_phf_lvl2: stores N, m, l0 (3 * sizeof(size_t)) + sigma vector (m * sizeof(size_t))
        size_t estimated_space = 3 * sizeof(size_t) + phf.m * sizeof(size_t);
        double bytes_per_key = static_cast<double>(estimated_space) / set_size;

        std::cout << std::setw(12) << set_size
                  << std::setw(15) << std::fixed << std::setprecision(1) << build_duration
                  << std::setw(15) << std::fixed << std::setprecision(1) << avg_query_ns
                  << std::setw(15) << std::fixed << std::setprecision(1) << bytes_per_key
                  << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Note: Query time averaged over all keys in the set" << std::endl;
}

int main() {
    benchmark_perfect_hash();
    return 0;
}
