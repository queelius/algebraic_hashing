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
 * @brief MurmurHash3 Implementation
 *
 * MurmurHash3 is a non-cryptographic hash function created by Austin Appleby.
 * It provides excellent distribution and performance characteristics:
 *
 * 1. **Fast computation**: Optimized for modern CPUs with multiplication
 * 2. **Excellent avalanche**: Small input changes cascade to ~50% bit flips
 * 3. **Good distribution**: Passes all SMHasher tests
 * 4. **Widely used**: Popular in Bloom filters, hash tables, and checksums
 *
 * This implementation provides:
 * - 32-bit variant (MurmurHash3_x86_32)
 * - 128-bit variant (MurmurHash3_x64_128)
 *
 * @see https://github.com/aappleby/smhasher
 */
template<typename HashType = hash64>
class murmur3_hash : public hash_function_base<murmur3_hash<HashType>, HashType> {
public:
    using hash_type = HashType;
    using base_type = hash_function_base<murmur3_hash<HashType>, HashType>;

    static_assert(concepts::HashValue<HashType>, "HashType must be a valid hash value type");

private:
    std::uint32_t seed_ = 0;

    // Type tags for specialization
    template<typename T> struct hash_size_tag {};
    using tag_32 = hash_size_tag<hash32>;
    using tag_64 = hash_size_tag<hash64>;
    using tag_128 = hash_size_tag<hash128>;
    using tag_other = hash_size_tag<void>;

    static constexpr auto determine_tag() {
        if constexpr (std::same_as<HashType, hash32>) {
            return tag_32{};
        } else if constexpr (std::same_as<HashType, hash64>) {
            return tag_64{};
        } else if constexpr (std::same_as<HashType, hash128>) {
            return tag_128{};
        } else {
            return tag_other{};
        }
    }

    using current_tag = decltype(determine_tag());

public:
    /**
     * @brief Default constructor with seed = 0
     */
    constexpr murmur3_hash() = default;

    /**
     * @brief Construct with specific seed
     * @param seed The seed value for the hash function
     */
    constexpr explicit murmur3_hash(std::uint32_t seed) : seed_(seed) {}

    /**
     * @brief Get current seed
     */
    constexpr std::uint32_t seed() const noexcept { return seed_; }

    /**
     * @brief Set seed
     */
    constexpr void set_seed(std::uint32_t seed) noexcept { seed_ = seed; }

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
        return std::string("MurmurHash3-") + std::to_string(sizeof(HashType) * 8);
    }

    /**
     * @brief Entropy estimate - MurmurHash3 has excellent distribution
     */
    double entropy_impl() const {
        return sizeof(HashType) * 8.0 * 0.98; // Near-theoretical maximum
    }

private:
    // ==================== MurmurHash3 Constants ====================

    // 32-bit constants
    static constexpr std::uint32_t c1_32 = 0xcc9e2d51;
    static constexpr std::uint32_t c2_32 = 0x1b873593;

    // 128-bit constants (x64)
    static constexpr std::uint64_t c1_128 = 0x87c37b91114253d5ULL;
    static constexpr std::uint64_t c2_128 = 0x4cf5ad432745937fULL;

    // ==================== Helper Functions ====================

    static constexpr std::uint32_t rotl32(std::uint32_t x, int r) noexcept {
        return (x << r) | (x >> (32 - r));
    }

    static constexpr std::uint64_t rotl64(std::uint64_t x, int r) noexcept {
        return (x << r) | (x >> (64 - r));
    }

    // Finalization mix - force all bits to avalanche
    static constexpr std::uint32_t fmix32(std::uint32_t h) noexcept {
        h ^= h >> 16;
        h *= 0x85ebca6b;
        h ^= h >> 13;
        h *= 0xc2b2ae35;
        h ^= h >> 16;
        return h;
    }

    static constexpr std::uint64_t fmix64(std::uint64_t k) noexcept {
        k ^= k >> 33;
        k *= 0xff51afd7ed558ccdULL;
        k ^= k >> 33;
        k *= 0xc4ceb9fe1a85ec53ULL;
        k ^= k >> 33;
        return k;
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

    // ==================== 32-bit MurmurHash3 ====================

    template<concepts::Hashable T>
    constexpr hash32 hash_dispatch(T const& input, tag_32) const {
        auto bytes = get_byte_representation(input);
        const std::uint8_t* data = bytes.data();
        const std::size_t len = bytes.size();
        const int nblocks = static_cast<int>(len / 4);

        std::uint32_t h1 = seed_;

        // Body - process 4-byte blocks
        const std::uint32_t* blocks = reinterpret_cast<const std::uint32_t*>(data);

        for (int i = 0; i < nblocks; i++) {
            std::uint32_t k1;
            std::memcpy(&k1, &blocks[i], sizeof(k1));

            k1 *= c1_32;
            k1 = rotl32(k1, 15);
            k1 *= c2_32;

            h1 ^= k1;
            h1 = rotl32(h1, 13);
            h1 = h1 * 5 + 0xe6546b64;
        }

        // Tail - handle remaining bytes
        const std::uint8_t* tail = data + nblocks * 4;
        std::uint32_t k1 = 0;

        switch (len & 3) {
            case 3: k1 ^= static_cast<std::uint32_t>(tail[2]) << 16; [[fallthrough]];
            case 2: k1 ^= static_cast<std::uint32_t>(tail[1]) << 8;  [[fallthrough]];
            case 1: k1 ^= static_cast<std::uint32_t>(tail[0]);
                    k1 *= c1_32;
                    k1 = rotl32(k1, 15);
                    k1 *= c2_32;
                    h1 ^= k1;
        }

        // Finalization
        h1 ^= static_cast<std::uint32_t>(len);
        h1 = fmix32(h1);

        hash32 result;
        std::memcpy(result.data().data(), &h1, sizeof(h1));
        return result;
    }

    // ==================== 64-bit (uses 128-bit and truncates) ====================

    template<concepts::Hashable T>
    constexpr hash64 hash_dispatch(T const& input, tag_64) const {
        auto full_hash = hash_dispatch(input, tag_128{});
        return truncate<8>(full_hash);
    }

    // ==================== 128-bit MurmurHash3 (x64) ====================

    template<concepts::Hashable T>
    constexpr hash128 hash_dispatch(T const& input, tag_128) const {
        auto bytes = get_byte_representation(input);
        const std::uint8_t* data = bytes.data();
        const std::size_t len = bytes.size();
        const int nblocks = static_cast<int>(len / 16);

        std::uint64_t h1 = seed_;
        std::uint64_t h2 = seed_;

        // Body - process 16-byte blocks
        const std::uint64_t* blocks = reinterpret_cast<const std::uint64_t*>(data);

        for (int i = 0; i < nblocks; i++) {
            std::uint64_t k1, k2;
            std::memcpy(&k1, &blocks[i * 2], sizeof(k1));
            std::memcpy(&k2, &blocks[i * 2 + 1], sizeof(k2));

            k1 *= c1_128;
            k1 = rotl64(k1, 31);
            k1 *= c2_128;
            h1 ^= k1;

            h1 = rotl64(h1, 27);
            h1 += h2;
            h1 = h1 * 5 + 0x52dce729;

            k2 *= c2_128;
            k2 = rotl64(k2, 33);
            k2 *= c1_128;
            h2 ^= k2;

            h2 = rotl64(h2, 31);
            h2 += h1;
            h2 = h2 * 5 + 0x38495ab5;
        }

        // Tail - handle remaining bytes
        const std::uint8_t* tail = data + nblocks * 16;
        std::uint64_t k1 = 0;
        std::uint64_t k2 = 0;

        switch (len & 15) {
            case 15: k2 ^= static_cast<std::uint64_t>(tail[14]) << 48; [[fallthrough]];
            case 14: k2 ^= static_cast<std::uint64_t>(tail[13]) << 40; [[fallthrough]];
            case 13: k2 ^= static_cast<std::uint64_t>(tail[12]) << 32; [[fallthrough]];
            case 12: k2 ^= static_cast<std::uint64_t>(tail[11]) << 24; [[fallthrough]];
            case 11: k2 ^= static_cast<std::uint64_t>(tail[10]) << 16; [[fallthrough]];
            case 10: k2 ^= static_cast<std::uint64_t>(tail[9])  << 8;  [[fallthrough]];
            case 9:  k2 ^= static_cast<std::uint64_t>(tail[8]);
                     k2 *= c2_128;
                     k2 = rotl64(k2, 33);
                     k2 *= c1_128;
                     h2 ^= k2;
                     [[fallthrough]];

            case 8:  k1 ^= static_cast<std::uint64_t>(tail[7]) << 56; [[fallthrough]];
            case 7:  k1 ^= static_cast<std::uint64_t>(tail[6]) << 48; [[fallthrough]];
            case 6:  k1 ^= static_cast<std::uint64_t>(tail[5]) << 40; [[fallthrough]];
            case 5:  k1 ^= static_cast<std::uint64_t>(tail[4]) << 32; [[fallthrough]];
            case 4:  k1 ^= static_cast<std::uint64_t>(tail[3]) << 24; [[fallthrough]];
            case 3:  k1 ^= static_cast<std::uint64_t>(tail[2]) << 16; [[fallthrough]];
            case 2:  k1 ^= static_cast<std::uint64_t>(tail[1]) << 8;  [[fallthrough]];
            case 1:  k1 ^= static_cast<std::uint64_t>(tail[0]);
                     k1 *= c1_128;
                     k1 = rotl64(k1, 31);
                     k1 *= c2_128;
                     h1 ^= k1;
        }

        // Finalization
        h1 ^= static_cast<std::uint64_t>(len);
        h2 ^= static_cast<std::uint64_t>(len);

        h1 += h2;
        h2 += h1;

        h1 = fmix64(h1);
        h2 = fmix64(h2);

        h1 += h2;
        h2 += h1;

        hash128 result;
        std::memcpy(result.data().data(), &h1, sizeof(h1));
        std::memcpy(result.data().data() + 8, &h2, sizeof(h2));
        return result;
    }

    // ==================== Other sizes ====================

    template<concepts::Hashable T>
    constexpr HashType hash_dispatch(T const& input, tag_other) const {
        if constexpr (sizeof(HashType) <= 4) {
            auto h32 = hash_dispatch(input, tag_32{});
            return truncate<sizeof(HashType)>(h32);
        } else if constexpr (sizeof(HashType) <= 16) {
            auto h128 = hash_dispatch(input, tag_128{});
            return truncate<sizeof(HashType)>(h128);
        } else {
            // For larger sizes, repeat with different seeds
            HashType result;
            auto result_bytes = result.bytes();
            std::size_t offset = 0;
            std::uint32_t current_seed = seed_;

            while (offset < sizeof(HashType)) {
                murmur3_hash<hash128> sub_hasher(current_seed);
                auto chunk = sub_hasher(input);
                std::size_t copy_bytes = std::min(sizeof(hash128), sizeof(HashType) - offset);
                std::memcpy(result_bytes.data() + offset, chunk.data().data(), copy_bytes);
                offset += copy_bytes;
                current_seed += 0x9e3779b9; // Golden ratio
            }
            return result;
        }
    }
};

// ==================== Convenience Aliases ====================

/**
 * @brief 32-bit MurmurHash3 (x86)
 */
using murmur3_32 = murmur3_hash<hash32>;

/**
 * @brief 64-bit MurmurHash3 (truncated from 128-bit x64)
 */
using murmur3_64 = murmur3_hash<hash64>;

/**
 * @brief 128-bit MurmurHash3 (x64)
 */
using murmur3_128 = murmur3_hash<hash128>;

} // namespace algebraic_hashing::functions
