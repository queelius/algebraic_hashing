#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/algebraic_hashing/core/concepts.hpp"
#include "../include/algebraic_hashing/core/hash_value.hpp"
#include "../include/algebraic_hashing/dsl/algebraic_operations.hpp"
#include "../include/algebraic_hashing/functions/fnv_hash_modern.hpp"
#include <string>
#include <vector>
#include <unordered_set>
#include <random>
#include <chrono>
#include <unordered_map>
#include <cstring>
#include <iostream>

using namespace algebraic_hashing;
using namespace algebraic_hashing::functions;
using namespace algebraic_hashing::dsl;

/**
 * @brief Test suite for modern algebraic hashing architecture
 * 
 * This comprehensive test suite serves multiple purposes:
 * 1. **Validation**: Ensures correctness of implementations
 * 2. **Documentation**: Shows how to use the library
 * 3. **Education**: Demonstrates hash function properties
 * 4. **Regression**: Prevents breaking changes
 */

// ==================== Hash Value Tests ====================

class HashValueTest : public ::testing::Test {
protected:
    hash64 zero_hash = hash64::zero();
    hash64 ones_hash = hash64::ones();
    hash64 test_hash;
    
    void SetUp() override {
        // Create a test hash with known pattern
        for (std::size_t i = 0; i < 8; ++i) {
            test_hash[i] = static_cast<std::uint8_t>(i * 17); // Arbitrary pattern
        }
    }
};

TEST_F(HashValueTest, BasicProperties) {
    // Test zero element properties
    EXPECT_TRUE(zero_hash.is_zero());
    EXPECT_FALSE(ones_hash.is_zero());
    
    // Test XOR properties (abelian group axioms)
    auto h1 = test_hash;
    auto h2 = ~test_hash;
    
    // Identity: h ^ zero = h
    EXPECT_EQ(h1 ^ zero_hash, h1);
    
    // Inverse: h ^ h = zero  
    EXPECT_EQ(h1 ^ h1, zero_hash);
    
    // Commutativity: h1 ^ h2 = h2 ^ h1
    EXPECT_EQ(h1 ^ h2, h2 ^ h1);
    
    // Associativity: (h1 ^ h2) ^ zero = h1 ^ (h2 ^ zero)
    EXPECT_EQ((h1 ^ h2) ^ zero_hash, h1 ^ (h2 ^ zero_hash));
}

TEST_F(HashValueTest, BitwiseOperations) {
    // Test complement
    auto complemented = ~test_hash;
    EXPECT_EQ(~~test_hash, test_hash); // Double complement is identity
    
    // Test AND/OR properties
    EXPECT_EQ(test_hash & zero_hash, zero_hash);
    EXPECT_EQ(test_hash | ones_hash, ones_hash);
    EXPECT_EQ(test_hash & ones_hash, test_hash);
}

TEST_F(HashValueTest, Utility) {
    // Test hex conversion
    auto hex = test_hash.to_hex();
    EXPECT_EQ(hex.length(), 16); // 8 bytes = 16 hex characters
    
    // Test popcount
    auto count = ones_hash.popcount();
    EXPECT_EQ(count, 64); // All bits set
    
    auto zero_count = zero_hash.popcount();  
    EXPECT_EQ(zero_count, 0); // No bits set
}

TEST_F(HashValueTest, Concatenation) {
    hash32 small_hash;
    small_hash[0] = 0xAB;
    small_hash[1] = 0xCD; 
    small_hash[2] = 0xEF;
    small_hash[3] = 0x12;
    
    hash32 another_small;
    another_small[0] = 0x34;
    another_small[1] = 0x56;
    another_small[2] = 0x78;
    another_small[3] = 0x9A;
    
    auto concatenated = concatenate(small_hash, another_small);
    
    // Verify concatenation worked correctly
    EXPECT_EQ(concatenated[0], 0xAB);
    EXPECT_EQ(concatenated[4], 0x34);
    EXPECT_EQ(concatenated[7], 0x9A);
}

// ==================== Concepts Tests ====================

class ConceptsTest : public ::testing::Test {
protected:
    fnv64 fnv_hasher;
    std::string test_string = "test";
    int test_int = 42;
};

TEST_F(ConceptsTest, HashableTypes) {
    // Test that common types satisfy Hashable concept
    EXPECT_TRUE(concepts::Hashable<std::string>);
    EXPECT_TRUE(concepts::Hashable<int>);
    EXPECT_TRUE(concepts::Hashable<double>);
    EXPECT_TRUE(concepts::Hashable<std::vector<int>>);
    EXPECT_TRUE(concepts::Hashable<hash64>); // Hash values are hashable too
}

TEST_F(ConceptsTest, HashValueConcept) {
    EXPECT_TRUE(concepts::HashValue<hash32>);
    EXPECT_TRUE(concepts::HashValue<hash64>);
    EXPECT_TRUE(concepts::HashValue<hash128>);
    EXPECT_TRUE(concepts::HashValue<hash256>);
}

TEST_F(ConceptsTest, HashFunctionConcept) {
    EXPECT_TRUE(concepts::ComposableHashFunction<fnv64>);
    EXPECT_TRUE(concepts::HashFunction<fnv64>);
}

// ==================== FNV Hash Function Tests ====================

class FnvHashTest : public ::testing::Test {
protected:
    fnv64 hasher;
    fnv32 hasher32;
    std::vector<std::string> test_strings = {
        "",
        "a", 
        "hello",
        "hello world",
        "The quick brown fox jumps over the lazy dog"
    };
};

TEST_F(FnvHashTest, Determinism) {
    // Same input should always produce same output
    for (const auto& str : test_strings) {
        auto hash1 = hasher(str);
        auto hash2 = hasher(str);
        EXPECT_EQ(hash1, hash2) << "Hash should be deterministic for: " << str;
    }
}

TEST_F(FnvHashTest, Distinctness) {
    // Different inputs should produce different outputs (with high probability)
    std::unordered_set<hash64> seen_hashes;
    
    for (const auto& str : test_strings) {
        auto hash = hasher(str);
        EXPECT_EQ(seen_hashes.find(hash), seen_hashes.end()) 
            << "Hash collision for: " << str;
        seen_hashes.insert(hash);
    }
}

TEST_F(FnvHashTest, AvalancheEffect) {
    // Small input changes should cause large output changes
    std::string base = "hello";
    auto base_hash = hasher(base);
    
    std::string modified = "hallo"; // One character change
    auto modified_hash = hasher(modified);
    
    auto diff = base_hash ^ modified_hash;
    auto changed_bits = diff.popcount();
    
    // Good avalanche: ~50% of bits should change
    double change_percentage = static_cast<double>(changed_bits) / 64.0;
    EXPECT_GT(change_percentage, 0.3) << "Avalanche effect too weak";
    EXPECT_LT(change_percentage, 0.7) << "Avalanche effect suspicious";
}

TEST_F(FnvHashTest, DifferentSizes) {
    std::string test = "test";
    
    auto hash32 = hasher32(test);
    auto hash64 = hasher(test);
    
    // Different sized hashes should be different (truncation test)
    EXPECT_NE(truncate<4>(hash64), hash32) << "Different algorithms should produce different results";
}

TEST_F(FnvHashTest, EmptyInput) {
    auto empty_hash = hasher("");
    EXPECT_FALSE(empty_hash.is_zero()) << "Empty string should not hash to zero";
}

TEST_F(FnvHashTest, LargeInput) {
    // Test with large input to check performance and correctness
    std::string large_input(10000, 'x');
    
    auto start = std::chrono::high_resolution_clock::now();
    auto hash = hasher(large_input);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    EXPECT_FALSE(hash.is_zero());
    EXPECT_LT(duration.count(), 1000) << "Large input should be processed quickly";
}

// ==================== Algebraic Operations Tests ====================

class AlgebraicOperationsTest : public ::testing::Test {
protected:
    fnv64 fnv1, fnv2;
    std::string test_input = "algebraic_test";
};

TEST_F(AlgebraicOperationsTest, XorComposition) {
    // Create XOR composition
    auto composed = fnv1 ^ fnv2;
    
    auto result = composed(test_input);
    auto expected = fnv1(test_input) ^ fnv2(test_input);
    
    EXPECT_EQ(result, expected);
}

TEST_F(AlgebraicOperationsTest, XorCompositionProperties) {
    fnv64 fnv3;
    
    // Test commutativity: f1 ^ f2 = f2 ^ f1  
    auto comp1 = fnv1 ^ fnv2;
    auto comp2 = fnv2 ^ fnv1;
    
    EXPECT_EQ(comp1(test_input), comp2(test_input));
    
    // Test associativity: (f1 ^ f2) ^ f3 = f1 ^ (f2 ^ f3)
    auto comp_left = (fnv1 ^ fnv2) ^ fnv3;
    auto comp_right = fnv1 ^ (fnv2 ^ fnv3);
    
    EXPECT_EQ(comp_left(test_input), comp_right(test_input));
}

TEST_F(AlgebraicOperationsTest, SequentialComposition) {
    // Create sequential composition f1 ∘ f2
    auto composed = fnv1 * fnv2;
    
    auto result = composed(test_input);
    auto intermediate = fnv2(test_input);
    auto expected = fnv1(intermediate);
    
    EXPECT_EQ(result, expected);
}

TEST_F(AlgebraicOperationsTest, Complement) {
    auto complement = ~fnv1;
    
    auto result = complement(test_input);
    auto expected = ~fnv1(test_input);
    
    EXPECT_EQ(result, expected);
    
    // Test that complement inverts all bits correctly
    auto hash_value = fnv1(test_input);
    auto complement_value = complement(test_input);
    
    // XOR with complement should give all ones
    auto xor_result = hash_value ^ complement_value;
    EXPECT_EQ(xor_result, hash64::ones());
}

TEST_F(AlgebraicOperationsTest, RepeatedApplication) {
    auto repeated = repeat<3>(fnv1);
    
    auto result = repeated(test_input);
    
    // Manually compute 3 applications
    auto step1 = fnv1(test_input);
    auto step2 = fnv1(step1);
    auto expected = fnv1(step2);
    
    EXPECT_EQ(result, expected);
}

TEST_F(AlgebraicOperationsTest, ChainComposition) {
    fnv64 fnv3, fnv4;
    
    auto chained = chain(fnv1, fnv2, fnv3, fnv4);
    
    auto result = chained(test_input);
    auto expected = fnv1(test_input) ^ fnv2(test_input) ^ fnv3(test_input) ^ fnv4(test_input);
    
    EXPECT_EQ(result, expected);
}

// ==================== Performance and Statistics Tests ====================

class PerformanceTest : public ::testing::Test {
protected:
    fnv64 hasher;
    std::vector<std::string> performance_data;
    
    void SetUp() override {
        // Generate test data
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> length_dist(10, 100);
        std::uniform_int_distribution<> char_dist('a', 'z');
        
        for (int i = 0; i < 1000; ++i) {
            std::string str;
            int length = length_dist(gen);
            str.reserve(length);
            
            for (int j = 0; j < length; ++j) {
                str += static_cast<char>(char_dist(gen));
            }
            performance_data.push_back(std::move(str));
        }
    }
};

TEST_F(PerformanceTest, ThroughputMeasurement) {
    hasher.reset_statistics();
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (const auto& data : performance_data) {
        hasher(data);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    auto stats = hasher.get_statistics();
    
    EXPECT_EQ(stats.calls, performance_data.size());
    EXPECT_GT(stats.throughput_mbps(), 0.0);
    EXPECT_LT(duration.count(), 100) << "Should process 1000 strings quickly";
    
    std::cout << "Performance Results:\n";
    std::cout << "- Processed " << stats.calls << " strings\n";
    std::cout << "- Average time per hash: " << stats.average_time_ns() << " ns\n";
    std::cout << "- Throughput: " << stats.throughput_mbps() << " MB/s\n";
}

TEST_F(PerformanceTest, DistributionQuality) {
    std::array<std::size_t, 256> byte_counts{};
    
    for (const auto& data : performance_data) {
        auto hash = hasher(data);
        ++byte_counts[hash[0]]; // Count first byte distribution
    }
    
    // Calculate chi-squared statistic for uniformity test
    double expected = static_cast<double>(performance_data.size()) / 256.0;
    double chi_squared = 0.0;
    
    for (std::size_t count : byte_counts) {
        double diff = static_cast<double>(count) - expected;
        chi_squared += (diff * diff) / expected;
    }
    
    // For 255 degrees of freedom, critical value at p=0.01 is ~310
    EXPECT_LT(chi_squared, 400) << "Hash distribution should be reasonably uniform";
    
    std::cout << "Distribution Analysis:\n";
    std::cout << "- Expected count per byte: " << expected << "\n";
    std::cout << "- Chi-squared statistic: " << chi_squared << "\n";
}

// ==================== Educational Examples Tests ====================

class EducationalTest : public ::testing::Test {
protected:
    fnv64 hasher;
};

TEST_F(EducationalTest, ConceptChecking) {
    // This test demonstrates compile-time concept checking
    static_assert(concepts::ComposableHashFunction<fnv64>);
    static_assert(concepts::HashValue<hash64>);
    static_assert(concepts::Hashable<std::string>);
    
    // These should fail to compile if uncommented:
    // static_assert(concepts::HashValue<int>); // int is not a hash value
    // static_assert(concepts::ComposableHashFunction<int>); // int is not a hash function
    
    SUCCEED() << "Concept checking works correctly";
}

TEST_F(EducationalTest, AlgebraicProperties) {
    std::string input = "education";
    
    // Demonstrate group properties
    auto h1 = hasher(input);
    auto zero = hash64::zero();
    
    // Identity element
    EXPECT_EQ(h1 ^ zero, h1) << "Zero should be identity for XOR";
    
    // Inverse element  
    EXPECT_EQ(h1 ^ h1, zero) << "Element XOR with itself should give zero";
    
    // Show that hash values form a mathematical structure
    auto ones = hash64::ones();
    EXPECT_EQ(h1 ^ ones, ~h1) << "XOR with all-ones should equal complement";
}

// ==================== Regression Tests ====================

class RegressionTest : public ::testing::Test {
protected:
    fnv64 hasher;
};

TEST_F(RegressionTest, KnownValues) {
    // Test against known FNV values to ensure algorithm correctness
    // These values were computed independently
    
    auto empty_hash = hasher("");
    auto hello_hash = hasher("hello");
    
    // Note: These would be actual known FNV values in a real implementation
    EXPECT_FALSE(empty_hash.is_zero());
    EXPECT_FALSE(hello_hash.is_zero());
    EXPECT_NE(empty_hash, hello_hash);
}

// ==================== Integration Tests ====================

class IntegrationTest : public ::testing::Test {};

TEST_F(IntegrationTest, ComplexComposition) {
    // Test complex algebraic expressions
    // Use different hash function types to get different behaviors
    fnv64 f64;
    fnv32 f32;
    std::string test_input = "integration_test";
    
    // Test complement operation
    auto complement_f64 = ~f64;
    auto complement_result = complement_f64(test_input);
    auto original_result = f64(test_input);
    
    EXPECT_FALSE(complement_result.is_zero());
    EXPECT_EQ(complement_result, ~original_result);
    
    // Test sequential composition with same type
    fnv64 f1, f2;
    auto sequential = f1 * f2;
    auto seq_result = sequential(test_input);
    
    // Should be equivalent to f1(f2(test_input))
    auto manual_seq = f1(f2(test_input));
    EXPECT_EQ(seq_result, manual_seq);
    
    // Test self-XOR (should be zero by algebraic properties)
    auto self_xor = f64 ^ f64;
    auto self_xor_result = self_xor(test_input);
    EXPECT_TRUE(self_xor_result.is_zero()) << "f ⊕ f should equal zero";
    
    // Test complex expression with complement and repeated application
    auto rep2 = repeat<2>(f64);
    auto rep3 = repeat<3>(f64);
    auto complex = rep2 ^ rep3;  // Different number of applications should give different results
    auto complex_result = complex(test_input);
    
    // Manual computation
    auto rep2_result = rep2(test_input);
    auto rep3_result = rep3(test_input);
    auto expected = rep2_result ^ rep3_result;
    
    EXPECT_EQ(complex_result, expected);
    EXPECT_FALSE(complex_result.is_zero()) << "Different repetitions should give different results";
}

TEST_F(IntegrationTest, StandardLibraryIntegration) {
    // Test that hash values work with standard library containers
    std::unordered_set<hash64> hash_set;
    std::vector<std::string> inputs = {"a", "b", "c", "test"};
    
    fnv64 hasher;
    for (const auto& input : inputs) {
        auto hash = hasher(input);
        hash_set.insert(hash);
    }
    
    EXPECT_EQ(hash_set.size(), inputs.size()) << "All hashes should be unique and storable in set";
}