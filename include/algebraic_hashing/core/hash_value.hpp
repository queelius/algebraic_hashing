#pragma once

#include "concepts.hpp"
#include <array>
#include <string>
#include <cstdint>
#include <cstring>
#include <bit>
#include <span>
#include <compare>
#include <ranges>
#include <algorithm>

namespace algebraic_hashing {

/**
 * @brief Modern, pedagogical hash value type
 * 
 * This class represents the output of hash functions and forms the foundation
 * of our algebraic operations. It demonstrates key mathematical properties:
 * 
 * 1. **Abelian Group under XOR**: (hash_value, ^, zero()) forms an abelian group
 *    - Associative: (a ^ b) ^ c = a ^ (b ^ c)
 *    - Commutative: a ^ b = b ^ a  
 *    - Identity: a ^ zero() = a
 *    - Inverse: a ^ a = zero()
 * 
 * 2. **Ring-like Structure**: When combined with other operations, forms
 *    algebraic structures useful for hash composition
 * 
 * @tparam N Number of bytes in the hash value
 */
template<std::size_t N>
class hash_value {
public:
    using value_type = std::uint8_t;
    using container_type = std::array<value_type, N>;
    using size_type = std::size_t;
    
    static constexpr size_type size_bytes = N;
    static constexpr size_type size_bits = N * 8;

private:
    container_type data_{};

public:
    // ==================== Construction ====================
    
    /**
     * @brief Default constructor - creates the zero element
     * 
     * The zero element is the additive identity in our algebra:
     * zero() ^ h = h for any hash_value h
     */
    constexpr hash_value() = default;
    
    /**
     * @brief Construct from byte array
     */
    constexpr explicit hash_value(container_type const& data) : data_(data) {}
    
    /**
     * @brief Construct from byte span
     */
    explicit hash_value(std::span<value_type const, N> bytes) {
        std::ranges::copy(bytes, data_.begin());
    }
    
    /**
     * @brief Construct from raw bytes
     */
    explicit hash_value(value_type const* bytes) {
        std::copy_n(bytes, N, data_.begin());
    }
    
    // ==================== Algebraic Operations ====================
    
    /**
     * @brief XOR assignment - fundamental algebraic operation
     * 
     * This implements the group operation in our hash value algebra.
     * Properties: associative, commutative, self-inverse.
     */
    constexpr hash_value& operator^=(hash_value const& rhs) noexcept {
        for (size_type i = 0; i < N; ++i) {
            data_[i] ^= rhs.data_[i];
        }
        return *this;
    }
    
    /**
     * @brief XOR operation
     */
    constexpr friend hash_value operator^(hash_value lhs, hash_value const& rhs) noexcept {
        return lhs ^= rhs;
    }
    
    /**
     * @brief Bitwise complement
     * 
     * Important for certain algebraic properties and hash transformations.
     */
    constexpr hash_value operator~() const noexcept {
        hash_value result;
        for (size_type i = 0; i < N; ++i) {
            result.data_[i] = ~data_[i];
        }
        return result;
    }
    
    /**
     * @brief Bitwise AND - useful for certain hash operations
     */
    constexpr hash_value& operator&=(hash_value const& rhs) noexcept {
        for (size_type i = 0; i < N; ++i) {
            data_[i] &= rhs.data_[i];
        }
        return *this;
    }
    
    constexpr friend hash_value operator&(hash_value lhs, hash_value const& rhs) noexcept {
        return lhs &= rhs;
    }
    
    /**
     * @brief Bitwise OR
     */
    constexpr hash_value& operator|=(hash_value const& rhs) noexcept {
        for (size_type i = 0; i < N; ++i) {
            data_[i] |= rhs.data_[i];
        }
        return *this;
    }
    
    constexpr friend hash_value operator|(hash_value lhs, hash_value const& rhs) noexcept {
        return lhs |= rhs;
    }
    
    // ==================== Special Elements ====================
    
    /**
     * @brief Returns the zero element (additive identity)
     */
    static constexpr hash_value zero() noexcept {
        return hash_value{};
    }
    
    /**
     * @brief Returns the one element (all bits set)
     * 
     * Useful as a multiplicative-like identity in certain contexts.
     */
    static constexpr hash_value ones() noexcept {
        hash_value result;
        result.data_.fill(0xFF);
        return result;
    }
    
    /**
     * @brief Check if this is the zero element
     */
    constexpr bool is_zero() const noexcept {
        return std::ranges::all_of(data_, [](value_type b) { return b == 0; });
    }
    
    // ==================== Access Operations ====================
    
    /**
     * @brief Access byte at index
     */
    constexpr value_type operator[](size_type index) const noexcept {
        return data_[index];
    }
    
    constexpr value_type& operator[](size_type index) noexcept {
        return data_[index];
    }
    
    /**
     * @brief Get underlying data
     */
    constexpr container_type const& data() const noexcept { return data_; }
    constexpr container_type& data() noexcept { return data_; }
    
    /**
     * @brief Get as byte span
     */
    constexpr std::span<value_type const> bytes() const noexcept {
        return std::span{data_};
    }
    
    constexpr std::span<value_type> bytes() noexcept {
        return std::span{data_};
    }
    
    // ==================== Iterator Interface ====================
    
    constexpr auto begin() const noexcept { return data_.begin(); }
    constexpr auto end() const noexcept { return data_.end(); }
    constexpr auto begin() noexcept { return data_.begin(); }
    constexpr auto end() noexcept { return data_.end(); }
    
    // ==================== Utility Operations ====================
    
    /**
     * @brief Convert to hexadecimal string representation
     * 
     * Provides human-readable output for debugging and display.
     */
    std::string to_hex() const {
        std::string result;
        result.reserve(N * 2);
        
        for (value_type byte : data_) {
            // Use manual hex conversion since std::format might not be available
            static const char hex_chars[] = "0123456789abcdef";
            result += hex_chars[byte >> 4];
            result += hex_chars[byte & 0x0F];
        }
        return result;
    }
    
    /**
     * @brief Convert to binary string representation
     */
    std::string to_binary() const {
        std::string result;
        result.reserve(N * 8);
        
        for (value_type byte : data_) {
            for (int i = 7; i >= 0; --i) {
                result += ((byte >> i) & 1) ? '1' : '0';
            }
        }
        return result;
    }
    
    /**
     * @brief Hamming weight (number of set bits)
     * 
     * Useful for analyzing hash distribution properties.
     */
    constexpr std::size_t popcount() const noexcept {
        std::size_t count = 0;
        for (value_type byte : data_) {
            count += std::popcount(byte);
        }
        return count;
    }
    
    // ==================== Comparison Operations ====================
    
    constexpr auto operator<=>(hash_value const&) const noexcept = default;
    constexpr bool operator==(hash_value const&) const noexcept = default;
};

// ==================== Free Functions ====================

/**
 * @brief Concatenate two hash values
 * 
 * Creates a longer hash value by concatenating two shorter ones.
 * Useful for building composite hash functions.
 */
template<std::size_t N1, std::size_t N2>
constexpr hash_value<N1 + N2> concatenate(
    hash_value<N1> const& left, 
    hash_value<N2> const& right) noexcept {
    
    hash_value<N1 + N2> result;
    auto result_bytes = result.bytes();
    
    std::ranges::copy(left.bytes(), result_bytes.begin());
    std::ranges::copy(right.bytes(), result_bytes.begin() + N1);
    
    return result;
}

/**
 * @brief Truncate hash value to smaller size
 * 
 * Takes the first N bytes of a larger hash value.
 * Useful for adapting hash functions to different output sizes.
 */
template<std::size_t NewSize, std::size_t OriginalSize>
constexpr hash_value<NewSize> truncate(hash_value<OriginalSize> const& original) noexcept 
    requires (NewSize <= OriginalSize) {
    
    hash_value<NewSize> result;
    std::ranges::copy_n(original.bytes().begin(), NewSize, result.bytes().begin());
    return result;
}

// ==================== Common Type Aliases ====================

using hash32 = hash_value<4>;   ///< 32-bit hash value
using hash64 = hash_value<8>;   ///< 64-bit hash value  
using hash128 = hash_value<16>; ///< 128-bit hash value (MD5-sized)
using hash256 = hash_value<32>; ///< 256-bit hash value (SHA256-sized)
using hash512 = hash_value<64>; ///< 512-bit hash value (SHA512-sized)

} // namespace algebraic_hashing

// ==================== Standard Library Integration ====================

namespace std {

/**
 * @brief Standard library hash specialization
 * 
 * Allows hash_value to be used in standard containers that require hashing.
 */
template<std::size_t N>
struct hash<algebraic_hashing::hash_value<N>> {
    std::size_t operator()(algebraic_hashing::hash_value<N> const& h) const noexcept {
        // Use first sizeof(size_t) bytes as hash, or fold if too small
        if constexpr (N >= sizeof(std::size_t)) {
            std::size_t result;
            std::memcpy(&result, h.data().data(), sizeof(std::size_t));
            return result;
        } else {
            std::size_t result = 0;
            for (std::size_t i = 0; i < N; ++i) {
                result ^= static_cast<std::size_t>(h[i]) << (i * 8);
            }
            return result;
        }
    }
};

/**
 * @brief String conversion support
 */
template<std::size_t N>
string to_string(algebraic_hashing::hash_value<N> const& h) {
    return h.to_hex();
}

} // namespace std