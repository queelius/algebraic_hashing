/**
 * @file test_basic_usage.cpp
 * @brief Basic usage test for AlgebraicHashing package validation
 * 
 * This test validates that the AlgebraicHashing package can be properly
 * consumed through Conan package manager. It tests basic functionality
 * to ensure the library is correctly installed and usable.
 */

#include <iostream>
#include <string>
#include <cassert>

// Test basic includes work
#include <algebraic_hashing/core/hash_value.hpp>
#include <algebraic_hashing/core/concepts.hpp>
#include <algebraic_hashing/functions/fnv_hash_modern.hpp>
#include <algebraic_hashing/dsl/algebraic_operations.hpp>

using namespace algebraic_hashing;

/**
 * Test basic hash value operations
 */
void test_hash_value_basic() {
    std::cout << "Testing hash value basic operations..." << std::endl;
    
    // Test construction and basic operations
    auto h1 = hash_value<64>::from_string("Hello, World!");
    auto h2 = hash_value<64>::from_string("Hello, Conan!");
    auto h3 = hash_value<64>::zero();
    
    // Test XOR operation (abelian group property)
    auto h4 = h1 ^ h2;
    auto h5 = h4 ^ h2;  // Should equal h1
    assert(h5 == h1);
    
    // Test zero element
    auto h6 = h1 ^ h1;
    assert(h6 == h3);
    
    std::cout << "✓ Hash value basic operations passed" << std::endl;
}

/**
 * Test FNV hash function
 */
void test_fnv_hash() {
    std::cout << "Testing FNV hash function..." << std::endl;
    
    auto fnv = fnv64{};
    
    // Test basic hashing
    std::string test_string = "Conan package test";
    auto hash1 = fnv(test_string);
    auto hash2 = fnv(test_string);
    
    // Should be deterministic
    assert(hash1 == hash2);
    
    // Different strings should produce different hashes (with high probability)
    auto hash3 = fnv("Different string");
    assert(hash1 != hash3);
    
    std::cout << "✓ FNV hash function tests passed" << std::endl;
}

/**
 * Test algebraic composition
 */
void test_algebraic_composition() {
    std::cout << "Testing algebraic composition..." << std::endl;
    
    auto fnv1 = fnv64{};
    auto fnv2 = fnv32{};
    
    // Test XOR composition
    auto composed = fnv1 ^ fnv2;
    
    std::string test_input = "Algebraic composition test";
    auto result = composed(test_input);
    
    // Should produce a valid hash value
    assert(result != hash_value<64>::zero() || test_input.empty());
    
    std::cout << "✓ Algebraic composition tests passed" << std::endl;
}

/**
 * Test concepts work correctly
 */
void test_concepts() {
    std::cout << "Testing concepts..." << std::endl;
    
    // Test that our types satisfy the concepts
    static_assert(Hashable<std::string>);
    static_assert(Hashable<int>);
    static_assert(HashValue<hash_value<64>>);
    static_assert(ComposableHashFunction<fnv64>);
    
    std::cout << "✓ Concepts validation passed" << std::endl;
}

/**
 * Main test function
 */
int main() {
    std::cout << "=== AlgebraicHashing Package Validation Test ===" << std::endl;
    std::cout << "Testing basic package functionality..." << std::endl << std::endl;
    
    try {
        test_hash_value_basic();
        test_fnv_hash();
        test_algebraic_composition();
        test_concepts();
        
        std::cout << std::endl;
        std::cout << "🎉 All tests passed! AlgebraicHashing package is working correctly." << std::endl;
        std::cout << "Package version: AlgebraicHashing 2.0.0" << std::endl;
        std::cout << "C++ Standard: C++20" << std::endl;
        std::cout << "Package type: Header-only library" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}