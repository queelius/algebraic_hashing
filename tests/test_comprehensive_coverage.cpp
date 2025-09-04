#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/algebraic_hashing/core/concepts.hpp"
#include "../include/algebraic_hashing/core/hash_value.hpp"
#include "../include/algebraic_hashing/dsl/algebraic_operations.hpp"
#include "../include/algebraic_hashing/functions/fnv_hash_modern.hpp"
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

// ==================== Main function ====================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}