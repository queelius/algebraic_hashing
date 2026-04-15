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
 * @brief Modern SHA-256 Implementation
 *
 * SHA-256 is a cryptographic hash function from the SHA-2 family, designed
 * by the NSA and published by NIST. It produces a 256-bit (32-byte) hash.
 *
 * Properties:
 * 1. **Cryptographic security**: Collision-resistant, preimage-resistant
 * 2. **Fixed output**: Always produces 256-bit output regardless of input size
 * 3. **Deterministic**: Same input always produces same output
 * 4. **Avalanche effect**: Small changes create completely different hashes
 *
 * This implementation:
 * - Follows the modern C++20 architecture
 * - Supports streaming updates for large inputs
 * - Integrates with the algebraic hashing DSL
 *
 * @note For non-cryptographic use cases, prefer FNV, MurmurHash3, or xxHash
 *       as they are significantly faster.
 *
 * @see FIPS 180-4: Secure Hash Standard
 */
class sha256_hash : public hash_function_base<sha256_hash, hash256> {
public:
    using hash_type = hash256;
    using base_type = hash_function_base<sha256_hash, hash256>;

    static constexpr std::size_t digest_size = 32;  // 256 bits
    static constexpr std::size_t block_size = 64;   // 512 bits

private:
    // SHA-256 round constants (first 32 bits of fractional parts of cube roots of first 64 primes)
    static constexpr std::array<std::uint32_t, 64> K = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    // Initial hash values (first 32 bits of fractional parts of square roots of first 8 primes)
    static constexpr std::array<std::uint32_t, 8> H0 = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    // State for streaming mode (mutable for const hash_impl)
    mutable std::array<std::uint32_t, 8> state_{};
    mutable std::array<std::uint8_t, block_size> buffer_{};
    mutable std::size_t buffer_len_ = 0;
    mutable std::uint64_t total_len_ = 0;
    mutable bool finalized_ = false;

public:
    /**
     * @brief Default constructor - initializes state
     */
    sha256_hash() { reset(); }

    /**
     * @brief Reset to initial state
     */
    sha256_hash const& reset() const noexcept {
        state_ = H0;
        buffer_.fill(0);
        buffer_len_ = 0;
        total_len_ = 0;
        finalized_ = false;
        return *this;
    }

    /**
     * @brief Update hash with more data (streaming interface)
     */
    sha256_hash const& update(const std::uint8_t* data, std::size_t len) const {
        if (finalized_) {
            reset();
        }

        total_len_ += len;

        // If we have buffered data, try to complete a block
        if (buffer_len_ > 0) {
            std::size_t to_copy = std::min(block_size - buffer_len_, len);
            std::memcpy(buffer_.data() + buffer_len_, data, to_copy);
            buffer_len_ += to_copy;
            data += to_copy;
            len -= to_copy;

            if (buffer_len_ == block_size) {
                transform(buffer_.data());
                buffer_len_ = 0;
            }
        }

        // Process complete blocks
        while (len >= block_size) {
            transform(data);
            data += block_size;
            len -= block_size;
        }

        // Buffer remaining data
        if (len > 0) {
            std::memcpy(buffer_.data(), data, len);
            buffer_len_ = len;
        }

        return *this;
    }

    /**
     * @brief Finalize and return hash
     */
    hash256 finalize() const {
        if (!finalized_) {
            // Padding
            std::size_t pad_len = (buffer_len_ < 56) ? (56 - buffer_len_) : (120 - buffer_len_);

            std::array<std::uint8_t, 128> padding{};
            padding[0] = 0x80;

            // Append length in bits (big-endian)
            std::uint64_t bit_len = total_len_ * 8;
            for (int i = 0; i < 8; ++i) {
                padding[pad_len + 7 - i] = static_cast<std::uint8_t>(bit_len >> (i * 8));
            }

            update(padding.data(), pad_len + 8);
            finalized_ = true;
        }

        // Convert state to hash value (big-endian)
        hash256 result;
        auto bytes = result.bytes();
        for (int i = 0; i < 8; ++i) {
            bytes[i * 4 + 0] = static_cast<std::uint8_t>(state_[i] >> 24);
            bytes[i * 4 + 1] = static_cast<std::uint8_t>(state_[i] >> 16);
            bytes[i * 4 + 2] = static_cast<std::uint8_t>(state_[i] >> 8);
            bytes[i * 4 + 3] = static_cast<std::uint8_t>(state_[i]);
        }

        return result;
    }

    /**
     * @brief Main hash implementation using CRTP
     */
    template<concepts::Hashable T>
    hash256 hash_impl(T const& input) const {
        reset();
        auto bytes = get_byte_representation(input);
        update(bytes.data(), bytes.size());
        return finalize();
    }

    /**
     * @brief Get function name
     */
    std::string name_impl() const {
        return "SHA-256";
    }

    /**
     * @brief Entropy estimate - SHA-256 provides full 256-bit security
     */
    double entropy_impl() const {
        return 256.0; // Full cryptographic strength
    }

private:
    // ==================== SHA-256 Core Functions ====================

    static constexpr std::uint32_t rotr(std::uint32_t x, int n) noexcept {
        return (x >> n) | (x << (32 - n));
    }

    static constexpr std::uint32_t ch(std::uint32_t x, std::uint32_t y, std::uint32_t z) noexcept {
        return (x & y) ^ (~x & z);
    }

    static constexpr std::uint32_t maj(std::uint32_t x, std::uint32_t y, std::uint32_t z) noexcept {
        return (x & y) ^ (x & z) ^ (y & z);
    }

    static constexpr std::uint32_t sigma0(std::uint32_t x) noexcept {
        return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
    }

    static constexpr std::uint32_t sigma1(std::uint32_t x) noexcept {
        return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
    }

    static constexpr std::uint32_t gamma0(std::uint32_t x) noexcept {
        return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
    }

    static constexpr std::uint32_t gamma1(std::uint32_t x) noexcept {
        return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
    }

    /**
     * @brief Transform a single 512-bit block
     */
    void transform(const std::uint8_t* block) const {
        std::array<std::uint32_t, 64> w;

        // Prepare message schedule
        for (int i = 0; i < 16; ++i) {
            w[i] = (static_cast<std::uint32_t>(block[i * 4 + 0]) << 24) |
                   (static_cast<std::uint32_t>(block[i * 4 + 1]) << 16) |
                   (static_cast<std::uint32_t>(block[i * 4 + 2]) << 8) |
                   (static_cast<std::uint32_t>(block[i * 4 + 3]));
        }

        for (int i = 16; i < 64; ++i) {
            w[i] = gamma1(w[i - 2]) + w[i - 7] + gamma0(w[i - 15]) + w[i - 16];
        }

        // Initialize working variables
        std::uint32_t a = state_[0];
        std::uint32_t b = state_[1];
        std::uint32_t c = state_[2];
        std::uint32_t d = state_[3];
        std::uint32_t e = state_[4];
        std::uint32_t f = state_[5];
        std::uint32_t g = state_[6];
        std::uint32_t h = state_[7];

        // Main loop
        for (int i = 0; i < 64; ++i) {
            std::uint32_t t1 = h + sigma1(e) + ch(e, f, g) + K[i] + w[i];
            std::uint32_t t2 = sigma0(a) + maj(a, b, c);

            h = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
        }

        // Add to state
        state_[0] += a;
        state_[1] += b;
        state_[2] += c;
        state_[3] += d;
        state_[4] += e;
        state_[5] += f;
        state_[6] += g;
        state_[7] += h;
    }

    // ==================== Byte Representation ====================

    template<typename T>
    auto get_byte_representation(T const& input) const {
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
    auto get_arithmetic_bytes(T value) const {
        std::array<std::uint8_t, sizeof(T)> bytes;
        for (std::size_t i = 0; i < sizeof(T); ++i) {
            bytes[i] = static_cast<std::uint8_t>(value & 0xFF);
            value >>= 8;
        }
        return bytes;
    }

    template<std::floating_point T>
    auto get_arithmetic_bytes(T value) const {
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
};

/**
 * @brief Convenience alias
 */
using sha256 = sha256_hash;

} // namespace algebraic_hashing::functions
