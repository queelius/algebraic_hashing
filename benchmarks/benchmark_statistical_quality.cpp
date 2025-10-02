/**
 * @file benchmark_statistical_quality.cpp
 * @brief Statistical quality metrics for hash functions
 *
 * Measures:
 * 1. Avalanche effect (bit flip probability)
 * 2. Chi-square uniformity test
 * 3. Entropy measurement
 */

#include <algebraic_hashing/hashing/fnv_hash.hpp>
#include <algebraic_hashing/algebra/xor_hash_fn_compose.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <cmath>
#include <iomanip>
#include <bitset>
#include <map>

using namespace algebraic_hashing;

// Generate random strings
std::vector<std::string> generate_test_strings(size_t count, size_t length) {
    std::vector<std::string> result;
    result.reserve(count);

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<> dis(32, 126);

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

// Flip a single bit in a string
std::string flip_bit(const std::string& s, size_t bit_position) {
    std::string result = s;
    if (bit_position / 8 < result.size()) {
        size_t byte_pos = bit_position / 8;
        size_t bit_in_byte = bit_position % 8;
        result[byte_pos] ^= (1 << bit_in_byte);
    }
    return result;
}

// Count different bits between two hash values
template <typename HashType>
size_t count_different_bits(HashType h1, HashType h2) {
    HashType diff = h1 ^ h2;
    return std::bitset<sizeof(HashType) * 8>(diff).count();
}

// Measure avalanche effect
template <typename HashFunc>
double measure_avalanche(HashFunc hasher, const std::vector<std::string>& data) {
    const size_t hash_bits = sizeof(size_t) * 8;
    size_t total_flips = 0;
    size_t total_opportunities = 0;

    for (const auto& s : data) {
        size_t original_hash = hasher(s);

        // Try flipping each bit in the input
        size_t max_bit = std::min(s.size() * 8, size_t(256)); // Test first 256 bits
        for (size_t bit = 0; bit < max_bit; ++bit) {
            std::string flipped = flip_bit(s, bit);
            size_t flipped_hash = hasher(flipped);

            total_flips += count_different_bits(original_hash, flipped_hash);
            total_opportunities += hash_bits;
        }
    }

    // Ideal avalanche: 50% of bits flip
    return static_cast<double>(total_flips) / total_opportunities;
}

// Chi-square test for uniformity
template <typename HashFunc>
double chi_square_test(HashFunc hasher, const std::vector<std::string>& data) {
    const size_t num_buckets = 1000;
    std::vector<size_t> buckets(num_buckets, 0);

    // Hash all strings and distribute into buckets
    for (const auto& s : data) {
        size_t hash = hasher(s);
        buckets[hash % num_buckets]++;
    }

    // Calculate chi-square statistic
    double expected = static_cast<double>(data.size()) / num_buckets;
    double chi_square = 0.0;

    for (size_t count : buckets) {
        double diff = count - expected;
        chi_square += (diff * diff) / expected;
    }

    // Normalize to [0, 1] where 1 is ideal
    // For 999 degrees of freedom, expected chi-square is approximately 999
    // We'll use a simple normalization: closer to expected = better
    double normalized = 1.0 - std::abs(chi_square - (num_buckets - 1)) / (num_buckets - 1);
    return std::max(0.0, std::min(1.0, normalized));
}

// Measure entropy (Shannon entropy of output distribution)
template <typename HashFunc>
double measure_entropy(HashFunc hasher, const std::vector<std::string>& data) {
    std::map<size_t, size_t> frequency;

    // Count hash frequencies
    for (const auto& s : data) {
        size_t hash = hasher(s);
        frequency[hash]++;
    }

    // Calculate Shannon entropy
    double entropy = 0.0;
    double total = static_cast<double>(data.size());

    for (const auto& [hash, count] : frequency) {
        double probability = count / total;
        if (probability > 0) {
            entropy -= probability * std::log2(probability);
        }
    }

    // Normalize to [0, 1]
    // Maximum entropy for N items is log2(N)
    double max_entropy = std::log2(total);
    return entropy / max_entropy;
}

void run_quality_benchmarks() {
    const size_t num_strings = 10000;
    const size_t string_length = 64;

    std::cout << "=== Statistical Quality Metrics ===" << std::endl;
    std::cout << "Dataset: " << num_strings << " random strings of " << string_length << " bytes" << std::endl;
    std::cout << "Ideal value: close to 0.5 for avalanche, close to 1.0 for others" << std::endl;
    std::cout << std::endl;

    auto data = generate_test_strings(num_strings, string_length);

    std::cout << std::setw(20) << "Hash Function"
              << std::setw(15) << "Avalanche"
              << std::setw(15) << "Chi² Test"
              << std::setw(15) << "Entropy" << std::endl;
    std::cout << std::string(65, '-') << std::endl;

    // FNV hash
    {
        fnv_hash h;
        double avalanche = measure_avalanche(h, data);
        double chi_sq = chi_square_test(h, data);
        double entropy = measure_entropy(h, data);

        std::cout << std::setw(20) << "FNV"
                  << std::setw(15) << std::fixed << std::setprecision(4) << avalanche
                  << std::setw(15) << std::fixed << std::setprecision(4) << chi_sq
                  << std::setw(15) << std::fixed << std::setprecision(4) << entropy
                  << std::endl;
    }

    // FNV XOR FNV (with different seeds)
    {
        xor_hash_fn_compose<fnv_hash, fnv_hash> h{fnv_hash{0x12345678}, fnv_hash{0x87654321}};
        double avalanche = measure_avalanche(h, data);
        double chi_sq = chi_square_test(h, data);
        double entropy = measure_entropy(h, data);

        std::cout << std::setw(20) << "FNV⊕FNV"
                  << std::setw(15) << std::fixed << std::setprecision(4) << avalanche
                  << std::setw(15) << std::fixed << std::setprecision(4) << chi_sq
                  << std::setw(15) << std::fixed << std::setprecision(4) << entropy
                  << std::endl;
    }


    // std::hash for comparison
    {
        std::hash<std::string> h;
        double avalanche = measure_avalanche(h, data);
        double chi_sq = chi_square_test(h, data);
        double entropy = measure_entropy(h, data);

        std::cout << std::setw(20) << "std::hash"
                  << std::setw(15) << std::fixed << std::setprecision(4) << avalanche
                  << std::setw(15) << std::fixed << std::setprecision(4) << chi_sq
                  << std::setw(15) << std::fixed << std::setprecision(4) << entropy
                  << std::endl;
    }

    std::cout << std::endl;
}

int main() {
    run_quality_benchmarks();
    return 0;
}
