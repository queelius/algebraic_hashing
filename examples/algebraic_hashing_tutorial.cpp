/**
 * @file algebraic_hashing_tutorial.cpp
 * @brief Comprehensive tutorial demonstrating algebraic hashing concepts
 * 
 * This tutorial serves as both documentation and example code, showing how
 * to use the algebraic hashing library to compose, analyze, and understand
 * hash functions from a mathematical perspective.
 * 
 * Topics covered:
 * 1. Basic hash function usage
 * 2. Hash value operations and properties  
 * 3. Algebraic composition of hash functions
 * 4. Performance analysis and statistics
 * 5. Educational insights into hash function behavior
 */

#include "../include/algebraic_hashing/core/concepts.hpp"
#include "../include/algebraic_hashing/core/hash_value.hpp"
#include "../include/algebraic_hashing/core/hash_function_base.hpp"
#include "../include/algebraic_hashing/functions/fnv_hash_modern.hpp"
#include "../include/algebraic_hashing/dsl/algebraic_operations.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <format>
#include <chrono>
#include <random>

using namespace algebraic_hashing;
using namespace algebraic_hashing::functions;
using namespace algebraic_hashing::dsl;

/**
 * @brief Chapter 1: Introduction to Hash Values
 * 
 * Hash values form the mathematical foundation of our algebra.
 * They support operations that create algebraic structures.
 */
void chapter1_hash_values() {
    std::cout << "\n=== Chapter 1: Hash Values and Their Algebra ===\n";
    
    // Create some hash values
    auto zero = hash64::zero();
    auto ones = hash64::ones();
    
    // Create a custom hash value
    hash64 custom;
    custom[0] = 0xAB;
    custom[1] = 0xCD;
    custom[7] = 0xFF;
    
    std::cout << "Zero hash: " << zero.to_hex() << "\n";
    std::cout << "Ones hash: " << ones.to_hex() << "\n";  
    std::cout << "Custom:    " << custom.to_hex() << "\n";
    
    // Demonstrate algebraic properties
    std::cout << "\nAlgebraic Properties:\n";
    
    // Group properties under XOR
    auto identity_test = custom ^ zero;
    std::cout << "Identity (h ⊕ 0): " << identity_test.to_hex() 
              << " (should equal custom)\n";
    
    auto inverse_test = custom ^ custom;
    std::cout << "Inverse (h ⊕ h):  " << inverse_test.to_hex() 
              << " (should be zero)\n";
    
    auto commutativity = custom ^ ones;
    auto commutativity2 = ones ^ custom;
    std::cout << "Commutative: " << (commutativity == commutativity2 ? "✓" : "✗") << "\n";
    
    // Ring-like properties
    auto complement = ~custom;
    auto complement_xor = custom ^ ones;
    std::cout << "Complement equivalence: " << (complement == complement_xor ? "✓" : "✗") << "\n";
    
    // Utility operations
    std::cout << "\nUtility Operations:\n";
    std::cout << "Custom hash in binary: " << custom.to_binary().substr(0, 16) << "...\n";
    std::cout << "Population count (set bits): " << custom.popcount() << "\n";
    std::cout << "Is zero? " << (custom.is_zero() ? "Yes" : "No") << "\n";
}

/**
 * @brief Chapter 2: Basic Hash Functions
 * 
 * Demonstrates how to use hash functions and understand their properties.
 */
void chapter2_basic_hash_functions() {
    std::cout << "\n=== Chapter 2: Hash Functions in Action ===\n";
    
    // Create hash function instances
    fnv64 hasher;
    fnv32 hasher32;
    
    // Test various input types
    std::vector<std::string> test_strings = {
        "",
        "a", 
        "hello",
        "hello world",
        "The algebraic nature of hash functions allows elegant composition"
    };
    
    std::cout << "FNV-64 Hash Results:\n";
    for (const auto& str : test_strings) {
        auto hash = hasher(str);
        std::cout << std::format("'{:30}' -> {}\n", str, hash.to_hex());
    }
    
    // Demonstrate determinism
    std::cout << "\nDeterminism Check:\n";
    auto hash1 = hasher("test");
    auto hash2 = hasher("test");
    std::cout << "Same input, same output: " << (hash1 == hash2 ? "✓" : "✗") << "\n";
    
    // Show avalanche effect
    std::cout << "\nAvalanche Effect Demonstration:\n";
    auto base_hash = hasher("hello");
    auto modified_hash = hasher("hallo"); // One character changed
    
    auto diff = base_hash ^ modified_hash;
    auto changed_bits = diff.popcount();
    
    std::cout << std::format("'hello' -> {}\n", base_hash.to_hex());
    std::cout << std::format("'hallo' -> {}\n", modified_hash.to_hex());
    std::cout << std::format("Changed bits: {}/64 ({:.1f}%)\n", 
                            changed_bits, 100.0 * changed_bits / 64.0);
    
    // Statistics and analysis
    std::cout << "\nHash Function Analysis:\n";
    std::cout << explain_hash_function(hasher);
}

/**
 * @brief Chapter 3: Algebraic Composition
 * 
 * The heart of algebraic hashing - composing hash functions to create new ones.
 */
void chapter3_algebraic_composition() {
    std::cout << "\n=== Chapter 3: Algebraic Composition ===\n";
    
    // Create multiple hash function instances
    fnv64 alice, bob, charlie;
    std::string test_input = "algebraic composition";
    
    std::cout << "Base hash functions:\n";
    auto alice_result = alice(test_input);
    auto bob_result = bob(test_input);  
    auto charlie_result = charlie(test_input);
    
    std::cout << std::format("Alice:   {}\n", alice_result.to_hex());
    std::cout << std::format("Bob:     {}\n", bob_result.to_hex());
    std::cout << std::format("Charlie: {}\n", charlie_result.to_hex());
    
    // XOR Composition: h₁ ⊕ h₂
    std::cout << "\n1. XOR Composition (h₁ ⊕ h₂):\n";
    auto xor_composed = alice ^ bob;
    auto xor_result = xor_composed(test_input);
    auto manual_xor = alice_result ^ bob_result;
    
    std::cout << std::format("Alice ⊕ Bob:    {}\n", xor_result.to_hex());
    std::cout << std::format("Manual XOR:     {}\n", manual_xor.to_hex());
    std::cout << "Results match: " << (xor_result == manual_xor ? "✓" : "✗") << "\n";
    
    // Sequential Composition: h₁ ∘ h₂  
    std::cout << "\n2. Sequential Composition (h₁ ∘ h₂):\n";
    auto seq_composed = alice * bob;
    auto seq_result = seq_composed(test_input);
    auto manual_seq = alice(bob_result);
    
    std::cout << std::format("Alice ∘ Bob:    {}\n", seq_result.to_hex());
    std::cout << std::format("Manual seq:     {}\n", manual_seq.to_hex());
    std::cout << "Results match: " << (seq_result == manual_seq ? "✓" : "✗") << "\n";
    
    // Complement: ~h
    std::cout << "\n3. Complement (~h):\n";
    auto complement_fn = ~alice;
    auto comp_result = complement_fn(test_input);
    auto manual_comp = ~alice_result;
    
    std::cout << std::format("~Alice:         {}\n", comp_result.to_hex());
    std::cout << std::format("Manual ~:       {}\n", manual_comp.to_hex());
    std::cout << "Results match: " << (comp_result == manual_comp ? "✓" : "✗") << "\n";
    
    // Chain Composition: h₁ ⊕ h₂ ⊕ h₃
    std::cout << "\n4. Chain Composition (h₁ ⊕ h₂ ⊕ h₃):\n";
    auto chained = chain(alice, bob, charlie);
    auto chain_result = chained(test_input);
    auto manual_chain = alice_result ^ bob_result ^ charlie_result;
    
    std::cout << std::format("chain(A,B,C):   {}\n", chain_result.to_hex());
    std::cout << std::format("Manual chain:   {}\n", manual_chain.to_hex());
    std::cout << "Results match: " << (chain_result == manual_chain ? "✓" : "✗") << "\n";
    
    // Repeated Application: h³
    std::cout << "\n5. Repeated Application (h³):\n";
    auto repeated = repeat<3>(alice);
    auto repeat_result = repeated(test_input);
    
    // Manual computation: h(h(h(x)))
    auto step1 = alice(test_input);
    auto step2 = alice(step1);
    auto manual_repeat = alice(step2);
    
    std::cout << std::format("Alice³:         {}\n", repeat_result.to_hex());
    std::cout << std::format("Manual repeat:  {}\n", manual_repeat.to_hex());
    std::cout << "Results match: " << (repeat_result == manual_repeat ? "✓" : "✗") << "\n";
    
    // Complex Expression: (Alice ⊕ Bob) ∘ (~Charlie)
    std::cout << "\n6. Complex Expression ((A ⊕ B) ∘ (~C)):\n";
    auto complex = (alice ^ bob) * (~charlie);
    auto complex_result = complex(test_input);
    
    std::cout << std::format("Complex result: {}\n", complex_result.to_hex());
    std::cout << "This demonstrates the power of algebraic composition!\n";
}

/**
 * @brief Chapter 4: Mathematical Properties
 * 
 * Explores the mathematical properties that make hash functions algebraic.
 */
void chapter4_mathematical_properties() {
    std::cout << "\n=== Chapter 4: Mathematical Properties ===\n";
    
    fnv64 f1, f2, f3;
    std::string test = "mathematical properties";
    
    std::cout << "Testing Algebraic Laws:\n";
    
    // 1. Commutativity of XOR composition
    auto comm1 = (f1 ^ f2)(test);
    auto comm2 = (f2 ^ f1)(test);
    std::cout << "1. Commutativity (f₁ ⊕ f₂ = f₂ ⊕ f₁): " 
              << (comm1 == comm2 ? "✓" : "✗") << "\n";
    
    // 2. Associativity of XOR composition
    auto assoc1 = ((f1 ^ f2) ^ f3)(test);
    auto assoc2 = (f1 ^ (f2 ^ f3))(test);
    std::cout << "2. Associativity ((f₁⊕f₂)⊕f₃ = f₁⊕(f₂⊕f₃)): " 
              << (assoc1 == assoc2 ? "✓" : "✗") << "\n";
    
    // 3. Involution of complement
    auto invol1 = f1(test);
    auto invol2 = (~(~f1))(test);
    std::cout << "3. Involution (~~f = f): " 
              << (invol1 == invol2 ? "✓" : "✗") << "\n";
    
    // 4. Distribution analysis
    std::cout << "\nDistribution Analysis:\n";
    std::vector<std::string> inputs;
    for (int i = 0; i < 1000; ++i) {
        inputs.push_back("test_" + std::to_string(i));
    }
    
    // Analyze distribution of first byte
    std::array<int, 256> counts{};
    for (const auto& input : inputs) {
        auto hash = f1(input);
        counts[hash[0]]++;
    }
    
    // Calculate some basic statistics
    double mean = 1000.0 / 256.0;
    double variance = 0.0;
    for (int count : counts) {
        double diff = count - mean;
        variance += diff * diff;
    }
    variance /= 256.0;
    
    std::cout << std::format("Expected count per byte: {:.2f}\n", mean);
    std::cout << std::format("Actual variance: {:.2f}\n", variance);
    std::cout << std::format("Std deviation: {:.2f}\n", std::sqrt(variance));
    
    // Show some sample counts
    std::cout << "Sample byte distributions:\n";
    for (int i = 0; i < 8; ++i) {
        std::cout << std::format("  Byte 0x{:02X}: {} occurrences\n", i * 32, counts[i * 32]);
    }
}

/**
 * @brief Chapter 5: Performance and Entropy Analysis  
 * 
 * Demonstrates how to analyze hash function performance and entropy.
 */
void chapter5_performance_analysis() {
    std::cout << "\n=== Chapter 5: Performance and Entropy Analysis ===\n";
    
    fnv64 hasher;
    
    // Performance benchmarking
    std::cout << "Performance Benchmarking:\n";
    
    // Create test data
    std::vector<std::string> test_data;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> len_dist(10, 100);
    std::uniform_int_distribution<> char_dist('a', 'z');
    
    for (int i = 0; i < 10000; ++i) {
        std::string str;
        int len = len_dist(gen);
        for (int j = 0; j < len; ++j) {
            str += static_cast<char>(char_dist(gen));
        }
        test_data.push_back(std::move(str));
    }
    
    // Benchmark hashing
    hasher.reset_statistics();
    auto start = std::chrono::high_resolution_clock::now();
    
    for (const auto& data : test_data) {
        hasher(data);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    auto stats = hasher.get_statistics();
    
    std::cout << std::format("Processed {} strings in {} ms\n", 
                            test_data.size(), duration.count());
    std::cout << std::format("Average time per hash: {:.2f} ns\n", 
                            stats.average_time_ns());
    std::cout << std::format("Throughput: {:.2f} MB/s\n", 
                            stats.throughput_mbps());
    
    // Entropy analysis
    std::cout << "\nEntropy Analysis:\n";
    std::cout << std::format("Theoretical entropy: {:.1f} bits\n", hasher.entropy());
    std::cout << std::format("Hash function: {}\n", hasher.name());
    
    // Collision probability estimation
    double collision_prob = collision_probability(hasher, test_data.size());
    std::cout << std::format("Estimated collision probability for {} inputs: {:.2e}\n",
                            test_data.size(), collision_prob);
    
    // Composition entropy analysis
    std::cout << "\nComposition Entropy Analysis:\n";
    fnv64 f2;
    auto composed = hasher ^ f2;
    
    // Note: Would need entropy() method on composed functions for full analysis
    std::cout << "XOR composition typically maintains entropy of stronger component\n";
    std::cout << "Sequential composition typically reduces entropy due to information bottleneck\n";
}

/**
 * @brief Chapter 6: Practical Applications
 * 
 * Shows real-world applications of algebraic hash composition.
 */
void chapter6_practical_applications() {
    std::cout << "\n=== Chapter 6: Practical Applications ===\n";
    
    std::cout << "1. Hash Function Diversification:\n";
    std::cout << "   Creating multiple independent hash functions from one base function\n";
    
    fnv64 base_hasher;
    
    // Create diversified hash functions using different salts
    auto salt1 = hash64::zero();
    salt1[0] = 0x01;
    auto salt2 = hash64::zero();
    salt2[0] = 0x02;
    auto salt3 = hash64::zero(); 
    salt3[0] = 0x03;
    
    auto salted1 = base_hasher.with_salt(salt1);
    auto salted2 = base_hasher.with_salt(salt2);
    auto salted3 = base_hasher.with_salt(salt3);
    
    std::string test_input = "diversification_test";
    std::cout << std::format("   Base:    {}\n", base_hasher(test_input).to_hex());
    std::cout << std::format("   Salt 1:  {}\n", salted1(test_input).to_hex());
    std::cout << std::format("   Salt 2:  {}\n", salted2(test_input).to_hex());
    std::cout << std::format("   Salt 3:  {}\n", salted3(test_input).to_hex());
    
    std::cout << "\n2. Bloom Filter Hash Functions:\n";
    std::cout << "   Using algebraic composition to create independent hash functions\n";
    
    // Create multiple hash functions for Bloom filter
    auto bloom_hash1 = base_hasher;
    auto bloom_hash2 = base_hasher ^ (~base_hasher);
    auto bloom_hash3 = repeat<2>(base_hasher);
    
    std::string bloom_test = "bloom_filter_element";
    auto h1 = bloom_hash1(bloom_test);
    auto h2 = bloom_hash2(bloom_test);
    auto h3 = bloom_hash3(bloom_test);
    
    // Convert to array indices (mod table size)
    const size_t table_size = 1000;
    auto index1 = std::hash<hash64>{}(h1) % table_size;
    auto index2 = std::hash<hash64>{}(h2) % table_size;
    auto index3 = std::hash<hash64>{}(h3) % table_size;
    
    std::cout << std::format("   Hash 1 -> Index {}\n", index1);
    std::cout << std::format("   Hash 2 -> Index {}\n", index2);
    std::cout << std::format("   Hash 3 -> Index {}\n", index3);
    
    std::cout << "\n3. Cryptographic Hash Chaining:\n";
    std::cout << "   Building more complex hash functions from simpler ones\n";
    
    // Simulate building a more complex hash from simple components
    auto complex_hash = chain(base_hasher, ~base_hasher, repeat<2>(base_hasher));
    auto result = complex_hash("cryptographic_chaining");
    
    std::cout << std::format("   Complex hash result: {}\n", result.to_hex());
    std::cout << "   (In practice, would use proper cryptographic primitives)\n";
    
    std::cout << "\n4. Hash Function Testing and Analysis:\n";
    std::cout << "   Using composition to test hash function properties\n";
    
    // Test idempotency (h^2 should be different from h for good hash functions)
    auto doubled = repeat<2>(base_hasher);
    auto original = base_hasher(test_input);
    auto doubled_result = doubled(test_input);
    
    std::cout << std::format("   h(x):     {}\n", original.to_hex());
    std::cout << std::format("   h(h(x)):  {}\n", doubled_result.to_hex());
    std::cout << "   Different results indicate good avalanche properties: " 
              << (original != doubled_result ? "✓" : "✗") << "\n";
}

/**
 * @brief Main tutorial function
 */
int main() {
    std::cout << "Algebraic Hashing Library Tutorial\n";
    std::cout << "===================================\n";
    std::cout << "This tutorial demonstrates the mathematical foundations\n";
    std::cout << "and practical applications of algebraic hash functions.\n";
    
    try {
        chapter1_hash_values();
        chapter2_basic_hash_functions();
        chapter3_algebraic_composition();
        chapter4_mathematical_properties();
        chapter5_performance_analysis();
        chapter6_practical_applications();
        
        std::cout << "\n=== Tutorial Complete ===\n";
        std::cout << "You've explored:\n";
        std::cout << "• Hash value algebra and mathematical properties\n";
        std::cout << "• Basic hash function usage and analysis\n";
        std::cout << "• Algebraic composition techniques\n";
        std::cout << "• Mathematical laws and properties\n";
        std::cout << "• Performance and entropy analysis\n";
        std::cout << "• Practical applications\n";
        std::cout << "\nThe algebraic hashing library provides a powerful\n";
        std::cout << "mathematical framework for understanding and composing\n";
        std::cout << "hash functions. Happy hashing!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Tutorial error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}