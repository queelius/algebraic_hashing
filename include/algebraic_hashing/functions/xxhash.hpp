#pragma once

#include "../core/hash_function_base.hpp"
#include "../core/concepts.hpp"
#include "../core/hash_value.hpp"
#include <string>
#include <string_view>
#include <span>
#include <array>
#include <cstdint>
#include <cstring>
#include <bit>

namespace algebraic_hashing::functions {

/**
 * @brief xxHash Implementation
 *
 * xxHash is an extremely fast non-cryptographic hash algorithm created by
 * Yann Collet. It offers:
 *
 * 1. **Exceptional speed**: One of the fastest hash algorithms available
 * 2. **Excellent distribution**: Passes all SMHasher quality tests
 * 3. **Portability**: Same results on all platforms (with proper endianness handling)
 * 4. **Multiple variants**: xxHash32, xxHash64, and xxHash3 (128-bit)
 *
 * This implementation provides:
 * - xxHash32: 32-bit variant
 * - xxHash64: 64-bit variant (recommended for most uses)
 *
 * @see https://github.com/Cyan4973/xxHash
 * @see https://xxhash.com/
 */
template<typename HashType = hash64>
class xxhash : public hash_function_base<xxhash<HashType>, HashType> {
public:
    using hash_type = HashType;
    using base_type = hash_function_base<xxhash<HashType>, HashType>;

    static_assert(concepts::HashValue<HashType>, "HashType must be a valid hash value type");

private:
    std::uint64_t seed_ = 0;

    // Type tags for specialization
    template<typename T> struct hash_size_tag {};
    using tag_32 = hash_size_tag<hash32>;
    using tag_64 = hash_size_tag<hash64>;
    using tag_other = hash_size_tag<void>;

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
    /**
     * @brief Default constructor with seed = 0
     */
    constexpr xxhash() = default;

    /**
     * @brief Construct with specific seed
     * @param seed The seed value for the hash function
     */
    constexpr explicit xxhash(std::uint64_t seed) : seed_(seed) {}

    /**
     * @brief Get current seed
     */
    constexpr std::uint64_t seed() const noexcept { return seed_; }

    /**
     * @brief Set seed
     */
    constexpr void set_seed(std::uint64_t seed) noexcept { seed_ = seed; }

    /**
     * @brief Main hash implementation using CRTP
     */
    template<concepts::Hashable T>
    constexpr hash_type hash_impl(T const& input) const {
        return hash_dispatch(input, current_tag{});
    }

    /**
     * @brief Get function name
     */
    std::string name_impl() const {
        return std::string("xxHash") + std::to_string(sizeof(HashType) * 8);
    }

    /**
     * @brief Entropy estimate - xxHash has excellent distribution
     */
    double entropy_impl() const {
        return sizeof(HashType) * 8.0 * 0.99; // Near-perfect distribution
    }

private:
    // ==================== xxHash32 Constants ====================

    static constexpr std::uint32_t PRIME32_1 = 0x9E3779B1U;
    static constexpr std::uint32_t PRIME32_2 = 0x85EBCA77U;
    static constexpr std::uint32_t PRIME32_3 = 0xC2B2AE3DU;
    static constexpr std::uint32_t PRIME32_4 = 0x27D4EB2FU;
    static constexpr std::uint32_t PRIME32_5 = 0x165667B1U;

    // ==================== xxHash64 Constants ====================

    static constexpr std::uint64_t PRIME64_1 = 0x9E3779B185EBCA87ULL;
    static constexpr std::uint64_t PRIME64_2 = 0xC2B2AE3D27D4EB4FULL;
    static constexpr std::uint64_t PRIME64_3 = 0x165667B19E3779F9ULL;
    static constexpr std::uint64_t PRIME64_4 = 0x85EBCA77C2B2AE63ULL;
    static constexpr std::uint64_t PRIME64_5 = 0x27D4EB2F165667C5ULL;

    // ==================== Helper Functions ====================

    static constexpr std::uint32_t rotl32(std::uint32_t x, int r) noexcept {
        return (x << r) | (x >> (32 - r));
    }

    static constexpr std::uint64_t rotl64(std::uint64_t x, int r) noexcept {
        return (x << r) | (x >> (64 - r));
    }

    static constexpr std::uint32_t read32(const std::uint8_t* ptr) noexcept {
        std::uint32_t val;
        std::memcpy(&val, ptr, sizeof(val));
        return val;
    }

    static constexpr std::uint64_t read64(const std::uint8_t* ptr) noexcept {
        std::uint64_t val;
        std::memcpy(&val, ptr, sizeof(val));
        return val;
    }

    // xxHash32 round function
    static constexpr std::uint32_t xxh32_round(std::uint32_t acc, std::uint32_t input) noexcept {
        acc += input * PRIME32_2;
        acc = rotl32(acc, 13);
        acc *= PRIME32_1;
        return acc;
    }

    // xxHash64 round function
    static constexpr std::uint64_t xxh64_round(std::uint64_t acc, std::uint64_t input) noexcept {
        acc += input * PRIME64_2;
        acc = rotl64(acc, 31);
        acc *= PRIME64_1;
        return acc;
    }

    // xxHash64 merge round
    static constexpr std::uint64_t xxh64_merge_round(std::uint64_t acc, std::uint64_t val) noexcept {
        val = xxh64_round(0, val);
        acc ^= val;
        acc = acc * PRIME64_1 + PRIME64_4;
        return acc;
    }

    // ==================== Byte Representation ====================

    template<typename T>
    constexpr auto get_byte_representation(T const& input) const {
        if constexpr (std::is_arithmetic_v<T>) {
            return get_arithmetic_bytes(input);
        } else if constexpr (std::is_array_v<T> && std::same_as<std::remove_extent_t<T>, char>) {
            auto len = std::extent_v<T> > 0 ? std::extent_v<T> - 1 : 0;
            return std::span<const std::uint8_t>(
                reinterpret_cast<const std::uint8_t*>(input),
                len
            );
        } else if constexpr (std::same_as<T, std::string> || std::same_as<T, std::string_view>) {
            return std::span<const std::uint8_t>(
                reinterpret_cast<const std::uint8_t*>(input.data()),
                input.size()
            );
        } else if constexpr (std::ranges::contiguous_range<T>) {
            using value_type = std::ranges::range_value_t<T>;
            return std::span<const std::uint8_t>(
                reinterpret_cast<const std::uint8_t*>(std::ranges::data(input)),
                std::ranges::size(input) * sizeof(value_type)
            );
        } else {
            return std::span<const std::uint8_t>(
                reinterpret_cast<const std::uint8_t*>(&input),
                sizeof(T)
            );
        }
    }

    template<std::integral T>
    constexpr auto get_arithmetic_bytes(T value) const {
        std::array<std::uint8_t, sizeof(T)> bytes;
        for (std::size_t i = 0; i < sizeof(T); ++i) {
            bytes[i] = static_cast<std::uint8_t>(value & 0xFF);
            value >>= 8;
        }
        return bytes;
    }

    template<std::floating_point T>
    constexpr auto get_arithmetic_bytes(T value) const {
        if constexpr (sizeof(T) == sizeof(std::uint32_t)) {
            return get_arithmetic_bytes(std::bit_cast<std::uint32_t>(value));
        } else if constexpr (sizeof(T) == sizeof(std::uint64_t)) {
            return get_arithmetic_bytes(std::bit_cast<std::uint64_t>(value));
        } else {
            std::array<std::uint8_t, sizeof(T)> bytes;
            std::memcpy(bytes.data(), &value, sizeof(T));
            return bytes;
        }
    }

    // ==================== xxHash32 Implementation ====================

    template<concepts::Hashable T>
    constexpr hash32 hash_dispatch(T const& input, tag_32) const {
        auto bytes = get_byte_representation(input);
        const std::uint8_t* data = bytes.data();
        const std::size_t len = bytes.size();
        const std::uint8_t* const end = data + len;
        std::uint32_t h32;

        if (len >= 16) {
            const std::uint8_t* const limit = end - 16;
            std::uint32_t v1 = static_cast<std::uint32_t>(seed_) + PRIME32_1 + PRIME32_2;
            std::uint32_t v2 = static_cast<std::uint32_t>(seed_) + PRIME32_2;
            std::uint32_t v3 = static_cast<std::uint32_t>(seed_) + 0;
            std::uint32_t v4 = static_cast<std::uint32_t>(seed_) - PRIME32_1;

            do {
                v1 = xxh32_round(v1, read32(data));
                data += 4;
                v2 = xxh32_round(v2, read32(data));
                data += 4;
                v3 = xxh32_round(v3, read32(data));
                data += 4;
                v4 = xxh32_round(v4, read32(data));
                data += 4;
            } while (data <= limit);

            h32 = rotl32(v1, 1) + rotl32(v2, 7) + rotl32(v3, 12) + rotl32(v4, 18);
        } else {
            h32 = static_cast<std::uint32_t>(seed_) + PRIME32_5;
        }

        h32 += static_cast<std::uint32_t>(len);

        // Process remaining bytes
        while (data + 4 <= end) {
            h32 += read32(data) * PRIME32_3;
            h32 = rotl32(h32, 17) * PRIME32_4;
            data += 4;
        }

        while (data < end) {
            h32 += (*data) * PRIME32_5;
            h32 = rotl32(h32, 11) * PRIME32_1;
            data++;
        }

        // Finalization
        h32 ^= h32 >> 15;
        h32 *= PRIME32_2;
        h32 ^= h32 >> 13;
        h32 *= PRIME32_3;
        h32 ^= h32 >> 16;

        hash32 result;
        std::memcpy(result.data().data(), &h32, sizeof(h32));
        return result;
    }

    // ==================== xxHash64 Implementation ====================

    template<concepts::Hashable T>
    constexpr hash64 hash_dispatch(T const& input, tag_64) const {
        auto bytes = get_byte_representation(input);
        const std::uint8_t* data = bytes.data();
        const std::size_t len = bytes.size();
        const std::uint8_t* const end = data + len;
        std::uint64_t h64;

        if (len >= 32) {
            const std::uint8_t* const limit = end - 32;
            std::uint64_t v1 = seed_ + PRIME64_1 + PRIME64_2;
            std::uint64_t v2 = seed_ + PRIME64_2;
            std::uint64_t v3 = seed_ + 0;
            std::uint64_t v4 = seed_ - PRIME64_1;

            do {
                v1 = xxh64_round(v1, read64(data));
                data += 8;
                v2 = xxh64_round(v2, read64(data));
                data += 8;
                v3 = xxh64_round(v3, read64(data));
                data += 8;
                v4 = xxh64_round(v4, read64(data));
                data += 8;
            } while (data <= limit);

            h64 = rotl64(v1, 1) + rotl64(v2, 7) + rotl64(v3, 12) + rotl64(v4, 18);
            h64 = xxh64_merge_round(h64, v1);
            h64 = xxh64_merge_round(h64, v2);
            h64 = xxh64_merge_round(h64, v3);
            h64 = xxh64_merge_round(h64, v4);
        } else {
            h64 = seed_ + PRIME64_5;
        }

        h64 += len;

        // Process remaining 8-byte chunks
        while (data + 8 <= end) {
            std::uint64_t k1 = xxh64_round(0, read64(data));
            h64 ^= k1;
            h64 = rotl64(h64, 27) * PRIME64_1 + PRIME64_4;
            data += 8;
        }

        // Process remaining 4-byte chunk
        if (data + 4 <= end) {
            h64 ^= static_cast<std::uint64_t>(read32(data)) * PRIME64_1;
            h64 = rotl64(h64, 23) * PRIME64_2 + PRIME64_3;
            data += 4;
        }

        // Process remaining bytes
        while (data < end) {
            h64 ^= (*data) * PRIME64_5;
            h64 = rotl64(h64, 11) * PRIME64_1;
            data++;
        }

        // Finalization
        h64 ^= h64 >> 33;
        h64 *= PRIME64_2;
        h64 ^= h64 >> 29;
        h64 *= PRIME64_3;
        h64 ^= h64 >> 32;

        hash64 result;
        std::memcpy(result.data().data(), &h64, sizeof(h64));
        return result;
    }

    // ==================== Other sizes ====================

    template<concepts::Hashable T>
    constexpr HashType hash_dispatch(T const& input, tag_other) const {
        if constexpr (sizeof(HashType) <= 4) {
            auto h32 = hash_dispatch(input, tag_32{});
            return truncate<sizeof(HashType)>(h32);
        } else if constexpr (sizeof(HashType) <= 8) {
            auto h64 = hash_dispatch(input, tag_64{});
            return truncate<sizeof(HashType)>(h64);
        } else {
            // For larger sizes, use multiple xxHash64 with different seeds
            HashType result;
            auto result_bytes = result.bytes();
            std::size_t offset = 0;
            std::uint64_t current_seed = seed_;

            while (offset < sizeof(HashType)) {
                xxhash<hash64> sub_hasher(current_seed);
                auto chunk = sub_hasher(input);
                std::size_t copy_bytes = std::min(sizeof(hash64), sizeof(HashType) - offset);
                std::memcpy(result_bytes.data() + offset, chunk.data().data(), copy_bytes);
                offset += copy_bytes;
                current_seed += 0x9e3779b97f4a7c15ULL; // Golden ratio for 64-bit
            }
            return result;
        }
    }
};

// ==================== Convenience Aliases ====================

/**
 * @brief 32-bit xxHash
 */
using xxhash32 = xxhash<hash32>;

/**
 * @brief 64-bit xxHash (recommended)
 */
using xxhash64 = xxhash<hash64>;

/**
 * @brief 128-bit xxHash (extended)
 */
using xxhash128 = xxhash<hash128>;

} // namespace algebraic_hashing::functions
