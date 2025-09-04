#pragma once

#include "../core/hash_function_base.hpp"
#include "../core/concepts.hpp"
#include "../core/hash_value.hpp"
#include <string>
#include <string_view>
#include <span>
#include <vector>
#include <tuple>
#include <variant>
#include <optional>
#include <ranges>
#include <bit>
#include <concepts>
#include <iostream>
#include <iomanip>
#include <unordered_map>

namespace algebraic_hashing::functions {

/**
 * @brief Modern FNV-1a Hash Function Implementation
 * 
 * This is a pedagogical implementation of the Fowler-Noll-Vo hash function
 * that demonstrates:
 * 
 * 1. **Non-cryptographic hash design**: Fast computation, good distribution
 * 2. **Avalanche properties**: Small changes create large hash differences
 * 3. **Modern C++ techniques**: Concepts, ranges, constexpr evaluation
 * 4. **Algebraic composition**: Can be combined with other hash functions
 * 
 * The FNV-1a algorithm:
 * ```
 * hash = FNV_offset_basis
 * for each byte in input:
 *     hash = hash ⊕ byte
 *     hash = hash × FNV_prime
 * ```
 * 
 * This implementation supports both compile-time and runtime evaluation,
 * multiple input types, and provides educational insights into hash function
 * behavior.
 */
template<typename HashType = hash64>
class fnv_hash : public hash_function_base<fnv_hash<HashType>, HashType> {
public:
    using hash_type = HashType;
    using base_type = hash_function_base<fnv_hash<HashType>, HashType>;
    
    static_assert(concepts::HashValue<HashType>, "HashType must be a valid hash value type");
    
    // FNV constants for different hash sizes
    struct constants {
        // 64-bit FNV constants (most common)
        static constexpr std::uint64_t prime_64 = 1099511628211ULL;
        static constexpr std::uint64_t offset_basis_64 = 14695981039346656037ULL;
        
        // 32-bit FNV constants  
        static constexpr std::uint32_t prime_32 = 16777619U;
        static constexpr std::uint32_t offset_basis_32 = 2166136261U;
    };

private:
    // Type tags for specialization - pedagogical technique to show
    // how we can handle different hash sizes generically
    template<typename T> struct hash_size_tag {};
    using tag_32 = hash_size_tag<hash32>;
    using tag_64 = hash_size_tag<hash64>;
    using tag_other = hash_size_tag<void>;
    
    // Determine which constants to use based on hash type
    static constexpr auto determine_tag() {
        if constexpr (std::same_as<HashType, hash32>) {
            return tag_32{};
        } else if constexpr (std::same_as<HashType, hash64>) {
            return tag_64{};
        } else {
            return tag_other{};
        }
    }
    
    using current_tag = decltype(determine_tag());

public:
    // ==================== Core Implementation ====================
    
    /**
     * @brief Main hash implementation using CRTP
     */
    template<concepts::Hashable T>
    constexpr hash_type hash_impl(T const& input) const {
        return hash_dispatch(input, current_tag{});
    }
    
    /**
     * @brief Get function name for pedagogical purposes
     */
    std::string name_impl() const {
        return std::string("FNV-1a-") + std::to_string(sizeof(HashType) * 8);
    }
    
    /**
     * @brief Entropy estimate
     * 
     * FNV has good but not perfect distribution, so we estimate
     * slightly less than theoretical maximum.
     */
    double entropy_impl() const {
        return (sizeof(HashType) * 8.0) * 0.95; // Conservative estimate
    }

private:
    // ==================== Specialized Hash Implementations ====================
    
    // 64-bit FNV-1a implementation
    template<concepts::Hashable T>
    constexpr hash64 hash_dispatch(T const& input, tag_64) const {
        std::uint64_t hash = constants::offset_basis_64;
        auto bytes = get_byte_representation(input);
        
        for (std::uint8_t byte : bytes) {
            hash ^= static_cast<std::uint64_t>(byte);
            hash *= constants::prime_64;
        }
        
        // Convert to our hash_value type
        hash64 result;
        std::memcpy(result.data().data(), &hash, sizeof(hash));
        return result;
    }
    
    // 32-bit FNV-1a implementation  
    template<concepts::Hashable T>
    constexpr hash32 hash_dispatch(T const& input, tag_32) const {
        std::uint32_t hash = constants::offset_basis_32;
        auto bytes = get_byte_representation(input);
        
        for (std::uint8_t byte : bytes) {
            hash ^= static_cast<std::uint32_t>(byte);
            hash *= constants::prime_32;
        }
        
        // Convert to our hash_value type
        hash32 result;
        std::memcpy(result.data().data(), &hash, sizeof(hash));
        return result;
    }
    
    // Generic implementation for other hash sizes
    template<concepts::Hashable T>
    constexpr HashType hash_dispatch(T const& input, tag_other) const {
        // Use 64-bit FNV and truncate/extend as needed
        auto hash64_result = hash_dispatch(input, tag_64{});
        
        if constexpr (sizeof(HashType) <= sizeof(hash64)) {
            return truncate<sizeof(HashType)>(hash64_result);
        } else {
            // For larger hashes, we can repeat the hash with different salts
            return extend_hash(hash64_result);
        }
    }
    
    // ==================== Input Processing ====================
    
    /**
     * @brief Convert various input types to byte sequences
     * 
     * This demonstrates how to handle different data types generically
     * while maintaining type safety and performance.
     */
    template<typename T>
    constexpr auto get_byte_representation(T const& input) const {
        if constexpr (std::is_arithmetic_v<T>) {
            // Handle fundamental types by direct byte access
            return get_arithmetic_bytes(input);
        } else if constexpr (std::is_array_v<T> && std::same_as<std::remove_extent_t<T>, char>) {
            // Handle const char arrays (string literals)
            auto len = std::extent_v<T> > 0 ? std::extent_v<T> - 1 : 0; // Exclude null terminator
            return std::span<const std::uint8_t>(
                reinterpret_cast<const std::uint8_t*>(input), 
                len
            );
        } else if constexpr (std::same_as<T, std::string> || std::same_as<T, std::string_view>) {
            // String types
            return std::span<const std::uint8_t>(
                reinterpret_cast<const std::uint8_t*>(input.data()), 
                input.size()
            );
        } else if constexpr (std::ranges::contiguous_range<T>) {
            // Handle containers like vector, array
            return get_container_bytes(input);
        } else if constexpr (requires { std::tuple_size_v<T>; }) {
            // Handle tuples
            return get_tuple_bytes(input);
        } else {
            // Fallback: treat as raw memory
            return std::span<const std::uint8_t>(
                reinterpret_cast<const std::uint8_t*>(&input), 
                sizeof(T)
            );
        }
    }
    
    template<std::integral T>
    constexpr auto get_arithmetic_bytes(T value) const {
        // Convert to little-endian byte array for platform independence
        std::array<std::uint8_t, sizeof(T)> bytes;
        for (std::size_t i = 0; i < sizeof(T); ++i) {
            bytes[i] = static_cast<std::uint8_t>(value & 0xFF);
            value >>= 8;
        }
        return std::span<const std::uint8_t>(bytes.data(), bytes.size());
    }
    
    template<std::floating_point T>
    constexpr auto get_arithmetic_bytes(T value) const {
        // For floating point, we use bit_cast for deterministic representation
        if constexpr (sizeof(T) == sizeof(std::uint32_t)) {
            auto int_value = std::bit_cast<std::uint32_t>(value);
            return get_arithmetic_bytes(int_value);
        } else if constexpr (sizeof(T) == sizeof(std::uint64_t)) {
            auto int_value = std::bit_cast<std::uint64_t>(value);
            return get_arithmetic_bytes(int_value);
        } else {
            // Fallback for unusual float types
            return std::span<const std::uint8_t>(
                reinterpret_cast<const std::uint8_t*>(&value), 
                sizeof(T)
            );
        }
    }
    
    template<std::ranges::contiguous_range R>
    constexpr auto get_container_bytes(R const& container) const {
        using value_type = std::ranges::range_value_t<R>;
        
        if constexpr (sizeof(value_type) == 1) {
            // Already bytes
            return std::span<const std::uint8_t>(
                reinterpret_cast<const std::uint8_t*>(std::ranges::data(container)),
                std::ranges::size(container)
            );
        } else {
            // Need to flatten multi-byte elements
            // For simplicity, treat as raw memory - could be more sophisticated
            return std::span<const std::uint8_t>(
                reinterpret_cast<const std::uint8_t*>(std::ranges::data(container)),
                std::ranges::size(container) * sizeof(value_type)
            );
        }
    }
    
    template<typename Tuple>
    constexpr auto get_tuple_bytes(Tuple const& tuple) const {
        // For tuples, we hash each element and combine
        // This is a simplified approach - real implementation might be more sophisticated
        return std::span<const std::uint8_t>(
            reinterpret_cast<const std::uint8_t*>(&tuple),
            sizeof(Tuple)
        );
    }
    
    // ==================== Hash Extension for Large Outputs ====================
    
    HashType extend_hash(hash64 const& base_hash) const {
        static_assert(sizeof(HashType) > sizeof(hash64));
        
        HashType result;
        auto result_bytes = result.bytes();
        
        // Copy base hash
        std::ranges::copy(base_hash.bytes(), result_bytes.begin());
        
        // Fill remaining bytes by rehashing with different salts
        std::size_t remaining_bytes = sizeof(HashType) - sizeof(hash64);
        std::size_t offset = sizeof(hash64);
        
        while (remaining_bytes > 0) {
            // Create a salted version and hash the base hash
            auto salted = base_hash ^ hash64{}; // Simplified salting
            auto rehashed = hash_dispatch(salted, tag_64{});
            
            std::size_t copy_bytes = std::min(remaining_bytes, sizeof(hash64));
            std::ranges::copy_n(rehashed.bytes().begin(), copy_bytes, 
                               result_bytes.begin() + offset);
            
            remaining_bytes -= copy_bytes;
            offset += copy_bytes;
        }
        
        return result;
    }
};

// ==================== Convenience Aliases ====================

/**
 * @brief Standard 64-bit FNV hash
 */
using fnv64 = fnv_hash<hash64>;

/**
 * @brief 32-bit FNV hash for memory-constrained applications
 */
using fnv32 = fnv_hash<hash32>;

/**
 * @brief 128-bit FNV hash for higher collision resistance
 */
using fnv128 = fnv_hash<hash128>;

// ==================== Compile-time Evaluation Examples ====================

/**
 * @brief Demonstrate compile-time hash computation
 * 
 * This shows how modern C++ allows hash computation at compile time,
 * which can be useful for perfect hash tables and other optimizations.
 */
namespace compile_time_examples {
    
    // These would be computed at compile time if operator() was constexpr
    // For now, these are regular runtime computations
    inline auto hello_hash() { return fnv64{}("hello"); }
    inline auto world_hash() { return fnv64{}("world"); }
    inline auto number_hash() { return fnv64{}(42); }
    
    // Demonstrate algebraic composition 
    inline auto composed_hash() { return hello_hash() ^ world_hash(); }
    
} // namespace compile_time_examples

// ==================== Educational Helpers ====================

/**
 * @brief Demonstrate hash function properties with FNV
 */
class fnv_educational_demo {
public:
    /**
     * @brief Show avalanche effect
     */
    static void demonstrate_avalanche_effect() {
        fnv64 hasher;
        
        std::string base = "hello";
        auto base_hash = hasher(base);
        
        // Change one character
        std::string modified = "hallo";
        auto modified_hash = hasher(modified);
        
        auto diff = base_hash ^ modified_hash;
        auto changed_bits = diff.popcount();
        
        std::cout << "Avalanche Effect Demonstration:\n";
        std::cout << "Base string: '" << base << "' -> " << base_hash.to_hex() << "\n";
        std::cout << "Modified:    '" << modified << "' -> " << modified_hash.to_hex() << "\n";
        std::cout << "Changed bits: " << changed_bits << "/" << (sizeof(hash64) * 8) << "\n";
        std::cout << "Percentage changed: " << (100.0 * changed_bits) / (sizeof(hash64) * 8) << "%\n";
    }
    
    /**
     * @brief Show distribution properties
     */
    static void demonstrate_distribution() {
        fnv64 hasher;
        std::unordered_map<std::uint8_t, std::size_t> byte_counts;
        
        // Hash many strings and analyze first byte distribution
        for (int i = 0; i < 10000; ++i) {
            auto hash = hasher(std::to_string(i));
            ++byte_counts[hash[0]];
        }
        
        std::cout << "Distribution Analysis (first byte):\n";
        std::cout << "Expected count per byte value: " << 10000.0 / 256.0 << "\n";
        
        // Show some sample counts
        for (int i = 0; i < 16; ++i) {
            std::cout << "Byte " << std::hex << std::setw(2) << std::setfill('0') 
                      << i << ": " << std::dec << byte_counts[i] << " occurrences\n";
        }
    }
};

} // namespace algebraic_hashing::functions