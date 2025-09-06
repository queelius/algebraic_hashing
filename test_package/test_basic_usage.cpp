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

// Test basic includes work
#include <algebraic_hashing/core/hash_value.hpp>
#include <algebraic_hashing/core/concepts.hpp>
#include <algebraic_hashing/functions/fnv_hash_modern.hpp>

using namespace algebraic_hashing;

/**
 * Test basic hash value operations
 */
void test_hash_value_basic() {
    std::cout << "Testing hash value basic operations..." << std::endl;
    
    // Test construction
    hash_value<64> h1;
    hash_value<64> h2;
    
    // Test copy and assignment
    auto h3 = h1;
    
    // Test XOR operation
    auto h4 = h1 ^ h2;
    
    std::cout << "✓ Hash value operations passed" << std::endl;
}

/**
 * Test FNV hash function
 */
void test_fnv_hash() {
    std::cout << "Testing FNV hash function..." << std::endl;
    
    // Test FNV hash function creation
    auto fnv = functions::fnv64{};
    
    // Test hashing
    std::string test_str = "Hello, World!";
    auto result = fnv(test_str);
    
    std::cout << "✓ FNV hash function passed" << std::endl;
}

int main() {
    std::cout << "=== AlgebraicHashing Conan Package Test ===" << std::endl;
    
    try {
        test_hash_value_basic();
        test_fnv_hash();
        
        std::cout << "\n✓ All tests passed! Package is working correctly." << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "✗ Test failed: " << e.what() << std::endl;
        return 1;
    }
}