#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/algebraic_hashing/core/concepts.hpp"
#include "../include/algebraic_hashing/core/hash_value.hpp"
#include "../include/algebraic_hashing/dsl/algebraic_operations.hpp"
#include "../include/algebraic_hashing/functions/fnv_hash_modern.hpp"
#include "../include/algebraic_hashing/functions/murmur3_hash.hpp"
#include "../include/algebraic_hashing/functions/xxhash.hpp"
#include "../include/algebraic_hashing/functions/sha256_hash.hpp"
#include <string>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <list>
#include <deque>
#include <forward_list>
#include <memory>
#include <optional>
#include <variant>
#include <tuple>
#include <cstdint>
#include <limits>
#include <thread>
#include <atomic>
#include <random>

using namespace algebraic_hashing;
using namespace algebraic_hashing::functions;
using namespace algebraic_hashing::dsl;

// ==================== Advanced Hash Value Tests ====================

class AdvancedHashValueTest : public ::testing::Test {
protected:
    hash128 h128;
    hash256 h256;
    
    void SetUp() override {
        // Initialize with patterns
        for (std::size_t i = 0; i < 16; ++i) {
            h128[i] = static_cast<std::uint8_t>(i * 7);
        }
        for (std::size_t i = 0; i < 32; ++i) {
            h256[i] = static_cast<std::uint8_t>(i * 3);
        }
    }
};

TEST_F(AdvancedHashValueTest, LargeHashValueOperations) {
    // Test 128-bit hash operations
    auto zero128 = hash128::zero();
    auto ones128 = hash128::ones();
    
    EXPECT_TRUE(zero128.is_zero());
    EXPECT_FALSE(ones128.is_zero());
    
    // Test XOR properties with larger hashes
    EXPECT_EQ(h128 ^ zero128, h128);
    EXPECT_EQ(h128 ^ h128, zero128);
    
    // Test 256-bit hash operations
    auto zero256 = hash256::zero();
    auto ones256 = hash256::ones();
    
    EXPECT_TRUE(zero256.is_zero());
    EXPECT_FALSE(ones256.is_zero());
    
    // Test complement and double complement
    auto comp256 = ~h256;
    auto double_comp256 = ~comp256;
    EXPECT_EQ(double_comp256, h256);
    
    // Test hex conversion for large hashes
    auto hex128 = h128.to_hex();
    EXPECT_EQ(hex128.length(), 32); // 16 bytes = 32 hex chars
    
    auto hex256 = h256.to_hex();
    EXPECT_EQ(hex256.length(), 64); // 32 bytes = 64 hex chars
}

TEST_F(AdvancedHashValueTest, EdgeCaseValues) {
    // Test with all possible single-bit patterns
    for (std::size_t bit = 0; bit < 64; ++bit) {
        hash64 single_bit;
        std::size_t byte_idx = bit / 8;
        std::size_t bit_idx = bit % 8;
        single_bit[byte_idx] = static_cast<std::uint8_t>(1 << bit_idx);
        
        EXPECT_EQ(single_bit.popcount(), 1);
        EXPECT_FALSE(single_bit.is_zero());
    }
    
    // Test maximum and minimum values
    hash64 max_hash;
    for (auto& byte : max_hash.data()) {
        byte = 0xFF;
    }
    EXPECT_EQ(max_hash.popcount(), 64);
    
    hash64 min_hash = hash64::zero();
    EXPECT_EQ(min_hash.popcount(), 0);
}

TEST_F(AdvancedHashValueTest, TruncationOperations) {
    // Test truncating larger hashes to smaller ones
    auto truncated_128_to_64 = truncate<8>(h128); // 8 bytes = 64 bits
    EXPECT_EQ(sizeof(truncated_128_to_64), 8);
    
    auto truncated_256_to_128 = truncate<16>(h256); // 16 bytes = 128 bits
    EXPECT_EQ(sizeof(truncated_256_to_128), 16);
    
    // Test that truncation preserves first N bytes
    for (std::size_t i = 0; i < 8; ++i) {
        EXPECT_EQ(truncated_128_to_64[i], h128[i]);
    }
}

TEST_F(AdvancedHashValueTest, ConcatenationEdgeCases) {
    // Concatenate different sized hashes
    hash32 small1, small2;
    for (std::size_t i = 0; i < 4; ++i) {
        small1[i] = static_cast<std::uint8_t>(i);
        small2[i] = static_cast<std::uint8_t>(i + 4);
    }
    
    auto concat_result = concatenate(small1, small2);
    
    // Verify the concatenation
    for (std::size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(concat_result[i], small1[i]);
        EXPECT_EQ(concat_result[i + 4], small2[i]);
    }
    
    // Test multiple concatenations using nested calls
    auto double_concat = concatenate(small1, small2);
    auto triple_concat = concatenate(double_concat, small1);
    EXPECT_EQ(sizeof(triple_concat), 12); // 4 + 4 + 4 bytes
}

// ==================== Advanced DSL Operations Tests ====================

class AdvancedDSLTest : public ::testing::Test {
protected:
    fnv64 f64;
    fnv32 f32;
    std::vector<std::string> test_inputs = {
        "",
        "a",
        "ab",
        "abc",
        std::string(1000, 'x'),
        std::string(10000, 'y'),
        "The quick brown fox jumps over the lazy dog",
        "Unicode test: 你好世界 🌍 مرحبا بالعالم"
    };
};

TEST_F(AdvancedDSLTest, ParallelComposition) {
    // Test parallel composition with two functions
    auto parallel_hash = parallel(f64, f32);
    
    for (const auto& input : test_inputs) {
        auto result = parallel_hash(input);
        
        // Verify it's equivalent to manual concatenation
        auto h1 = f64(input);
        auto h2 = f32(input);
        auto expected = concatenate(h1, h2);
        
        EXPECT_EQ(result, expected) << "Failed for input: " << input;
    }
}

TEST_F(AdvancedDSLTest, NestedCompositions) {
    // Test deeply nested compositions
    auto nested1 = (f64 ^ f64) * (f64 ^ f64);
    auto nested2 = repeat<2>(~f64) ^ repeat<3>(f64);
    auto complex = (nested1 ^ nested2) * (~f64);
    
    for (const auto& input : test_inputs) {
        auto result = complex(input);
        EXPECT_FALSE(result.is_zero()) << "Unexpected zero for: " << input;
    }
}

TEST_F(AdvancedDSLTest, ChainWithManyFunctions) {
    // Test chaining many functions
    fnv64 f1, f2, f3, f4, f5, f6, f7, f8;
    auto chained = chain(f1, f2, f3, f4, f5, f6, f7, f8);
    
    std::string test = "chain_test";
    auto result = chained(test);
    
    // Since all functions are identical, result should be zero
    // (f ^ f ^ f ^ f ^ f ^ f ^ f ^ f = 0 when number is even)
    EXPECT_TRUE(result.is_zero());
    
    // Test with odd number
    auto chained_odd = chain(f1, f2, f3, f4, f5, f6, f7);
    auto result_odd = chained_odd(test);
    
    // With odd number of identical functions, result should be f(test)
    EXPECT_EQ(result_odd, f1(test));
}

TEST_F(AdvancedDSLTest, RepeatedApplicationLimits) {
    // Test repeated application with various counts
    for (std::size_t n = 1; n <= 10; ++n) {
        std::string test = "repeat_test";
        
        // Manual computation
        auto manual = f64(test);
        for (std::size_t i = 1; i < n; ++i) {
            manual = f64(manual);
        }
        
        // Compare based on n
        if (n == 1) {
            auto rep1 = repeat<1>(f64);
            EXPECT_EQ(rep1(test), manual);
        } else if (n == 2) {
            auto rep2 = repeat<2>(f64);
            EXPECT_EQ(rep2(test), manual);
        } else if (n == 3) {
            auto rep3 = repeat<3>(f64);
            EXPECT_EQ(rep3(test), manual);
        }
        // Add more cases as needed
    }
}

TEST_F(AdvancedDSLTest, AnalysisFunctions) {
    // Test collision probability calculation
    double prob = collision_probability(f64, 1000000);
    EXPECT_GT(prob, 0.0);
    EXPECT_LT(prob, 1.0);
    
    // Test hash analysis
    auto analysis = analyze(f64);
    EXPECT_GT(analysis.entropy, 0.0);
    EXPECT_EQ(analysis.output_bits, 64);
    
    // Test expected collisions
    auto expected_collisions = analysis.expected_collisions_for(1000000);
    EXPECT_GT(expected_collisions, 0.0);
}

// ==================== Different Input Type Tests ====================

class InputTypeTest : public ::testing::Test {
protected:
    fnv64 hasher;
};

TEST_F(InputTypeTest, NumericTypes) {
    // Test all numeric types
    EXPECT_FALSE(hasher(int8_t{42}).is_zero());
    EXPECT_FALSE(hasher(uint8_t{42}).is_zero());
    EXPECT_FALSE(hasher(int16_t{42}).is_zero());
    EXPECT_FALSE(hasher(uint16_t{42}).is_zero());
    EXPECT_FALSE(hasher(int32_t{42}).is_zero());
    EXPECT_FALSE(hasher(uint32_t{42}).is_zero());
    EXPECT_FALSE(hasher(int64_t{42}).is_zero());
    EXPECT_FALSE(hasher(uint64_t{42}).is_zero());
    
    // Floating point
    EXPECT_FALSE(hasher(3.14f).is_zero());
    EXPECT_FALSE(hasher(3.14).is_zero());
    EXPECT_FALSE(hasher(3.14L).is_zero());
    
    // Test that different representations give different hashes
    EXPECT_NE(hasher(int32_t{42}), hasher(int64_t{42}));
    EXPECT_NE(hasher(42.0f), hasher(42.0));
}

TEST_F(InputTypeTest, ContainerTypes) {
    // Vectors
    std::vector<int> vec = {1, 2, 3, 4, 5};
    EXPECT_FALSE(hasher(vec).is_zero());
    
    // Arrays
    std::array<int, 5> arr = {1, 2, 3, 4, 5};
    EXPECT_FALSE(hasher(arr).is_zero());
    
    // Lists
    std::list<int> lst = {1, 2, 3, 4, 5};
    EXPECT_FALSE(hasher(lst).is_zero());
    
    // Deques
    std::deque<int> deq = {1, 2, 3, 4, 5};
    EXPECT_FALSE(hasher(deq).is_zero());
    
    // Maps
    std::map<int, std::string> map = {{1, "one"}, {2, "two"}};
    EXPECT_FALSE(hasher(map).is_zero());
    
    // Sets
    std::set<int> set = {1, 2, 3, 4, 5};
    EXPECT_FALSE(hasher(set).is_zero());
}

TEST_F(InputTypeTest, SmartPointers) {
    // Test with raw pointer values (unique_ptr can't be copied)
    int* raw_ptr = new int(42);
    EXPECT_FALSE(hasher(raw_ptr).is_zero());
    delete raw_ptr;
    
    // Shared pointer (is copyable)
    auto sptr = std::make_shared<std::string>("test");
    EXPECT_FALSE(hasher(sptr).is_zero());
    
    // Null shared pointer
    std::shared_ptr<int> null_sptr;
    auto null_sptr_hash = hasher(null_sptr);
    EXPECT_FALSE(null_sptr_hash.is_zero());
    
    // Different shared pointers to same value should hash the same
    auto sptr2 = sptr; // Copy
    EXPECT_EQ(hasher(sptr), hasher(sptr2));
}

TEST_F(InputTypeTest, OptionalAndVariant) {
    // Optional with value
    std::optional<int> opt_value = 42;
    EXPECT_FALSE(hasher(opt_value).is_zero());
    
    // Optional without value
    std::optional<int> opt_empty;
    EXPECT_FALSE(hasher(opt_empty).is_zero());
    
    // These should hash differently
    EXPECT_NE(hasher(opt_value), hasher(opt_empty));
    
    // Variant
    std::variant<int, std::string> var_int = 42;
    std::variant<int, std::string> var_str = "42";
    
    EXPECT_FALSE(hasher(var_int).is_zero());
    EXPECT_FALSE(hasher(var_str).is_zero());
    EXPECT_NE(hasher(var_int), hasher(var_str));
}

TEST_F(InputTypeTest, TuplesAndPairs) {
    // Pairs
    std::pair<int, std::string> p = {42, "answer"};
    EXPECT_FALSE(hasher(p).is_zero());
    
    // Tuples
    std::tuple<int, double, std::string> t = {1, 3.14, "pi"};
    EXPECT_FALSE(hasher(t).is_zero());
    
    // Different tuple orders should give different hashes
    std::tuple<int, int, int> t1 = {1, 2, 3};
    std::tuple<int, int, int> t2 = {3, 2, 1};
    
    EXPECT_NE(hasher(t1), hasher(t2));
}

// ==================== Boundary and Stress Tests ====================

class BoundaryTest : public ::testing::Test {
protected:
    fnv64 hasher;
};

TEST_F(BoundaryTest, ExtremeSizes) {
    // Empty input
    std::string empty;
    EXPECT_FALSE(hasher(empty).is_zero());
    
    // Single byte
    std::string single = "x";
    EXPECT_FALSE(hasher(single).is_zero());
    
    // Very large input (1MB)
    std::string large(1024 * 1024, 'a');
    auto large_hash = hasher(large);
    EXPECT_FALSE(large_hash.is_zero());
    
    // Slightly different large input should give different hash
    large[500000] = 'b';
    auto modified_hash = hasher(large);
    EXPECT_NE(large_hash, modified_hash);
}

TEST_F(BoundaryTest, SpecialCharacters) {
    // Null bytes
    std::string with_null = "hello\0world";
    std::string_view with_null_view(with_null.data(), 11);
    EXPECT_FALSE(hasher(with_null_view).is_zero());
    
    // All control characters
    std::string controls;
    for (char c = 0; c < 32; ++c) {
        controls += c;
    }
    EXPECT_FALSE(hasher(controls).is_zero());
    
    // High ASCII
    std::string high_ascii;
    for (int c = 128; c < 256; ++c) {
        high_ascii += static_cast<char>(c);
    }
    EXPECT_FALSE(hasher(high_ascii).is_zero());
}

TEST_F(BoundaryTest, NumericLimits) {
    // Test with numeric limits
    EXPECT_FALSE(hasher(std::numeric_limits<int>::min()).is_zero());
    EXPECT_FALSE(hasher(std::numeric_limits<int>::max()).is_zero());
    EXPECT_FALSE(hasher(std::numeric_limits<double>::min()).is_zero());
    EXPECT_FALSE(hasher(std::numeric_limits<double>::max()).is_zero());
    EXPECT_FALSE(hasher(std::numeric_limits<double>::infinity()).is_zero());
    EXPECT_FALSE(hasher(-std::numeric_limits<double>::infinity()).is_zero());
    
    // NaN should be hashable
    double nan = std::numeric_limits<double>::quiet_NaN();
    EXPECT_FALSE(hasher(nan).is_zero());
}

// ==================== Algebraic Properties Verification ====================

class AlgebraicPropertiesTest : public ::testing::Test {
protected:
    fnv64 f, g, h;
    std::string test_input = "algebraic_test";
};

TEST_F(AlgebraicPropertiesTest, GroupAxioms) {
    // Test that hash values with XOR form an abelian group
    
    auto hf = f(test_input);
    auto hg = g(test_input);
    auto hh = h(test_input);
    auto zero = hash64::zero();
    
    // Closure: XOR of two hash values is a hash value (always true by type system)
    
    // Associativity: (a ⊕ b) ⊕ c = a ⊕ (b ⊕ c)
    EXPECT_EQ((hf ^ hg) ^ hh, hf ^ (hg ^ hh));
    
    // Identity: a ⊕ 0 = a
    EXPECT_EQ(hf ^ zero, hf);
    EXPECT_EQ(zero ^ hf, hf);
    
    // Inverse: a ⊕ a = 0
    EXPECT_EQ(hf ^ hf, zero);
    
    // Commutativity: a ⊕ b = b ⊕ a
    EXPECT_EQ(hf ^ hg, hg ^ hf);
}

TEST_F(AlgebraicPropertiesTest, DistributiveLaws) {
    auto hf = f(test_input);
    auto hg = g(test_input);
    auto hh = h(test_input);
    
    // AND distributes over XOR: a ∧ (b ⊕ c) = (a ∧ b) ⊕ (a ∧ c)
    EXPECT_EQ(hf & (hg ^ hh), (hf & hg) ^ (hf & hh));
    
    // OR distributes over AND: a ∨ (b ∧ c) = (a ∨ b) ∧ (a ∨ c) 
    // (Note: This is actually not true for OR/AND, but let's test De Morgan's laws instead)
    
    // De Morgan's Laws: ~(a ∧ b) = ~a ∨ ~b
    EXPECT_EQ(~(hf & hg), (~hf) | (~hg));
    
    // De Morgan's Laws: ~(a ∨ b) = ~a ∧ ~b
    EXPECT_EQ(~(hf | hg), (~hf) & (~hg));
}

TEST_F(AlgebraicPropertiesTest, CompositionAssociativity) {
    // Test that function composition is associative
    
    // (f * g) * h should equal f * (g * h)
    auto left_assoc = (f * g) * h;
    auto right_assoc = f * (g * h);
    
    auto left_result = left_assoc(test_input);
    auto right_result = right_assoc(test_input);
    
    EXPECT_EQ(left_result, right_result);
}

// ==================== Performance Characteristics Tests ====================

class PerformanceCharacteristicsTest : public ::testing::Test {
protected:
    fnv64 hasher;
    
    double measure_hash_rate(std::size_t input_size, std::size_t iterations) {
        std::string input(input_size, 'x');
        
        auto start = std::chrono::high_resolution_clock::now();
        for (std::size_t i = 0; i < iterations; ++i) {
            volatile auto result = hasher(input);
            (void)result; // Prevent optimization
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        double total_bytes = input_size * iterations;
        double seconds = duration.count() / 1e9;
        
        return total_bytes / seconds / (1024 * 1024); // MB/s
    }
};

TEST_F(PerformanceCharacteristicsTest, LinearComplexity) {
    // Verify that hash time scales linearly with input size
    const std::size_t iterations = 10000;
    
    auto rate_small = measure_hash_rate(100, iterations);
    auto rate_medium = measure_hash_rate(1000, iterations);
    auto rate_large = measure_hash_rate(10000, iterations);
    
    // Rates should be relatively similar (within 50% of each other)
    // as the algorithm is O(n)
    double min_rate = std::min({rate_small, rate_medium, rate_large});
    double max_rate = std::max({rate_small, rate_medium, rate_large});
    
    EXPECT_LT(max_rate / min_rate, 2.0) 
        << "Hash rate varies too much with input size (not O(n)?)\n"
        << "Small: " << rate_small << " MB/s\n"
        << "Medium: " << rate_medium << " MB/s\n"
        << "Large: " << rate_large << " MB/s";
}

TEST_F(PerformanceCharacteristicsTest, CompositionOverhead) {
    // Measure overhead of composition
    fnv64 f1, f2, f3;
    auto composed = f1 ^ f2 ^ f3;
    
    std::string input(1000, 'x');
    const std::size_t iterations = 100000;
    
    // Measure single hash
    auto start_single = std::chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < iterations; ++i) {
        volatile auto result = f1(input);
        (void)result;
    }
    auto end_single = std::chrono::high_resolution_clock::now();
    auto single_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
        end_single - start_single).count();
    
    // Measure composed hash
    auto start_composed = std::chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < iterations; ++i) {
        volatile auto result = composed(input);
        (void)result;
    }
    auto end_composed = std::chrono::high_resolution_clock::now();
    auto composed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
        end_composed - start_composed).count();
    
    // Composed should take roughly 3x the time (3 hash computations + 2 XORs)
    double ratio = static_cast<double>(composed_time) / single_time;
    
    EXPECT_GT(ratio, 2.5) << "Composition seems too fast";
    EXPECT_LT(ratio, 4.0) << "Composition overhead too high";
}

// ==================== Regression Tests ====================

/**
 * @brief Regression test for stack-use-after-return bug in get_arithmetic_bytes()
 *
 * This test catches a bug where get_arithmetic_bytes() returned a span
 * pointing to a local array that was destroyed when the function returned.
 *
 * The bug manifested when hashing numeric types (integers, floats) because
 * the hash function would read from deallocated stack memory, causing
 * undefined behavior.
 *
 * This test verifies:
 * 1. Numeric hashes are deterministic (same input produces same output)
 * 2. Numeric hashes are stable across multiple calls
 * 3. Different numeric values produce different hashes
 */
TEST(RegressionTest, NumericHashingConsistency) {
    fnv64 hasher;

    // Test integer types - hash the same value multiple times
    const int32_t test_int = 42;
    auto hash1 = hasher(test_int);
    auto hash2 = hasher(test_int);
    auto hash3 = hasher(test_int);

    // All hashes of the same value must be identical
    EXPECT_EQ(hash1, hash2) << "Integer hashing is not deterministic";
    EXPECT_EQ(hash2, hash3) << "Integer hashing is not stable";

    // Different values must produce different hashes
    const int32_t test_int2 = 43;
    auto hash4 = hasher(test_int2);
    EXPECT_NE(hash1, hash4) << "Different integers produce same hash";

    // Test floating point types
    const double test_double = 3.14159;
    auto double_hash1 = hasher(test_double);
    auto double_hash2 = hasher(test_double);
    auto double_hash3 = hasher(test_double);

    EXPECT_EQ(double_hash1, double_hash2) << "Double hashing is not deterministic";
    EXPECT_EQ(double_hash2, double_hash3) << "Double hashing is not stable";

    const double test_double2 = 2.71828;
    auto double_hash4 = hasher(test_double2);
    EXPECT_NE(double_hash1, double_hash4) << "Different doubles produce same hash";

    // Test various integer sizes to ensure byte representation is correct
    std::vector<std::pair<std::int64_t, hash64>> test_cases;
    for (int i = 0; i < 100; ++i) {
        std::int64_t value = i * 12345;
        auto hash = hasher(value);
        test_cases.emplace_back(value, hash);
    }

    // Verify consistency - hash the same values again
    for (const auto& [value, expected_hash] : test_cases) {
        auto rehash = hasher(value);
        EXPECT_EQ(rehash, expected_hash)
            << "Inconsistent hash for value " << value;
    }

    // Test edge cases that might trigger stack corruption
    EXPECT_EQ(hasher(0), hasher(0));
    EXPECT_EQ(hasher(-1), hasher(-1));
    EXPECT_EQ(hasher(std::numeric_limits<int64_t>::max()),
              hasher(std::numeric_limits<int64_t>::max()));
    EXPECT_EQ(hasher(std::numeric_limits<int64_t>::min()),
              hasher(std::numeric_limits<int64_t>::min()));
}

/**
 * @brief Regression test for floating-point byte representation bug
 *
 * Ensures that floating-point values are consistently converted to bytes
 * and that the hash function correctly handles their representation.
 */
TEST(RegressionTest, FloatingPointByteRepresentation) {
    fnv64 hasher;
    fnv32 hasher32;

    // Test that NaN, infinity, etc. are handled consistently
    const float test_vals_f[] = {
        0.0f, -0.0f, 1.0f, -1.0f,
        3.14159f, 2.71828f,
        std::numeric_limits<float>::min(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::epsilon()
    };

    for (float val : test_vals_f) {
        auto hash1 = hasher(val);
        auto hash2 = hasher(val);
        EXPECT_EQ(hash1, hash2) << "Float " << val << " hashing not deterministic";
    }

    const double test_vals_d[] = {
        0.0, -0.0, 1.0, -1.0,
        3.14159265358979323846, 2.71828182845904523536,
        std::numeric_limits<double>::min(),
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::epsilon()
    };

    for (double val : test_vals_d) {
        auto hash1 = hasher(val);
        auto hash2 = hasher(val);
        EXPECT_EQ(hash1, hash2) << "Double " << val << " hashing not deterministic";

        // Also test with 32-bit hasher
        auto hash32_1 = hasher32(val);
        auto hash32_2 = hasher32(val);
        EXPECT_EQ(hash32_1, hash32_2) << "Double " << val << " 32-bit hashing not deterministic";
    }
}

// ==================== Thread Safety Tests ====================

/**
 * @brief Test that hash functions are thread-safe for concurrent use
 *
 * Hash functions should produce deterministic results even when called
 * concurrently from multiple threads. This tests:
 * 1. No data races during concurrent hashing
 * 2. Deterministic results across threads
 * 3. Thread-safe initialization and usage
 */
TEST(ThreadSafetyTest, ConcurrentHashingDeterminism) {
    fnv64 hasher;
    constexpr int num_threads = 8;
    constexpr int iterations_per_thread = 1000;

    // Test data
    std::vector<std::string> test_strings = {
        "hello", "world", "concurrent", "hashing",
        "thread", "safety", "test", "data"
    };

    // Expected hashes (computed once)
    std::vector<hash64> expected_hashes;
    for (const auto& str : test_strings) {
        expected_hashes.push_back(hasher(str));
    }

    // Storage for results from all threads
    std::vector<std::thread> threads;
    std::atomic<int> errors{0};

    // Lambda for each thread
    auto hash_worker = [&](int thread_id) {
        fnv64 local_hasher; // Each thread has its own hasher instance

        for (int i = 0; i < iterations_per_thread; ++i) {
            for (size_t j = 0; j < test_strings.size(); ++j) {
                auto result = local_hasher(test_strings[j]);

                // Verify result matches expected hash
                if (result != expected_hashes[j]) {
                    errors.fetch_add(1, std::memory_order_relaxed);
                }
            }
        }
    };

    // Launch threads
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(hash_worker, i);
    }

    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }

    // Verify no errors occurred
    EXPECT_EQ(errors.load(), 0)
        << "Concurrent hashing produced inconsistent results";
}

/**
 * @brief Test thread safety with shared hasher instance
 *
 * Tests whether a single hasher instance can be safely used from
 * multiple threads simultaneously (stateless operation).
 */
TEST(ThreadSafetyTest, SharedHasherInstance) {
    // Create a single shared hasher
    const fnv64 hasher; // const to ensure read-only access

    constexpr int num_threads = 16;
    constexpr int iterations = 500;

    std::atomic<int> mismatches{0};
    std::vector<std::thread> threads;

    // Expected result for test string
    const std::string test_str = "shared_hasher_test";
    const auto expected = hasher(test_str);

    // Lambda for concurrent access
    auto worker = [&]() {
        for (int i = 0; i < iterations; ++i) {
            auto result = hasher(test_str);
            if (result != expected) {
                mismatches.fetch_add(1, std::memory_order_relaxed);
            }

            // Also test with different data
            auto result2 = hasher(std::to_string(i));
            // Just verify it doesn't crash - result varies by i
            EXPECT_FALSE(result2.is_zero());
        }
    };

    // Launch threads
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker);
    }

    // Join all threads
    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(mismatches.load(), 0)
        << "Shared hasher produced inconsistent results";
}

/**
 * @brief Stress test for concurrent hash composition
 *
 * Tests that algebraic operations (XOR, composition) are thread-safe.
 */
TEST(ThreadSafetyTest, ConcurrentComposition) {
    fnv64 h1;
    fnv64 h2;

    // Create composed hash function
    auto composed = h1 ^ h2;

    constexpr int num_threads = 8;
    constexpr int iterations = 1000;

    std::atomic<int> errors{0};
    std::vector<std::thread> threads;

    const std::string test_data = "composition_test";
    const auto expected = composed(test_data);

    auto worker = [&]() {
        for (int i = 0; i < iterations; ++i) {
            auto result = composed(test_data);
            if (result != expected) {
                errors.fetch_add(1, std::memory_order_relaxed);
            }
        }
    };

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(errors.load(), 0)
        << "Concurrent composition produced inconsistent results";
}

/**
 * @brief Test thread safety of hash value operations
 *
 * Ensures that hash value algebraic operations are thread-safe.
 */
TEST(ThreadSafetyTest, ConcurrentHashValueOperations) {
    constexpr int num_threads = 8;
    constexpr int iterations = 1000;

    // Create base hash values
    hash64 h1;
    hash64 h2;
    for (int i = 0; i < 8; ++i) {
        h1[i] = static_cast<uint8_t>(i * 17);
        h2[i] = static_cast<uint8_t>(i * 23);
    }

    // Expected results
    const auto expected_xor = h1 ^ h2;
    const auto expected_complement = ~h1;

    std::atomic<int> errors{0};
    std::vector<std::thread> threads;

    auto worker = [&]() {
        for (int i = 0; i < iterations; ++i) {
            // Test XOR
            auto result_xor = h1 ^ h2;
            if (result_xor != expected_xor) {
                errors.fetch_add(1, std::memory_order_relaxed);
            }

            // Test complement
            auto result_complement = ~h1;
            if (result_complement != expected_complement) {
                errors.fetch_add(1, std::memory_order_relaxed);
            }

            // Test that operations don't modify originals
            EXPECT_EQ(h1[0], static_cast<uint8_t>(0 * 17));
            EXPECT_EQ(h2[0], static_cast<uint8_t>(0 * 23));
        }
    };

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(errors.load(), 0)
        << "Concurrent hash value operations produced errors";
}

/**
 * @brief Test for data races using ThreadSanitizer
 *
 * This test is designed to trigger ThreadSanitizer if there are any
 * data races in the hash function implementation. It creates heavy
 * contention by having many threads hash the same data simultaneously.
 */
TEST(ThreadSafetyTest, NoDataRaces) {
    constexpr int num_threads = 32; // High thread count for contention
    constexpr int iterations = 100;

    fnv64 shared_hasher;
    std::vector<std::thread> threads;

    // Shared test data (read-only, safe to share)
    const std::vector<std::string> shared_data = {
        "test1", "test2", "test3", "test4", "test5"
    };

    // Counter for completed operations (atomic for thread safety)
    std::atomic<uint64_t> operations_completed{0};

    auto worker = [&]() {
        for (int i = 0; i < iterations; ++i) {
            for (const auto& data : shared_data) {
                // This should not cause data races
                volatile auto result = shared_hasher(data);
                (void)result; // Suppress unused variable warning

                operations_completed.fetch_add(1, std::memory_order_relaxed);
            }
        }
    };

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // Verify all operations completed
    EXPECT_EQ(operations_completed.load(),
              num_threads * iterations * shared_data.size());
}

// ==================== FNV Official Test Vectors ====================

/**
 * @brief Test FNV-1a implementation against official test vectors
 *
 * These test vectors are from the official FNV specification to ensure
 * our implementation is correct and matches the standard.
 *
 * Source: http://www.isthe.com/chongo/tech/comp/fnv/
 */
TEST(FNVTestVectors, FNV1a_64bit_OfficialVectors) {
    fnv64 hasher;

    // Official FNV-1a 64-bit test vectors
    struct TestVector {
        std::string input;
        uint64_t expected_hash;
    };

    std::vector<TestVector> test_vectors = {
        // Empty string (official test vector)
        {"", 0xcbf29ce484222325ULL},

        // Single characters (official test vectors)
        {"a", 0xaf63dc4c8601ec8cULL},
        {"b", 0xaf63df4c8601f1a5ULL},
        {"c", 0xaf63de4c8601eff2ULL},

        // Common test strings (official test vector)
        {"foobar", 0x85944171f73967e8ULL},

        // Additional test strings (computed with correct FNV-1a)
        {"chongo was here!\n", 0x46810940eff5f915ULL},
        {"chongo <Landon Curt Noll> /\\../\\", 0x2c8f4c9af81bcf06ULL},

        // Numeric strings
        {"127.0.0.1", 0xaabafe7104d914beULL},

        // Special characters
        {"\n", 0xaf63c74c8601c8ddULL},
        {"\r", 0xaf63c04c8601bcf8ULL},
        {"\r\n", 0x083cb407b4f40f36ULL},
    };

    for (const auto& tv : test_vectors) {
        auto result = hasher(tv.input);

        // Convert hash_value<8> to uint64_t for comparison
        uint64_t result_u64 = 0;
        std::memcpy(&result_u64, result.data().data(), sizeof(uint64_t));

        EXPECT_EQ(result_u64, tv.expected_hash)
            << "FNV-1a hash mismatch for input: \"" << tv.input << "\"";
    }
}

/**
 * @brief Test FNV-1a 32-bit implementation against official test vectors
 */
TEST(FNVTestVectors, FNV1a_32bit_OfficialVectors) {
    fnv32 hasher;

    // Official FNV-1a 32-bit test vectors
    struct TestVector {
        std::string input;
        uint32_t expected_hash;
    };

    std::vector<TestVector> test_vectors = {
        // Empty string (official test vector)
        {"", 0x811c9dc5U},

        // Single characters (official test vectors)
        {"a", 0xe40c292cU},
        {"b", 0xe70c2de5U},
        {"c", 0xe60c2c52U},

        // Common test strings (official test vector)
        {"foobar", 0xbf9cf968U},

        // Longer strings (computed with correct FNV-1a)
        {"chongo was here!\n", 0xd49930d5U},

        // Numeric strings
        {"127.0.0.1", 0x08a3d11eU},
    };

    for (const auto& tv : test_vectors) {
        auto result = hasher(tv.input);

        // Convert hash_value<4> to uint32_t for comparison
        uint32_t result_u32 = 0;
        std::memcpy(&result_u32, result.data().data(), sizeof(uint32_t));

        EXPECT_EQ(result_u32, tv.expected_hash)
            << "FNV-1a 32-bit hash mismatch for input: \"" << tv.input << "\"";
    }
}

/**
 * @brief Test FNV-1a with binary data
 *
 * Ensures the hash function correctly handles non-text binary data.
 */
TEST(FNVTestVectors, BinaryData) {
    fnv64 hasher;

    // Binary test data including null bytes
    std::vector<uint8_t> binary_data = {0x00, 0x01, 0x02, 0xFF, 0xFE, 0x00};

    // Hash as span
    auto result1 = hasher(std::span<const uint8_t>(binary_data.data(), binary_data.size()));

    // Hash the same data again
    auto result2 = hasher(std::span<const uint8_t>(binary_data.data(), binary_data.size()));

    // Should be deterministic
    EXPECT_EQ(result1, result2);

    // Different data should produce different hash
    std::vector<uint8_t> different_data = {0x00, 0x01, 0x02, 0xFF, 0xFE, 0x01}; // Last byte differs
    auto result3 = hasher(std::span<const uint8_t>(different_data.data(), different_data.size()));

    EXPECT_NE(result1, result3);
}

/**
 * @brief Test FNV with long strings
 *
 * Verifies correct behavior with longer inputs.
 */
TEST(FNVTestVectors, LongStrings) {
    fnv64 hasher;

    // Create a long string
    std::string long_str(1000, 'a');
    auto hash1 = hasher(long_str);

    // Change one character in the middle
    long_str[500] = 'b';
    auto hash2 = hasher(long_str);

    // Should produce different hashes (avalanche effect)
    EXPECT_NE(hash1, hash2);

    // Hash should be deterministic
    auto hash3 = hasher(long_str);
    EXPECT_EQ(hash2, hash3);
}

/**
 * @brief Test FNV with incremental construction
 *
 * Ensures that hashing different chunks produces different results
 * (FNV is not incremental, each call is independent).
 */
TEST(FNVTestVectors, NonIncremental) {
    fnv64 hasher;

    std::string part1 = "hello";
    std::string part2 = "world";
    std::string combined = "helloworld";

    auto hash_combined = hasher(combined);
    auto hash_part1 = hasher(part1);
    auto hash_part2 = hasher(part2);

    // Hashing separately should not equal hashing together
    // (FNV is not designed for incremental hashing)
    EXPECT_NE(hash_part1, hash_combined);
    EXPECT_NE(hash_part2, hash_combined);
}

// ==================== Property-Based Algebraic Tests ====================

/**
 * @brief Test algebraic group properties for hash values under XOR
 *
 * Hash values form an abelian group (H, ^) where:
 * - Closure: h1 ^ h2 is in H
 * - Associativity: (h1 ^ h2) ^ h3 = h1 ^ (h2 ^ h3)
 * - Identity: h ^ zero = h
 * - Inverse: h ^ h = zero
 * - Commutativity: h1 ^ h2 = h2 ^ h1
 */
TEST(AlgebraicProperties, HashValueGroupProperties) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<uint8_t> dist(0, 255);

    // Generate random hash values
    auto make_random_hash = [&]() {
        hash64 h;
        for (auto& byte : h.data()) {
            byte = dist(rng);
        }
        return h;
    };

    for (int trial = 0; trial < 100; ++trial) {
        hash64 h1 = make_random_hash();
        hash64 h2 = make_random_hash();
        hash64 h3 = make_random_hash();
        hash64 zero = hash64::zero();

        // Closure (implicit - type system ensures this)

        // Associativity: (h1 ^ h2) ^ h3 = h1 ^ (h2 ^ h3)
        EXPECT_EQ((h1 ^ h2) ^ h3, h1 ^ (h2 ^ h3))
            << "Associativity failed on trial " << trial;

        // Identity: h ^ zero = h
        EXPECT_EQ(h1 ^ zero, h1) << "Identity failed on trial " << trial;
        EXPECT_EQ(zero ^ h1, h1) << "Left identity failed on trial " << trial;

        // Inverse: h ^ h = zero
        EXPECT_EQ(h1 ^ h1, zero) << "Self-inverse failed on trial " << trial;

        // Commutativity: h1 ^ h2 = h2 ^ h1
        EXPECT_EQ(h1 ^ h2, h2 ^ h1) << "Commutativity failed on trial " << trial;
    }
}

/**
 * @brief Test complement as involution
 *
 * The complement operator ~ is an involution: ~~h = h
 */
TEST(AlgebraicProperties, ComplementInvolution) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<uint8_t> dist(0, 255);

    auto make_random_hash = [&]() {
        hash64 h;
        for (auto& byte : h.data()) {
            byte = dist(rng);
        }
        return h;
    };

    for (int trial = 0; trial < 100; ++trial) {
        hash64 h = make_random_hash();

        // Double complement is identity
        EXPECT_EQ(~~h, h) << "Involution failed on trial " << trial;

        // Complement XOR original gives all ones
        auto all_ones = hash64::ones();
        EXPECT_EQ(h ^ ~h, all_ones) << "h ^ ~h != ones on trial " << trial;
    }
}

/**
 * @brief Test XOR composition commutativity for hash functions
 */
TEST(AlgebraicProperties, XORCompositionCommutativity) {
    fnv64 f;
    murmur3_64 g;

    std::vector<std::string> test_inputs = {
        "", "a", "hello", "world", "test123",
        std::string(100, 'x'),
        "The quick brown fox jumps over the lazy dog"
    };

    auto fg = f ^ g;
    auto gf = g ^ f;

    for (const auto& input : test_inputs) {
        EXPECT_EQ(fg(input), gf(input))
            << "XOR composition not commutative for input: " << input;
    }
}

/**
 * @brief Test XOR composition associativity for hash functions
 */
TEST(AlgebraicProperties, XORCompositionAssociativity) {
    fnv64 f;
    murmur3_64 g;
    xxhash64 h;

    std::vector<std::string> test_inputs = {
        "", "a", "hello", "test123"
    };

    auto fg_h = (f ^ g) ^ h;
    auto f_gh = f ^ (g ^ h);

    for (const auto& input : test_inputs) {
        EXPECT_EQ(fg_h(input), f_gh(input))
            << "XOR composition not associative for input: " << input;
    }
}

/**
 * @brief Test that same hasher XOR'd with itself produces zero
 */
TEST(AlgebraicProperties, SelfInverseComposition) {
    fnv64 f;

    std::vector<std::string> test_inputs = {
        "", "a", "hello", "world", "test"
    };

    auto ff = f ^ f;

    for (const auto& input : test_inputs) {
        auto result = ff(input);
        EXPECT_TRUE(result.is_zero())
            << "f ^ f should produce zero for input: " << input;
    }
}

// ==================== New Hash Function Tests ====================

/**
 * @brief Basic tests for MurmurHash3
 */
TEST(MurmurHash3Test, BasicFunctionality) {
    murmur3_64 hasher;

    // Determinism
    EXPECT_EQ(hasher("hello"), hasher("hello"));
    EXPECT_EQ(hasher("world"), hasher("world"));

    // Different inputs produce different outputs
    EXPECT_NE(hasher("hello"), hasher("world"));
    EXPECT_NE(hasher("test"), hasher("Test"));

    // Empty string hash is consistent (MurmurHash3 with seed=0 can produce 0 for empty input)
    auto empty_hash = hasher("");
    EXPECT_EQ(empty_hash, hasher(""));  // Consistency check
}

/**
 * @brief Test MurmurHash3 seeding
 */
TEST(MurmurHash3Test, Seeding) {
    murmur3_64 h1(0);
    murmur3_64 h2(1);
    murmur3_64 h3(42);

    std::string input = "test_string";

    // Different seeds produce different hashes
    EXPECT_NE(h1(input), h2(input));
    EXPECT_NE(h2(input), h3(input));
    EXPECT_NE(h1(input), h3(input));

    // Same seed is deterministic
    murmur3_64 h1_copy(0);
    EXPECT_EQ(h1(input), h1_copy(input));
}

/**
 * @brief Test MurmurHash3 variants
 */
TEST(MurmurHash3Test, Variants) {
    murmur3_32 h32;
    murmur3_64 h64;
    murmur3_128 h128;

    std::string input = "test";

    auto r32 = h32(input);
    auto r64 = h64(input);
    auto r128 = h128(input);

    EXPECT_EQ(r32.to_hex().length(), 8);   // 4 bytes = 8 hex chars
    EXPECT_EQ(r64.to_hex().length(), 16);  // 8 bytes = 16 hex chars
    EXPECT_EQ(r128.to_hex().length(), 32); // 16 bytes = 32 hex chars
}

/**
 * @brief Basic tests for xxHash
 */
TEST(XXHashTest, BasicFunctionality) {
    xxhash64 hasher;

    // Determinism
    EXPECT_EQ(hasher("hello"), hasher("hello"));
    EXPECT_EQ(hasher("world"), hasher("world"));

    // Different inputs produce different outputs
    EXPECT_NE(hasher("hello"), hasher("world"));
    EXPECT_NE(hasher("test"), hasher("Test"));

    // Empty string hash is consistent
    auto empty_hash = hasher("");
    EXPECT_EQ(empty_hash, hasher(""));  // Consistency check
}

/**
 * @brief Test xxHash seeding
 */
TEST(XXHashTest, Seeding) {
    xxhash64 h1(0);
    xxhash64 h2(1);
    xxhash64 h3(12345);

    std::string input = "test_string";

    // Different seeds produce different hashes
    EXPECT_NE(h1(input), h2(input));
    EXPECT_NE(h2(input), h3(input));
    EXPECT_NE(h1(input), h3(input));

    // Same seed is deterministic
    xxhash64 h1_copy(0);
    EXPECT_EQ(h1(input), h1_copy(input));
}

/**
 * @brief Test xxHash variants
 */
TEST(XXHashTest, Variants) {
    xxhash32 h32;
    xxhash64 h64;

    std::string input = "test";

    auto r32 = h32(input);
    auto r64 = h64(input);

    EXPECT_EQ(r32.to_hex().length(), 8);   // 4 bytes = 8 hex chars
    EXPECT_EQ(r64.to_hex().length(), 16);  // 8 bytes = 16 hex chars
}

/**
 * @brief Basic tests for SHA-256
 */
TEST(SHA256Test, BasicFunctionality) {
    sha256 hasher;

    // Determinism
    EXPECT_EQ(hasher("hello"), hasher("hello"));
    EXPECT_EQ(hasher("world"), hasher("world"));

    // Different inputs produce different outputs
    EXPECT_NE(hasher("hello"), hasher("world"));
    EXPECT_NE(hasher("test"), hasher("Test"));

    // Correct output size
    auto result = hasher("test");
    EXPECT_EQ(result.to_hex().length(), 64); // 32 bytes = 64 hex chars
}

/**
 * @brief Test SHA-256 against known test vectors
 */
TEST(SHA256Test, KnownTestVectors) {
    sha256 hasher;

    // Test vector: empty string
    auto empty_hash = hasher("");
    EXPECT_EQ(empty_hash.to_hex(),
              "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");

    // Test vector: "abc"
    auto abc_hash = hasher("abc");
    EXPECT_EQ(abc_hash.to_hex(),
              "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");

    // Test vector: "hello"
    auto hello_hash = hasher("hello");
    EXPECT_EQ(hello_hash.to_hex(),
              "2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824");
}

/**
 * @brief Test SHA-256 streaming interface
 */
TEST(SHA256Test, StreamingInterface) {
    sha256 streaming;
    sha256 oneshot;

    std::string part1 = "hello";
    std::string part2 = "world";
    std::string combined = "helloworld";

    // One-shot hash
    auto result1 = oneshot(combined);

    // Streaming hash
    streaming.reset();
    streaming.update(reinterpret_cast<const uint8_t*>(part1.data()), part1.size());
    streaming.update(reinterpret_cast<const uint8_t*>(part2.data()), part2.size());
    auto result2 = streaming.finalize();

    EXPECT_EQ(result1, result2);
}

// ==================== Cross-Hash Function Composition Tests ====================

/**
 * @brief Test that different hash functions can be composed
 */
TEST(CompositionTest, CrossFunctionXOR) {
    fnv64 fnv;
    murmur3_64 murmur;
    xxhash64 xx;

    // All compositions should work
    auto fnv_murmur = fnv ^ murmur;
    auto fnv_xx = fnv ^ xx;
    auto murmur_xx = murmur ^ xx;
    auto all_three = fnv ^ murmur ^ xx;

    std::string input = "test";

    // All should produce valid hashes
    auto r1 = fnv_murmur(input);
    auto r2 = fnv_xx(input);
    auto r3 = murmur_xx(input);
    auto r4 = all_three(input);

    // None should be zero (extremely unlikely)
    EXPECT_FALSE(r1.is_zero());
    EXPECT_FALSE(r2.is_zero());
    EXPECT_FALSE(r3.is_zero());
    EXPECT_FALSE(r4.is_zero());

    // All should be different (extremely likely)
    EXPECT_NE(r1, r2);
    EXPECT_NE(r2, r3);
    EXPECT_NE(r3, r4);
}

/**
 * @brief Test sequential composition with different hash functions
 */
TEST(CompositionTest, SequentialComposition) {
    fnv64 fnv;
    murmur3_64 murmur;

    auto fnv_then_murmur = fnv * murmur;  // fnv(murmur(x))
    auto murmur_then_fnv = murmur * fnv;  // murmur(fnv(x))

    std::string input = "test";

    auto r1 = fnv_then_murmur(input);
    auto r2 = murmur_then_fnv(input);

    // Order matters - should be different
    EXPECT_NE(r1, r2);

    // Should be deterministic
    EXPECT_EQ(fnv_then_murmur(input), fnv_then_murmur(input));
}

// ==================== Avalanche Effect Tests ====================

/**
 * @brief Test avalanche effect - small changes should flip ~50% of bits
 */
TEST(AvalancheTest, SingleBitChange) {
    auto test_avalanche = [](auto hasher, const std::string& name) {
        std::string base = "test_avalanche_string";
        auto base_hash = hasher(base);

        int total_bit_flips = 0;
        int total_tests = 0;

        // Test single character changes
        for (size_t i = 0; i < base.size(); ++i) {
            std::string modified = base;
            modified[i] = static_cast<char>((modified[i] + 1) % 256);

            auto modified_hash = hasher(modified);
            auto diff = base_hash ^ modified_hash;
            total_bit_flips += diff.popcount();
            total_tests++;
        }

        double average_flip_rate = static_cast<double>(total_bit_flips) /
                                   (total_tests * 64);  // 64 bits

        // Good avalanche should flip around 50% of bits
        // Accept 30-70% as reasonable
        EXPECT_GT(average_flip_rate, 0.30)
            << name << " avalanche too low: " << average_flip_rate;
        EXPECT_LT(average_flip_rate, 0.70)
            << name << " avalanche too high: " << average_flip_rate;
    };

    test_avalanche(fnv64{}, "FNV64");
    test_avalanche(murmur3_64{}, "MurmurHash3");
    test_avalanche(xxhash64{}, "xxHash64");
}

// ==================== Distribution Tests ====================

/**
 * @brief Test hash distribution - outputs should be roughly uniform
 */
TEST(DistributionTest, ByteDistribution) {
    auto test_distribution = [](auto hasher, const std::string& name) {
        std::array<int, 256> byte_counts{};

        // Hash many strings, count first byte distribution
        for (int i = 0; i < 10000; ++i) {
            auto hash = hasher(std::to_string(i));
            byte_counts[hash[0]]++;
        }

        // Expected count per byte value: 10000 / 256 ≈ 39
        double expected = 10000.0 / 256.0;

        // Calculate chi-squared statistic
        double chi_squared = 0;
        for (int count : byte_counts) {
            double diff = count - expected;
            chi_squared += (diff * diff) / expected;
        }

        // Chi-squared with 255 df, p=0.01: 310.5
        // Accept if less than this (not terribly biased)
        EXPECT_LT(chi_squared, 400.0)
            << name << " distribution too biased, chi-squared: " << chi_squared;
    };

    test_distribution(fnv64{}, "FNV64");
    test_distribution(murmur3_64{}, "MurmurHash3");
    test_distribution(xxhash64{}, "xxHash64");
}

// ==================== Main function ====================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}