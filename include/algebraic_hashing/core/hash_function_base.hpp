#pragma once

#include "concepts.hpp"
#include "hash_value.hpp"
#include <string>
#include <memory>
#include <type_traits>
#include <chrono>

namespace algebraic_hashing {

/**
 * @brief Base class for hash functions providing common interface
 * 
 * This class provides a pedagogical foundation for understanding hash functions
 * and their algebraic properties. It demonstrates key concepts:
 * 
 * 1. **Determinism**: Same input always produces same output
 * 2. **Avalanche Effect**: Small input changes cause large output changes  
 * 3. **Uniform Distribution**: Outputs should be uniformly distributed
 * 4. **Efficiency**: Fast computation for practical use
 * 
 * The class uses CRTP (Curiously Recurring Template Pattern) to provide
 * static polymorphism while maintaining performance.
 */
template<typename Derived, typename HashType>
class hash_function_base {
public:
    using hash_type = HashType;
    using derived_type = Derived;
    
    static_assert(concepts::HashValue<HashType>, 
        "HashType must satisfy HashValue concept");

protected:
    /**
     * @brief Statistics for pedagogical analysis
     *
     * @warning NOT THREAD-SAFE: Statistics collection is not thread-safe.
     * If you use hash functions concurrently from multiple threads, do NOT
     * rely on statistics being accurate. Statistics are intended for
     * educational/debugging purposes in single-threaded contexts only.
     *
     * For production multi-threaded use, either:
     * 1. Ignore statistics (they may be inaccurate but won't cause crashes)
     * 2. Use separate hash function instances per thread
     * 3. Disable statistics collection with ALGEBRAIC_HASHING_DISABLE_STATISTICS
     *
     * The hash function itself IS thread-safe (const operations don't modify state),
     * but statistics tracking uses mutable state that is not synchronized.
     */
    struct statistics {
        std::size_t calls = 0;
        std::size_t total_input_bytes = 0;
        std::chrono::nanoseconds total_time{0};

        double average_time_ns() const {
            return calls > 0 ? static_cast<double>(total_time.count()) / calls : 0.0;
        }

        double throughput_mbps() const {
            if (total_time.count() == 0) return 0.0;
            double seconds = total_time.count() / 1e9;
            double megabytes = total_input_bytes / (1024.0 * 1024.0);
            return megabytes / seconds;
        }
    };

    #ifndef ALGEBRAIC_HASHING_DISABLE_STATISTICS
    mutable statistics stats_;
    #endif

public:
    // ==================== Core Interface ====================
    
    /**
     * @brief Main hash function interface
     *
     * This is the primary entry point that delegates to the derived class
     * implementation while optionally collecting statistics.
     *
     * @note Statistics collection can be disabled with ALGEBRAIC_HASHING_DISABLE_STATISTICS
     */
    template<concepts::Hashable T>
    hash_type operator()(T const& input) const {
        #ifndef ALGEBRAIC_HASHING_DISABLE_STATISTICS
        auto start = std::chrono::high_resolution_clock::now();
        #endif

        hash_type result = static_cast<Derived const&>(*this).hash_impl(input);

        #ifndef ALGEBRAIC_HASHING_DISABLE_STATISTICS
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

        // Update statistics (mutable for const correctness)
        // WARNING: Not thread-safe! See statistics struct documentation.
        ++stats_.calls;
        stats_.total_input_bytes += estimate_input_size(input);
        stats_.total_time += duration;
        #endif

        return result;
    }
    
    /**
     * @brief Get hash function name for debugging/logging
     */
    std::string name() const {
        if constexpr (requires { static_cast<Derived const&>(*this).name_impl(); }) {
            return static_cast<Derived const&>(*this).name_impl();
        } else {
            return typeid(Derived).name();
        }
    }
    
    /**
     * @brief Get entropy estimate in bits
     * 
     * Default implementation assumes full entropy based on output size.
     * Derived classes should override for more accurate estimates.
     */
    double entropy() const {
        if constexpr (requires { static_cast<Derived const&>(*this).entropy_impl(); }) {
            return static_cast<Derived const&>(*this).entropy_impl();
        } else {
            return sizeof(HashType) * 8.0; // Assume full entropy
        }
    }
    
    // ==================== Pedagogical Interface ====================
    
    /**
     * @brief Demonstrate avalanche effect
     * 
     * Shows how small input changes create large output changes.
     * This is a key property of good hash functions.
     */
    template<concepts::Hashable T>
    double measure_avalanche_effect(T const& input) const {
        // This is a simplified demonstration - real avalanche tests
        // would use bit-level input modifications
        hash_type original = (*this)(input);
        
        // Create a slightly modified input (conceptual)
        // In practice, we'd flip individual bits
        hash_type modified = ~original; // Placeholder for demonstration
        
        // Count differing bits
        auto xor_result = original ^ modified;
        return static_cast<double>(xor_result.popcount()) / (sizeof(HashType) * 8);
    }
    
    /**
     * @brief Get performance statistics
     *
     * @note Only available when statistics are enabled (default).
     * @warning Statistics are NOT thread-safe. Values may be inaccurate
     *          if hash function is used concurrently.
     */
    #ifndef ALGEBRAIC_HASHING_DISABLE_STATISTICS
    statistics const& get_statistics() const noexcept {
        return stats_;
    }

    /**
     * @brief Reset statistics
     *
     * @warning NOT thread-safe. Do not call while hash function is being
     *          used concurrently from other threads.
     */
    void reset_statistics() const noexcept {
        stats_ = statistics{};
    }
    #endif
    
    /**
     * @brief Test hash distribution quality (simplified)
     * 
     * In practice, this would use more sophisticated statistical tests.
     */
    template<std::ranges::range R>
    double measure_distribution_quality(R const& inputs) const 
        requires concepts::Hashable<std::ranges::range_value_t<R>> {
        
        if (std::ranges::empty(inputs)) return 0.0;
        
        // Simple variance-based measure
        std::vector<hash_type> hashes;
        for (auto const& input : inputs) {
            hashes.push_back((*this)(input));
        }
        
        // Calculate some basic distribution metrics
        // In practice, would use chi-squared test, etc.
        return 1.0; // Placeholder - real implementation would calculate variance
    }
    
    // ==================== Utility Functions ====================
    
    /**
     * @brief Create a salted version of this hash function
     * 
     * Salting is important for security and helps prevent certain attacks.
     */
    auto with_salt(hash_type const& salt) const {
        return [*this, salt](auto const& input) {
            auto base_hash = (*this)(input);
            return base_hash ^ salt; // Simple XOR salting
        };
    }
    
    /**
     * @brief Create a truncated version with smaller output
     */
    template<std::size_t NewSize>
    auto truncated() const 
        requires (NewSize < sizeof(HashType)) {
        return [*this](auto const& input) {
            auto full_hash = (*this)(input);
            return truncate<NewSize>(full_hash);
        };
    }

private:
    /**
     * @brief Estimate input size for statistics
     */
    template<typename T>
    static std::size_t estimate_input_size(T const& input) {
        if constexpr (std::is_arithmetic_v<T>) {
            return sizeof(T);
        } else if constexpr (requires { input.size(); }) {
            return input.size();
        } else {
            return sizeof(T); // Fallback estimate
        }
    }
};

// ==================== Convenience Aliases ====================

/**
 * @brief Base class for 32-bit hash functions
 */
template<typename Derived>
using hash_function_32 = hash_function_base<Derived, hash32>;

/**
 * @brief Base class for 64-bit hash functions  
 */
template<typename Derived>
using hash_function_64 = hash_function_base<Derived, hash64>;

/**
 * @brief Base class for 128-bit hash functions (like MD5)
 */
template<typename Derived>
using hash_function_128 = hash_function_base<Derived, hash128>;

/**
 * @brief Base class for 256-bit hash functions (like SHA-256)
 */
template<typename Derived>
using hash_function_256 = hash_function_base<Derived, hash256>;

// ==================== Educational Helpers ====================

/**
 * @brief Concept checker for educational purposes
 * 
 * This function template helps students understand what makes
 * a valid hash function by checking concepts at compile time.
 */
template<typename F>
consteval bool is_valid_hash_function() {
    return concepts::ComposableHashFunction<F>;
}

/**
 * @brief Educational function to explain hash function properties
 */
template<concepts::ComposableHashFunction F>
std::string explain_hash_function(F const& f) {
    std::string explanation = "Hash Function Analysis:\n";
    explanation += "- Name: " + f.name() + "\n";
    explanation += "- Output size: " + std::to_string(sizeof(typename F::hash_type)) + " bytes\n";
    explanation += "- Entropy: " + std::to_string(f.entropy()) + " bits\n";
    
    if constexpr (requires { f.get_statistics(); }) {
        auto stats = f.get_statistics();
        explanation += "- Calls made: " + std::to_string(stats.calls) + "\n";
        explanation += "- Average time: " + std::to_string(stats.average_time_ns()) + " ns\n";
        explanation += "- Throughput: " + std::to_string(stats.throughput_mbps()) + " MB/s\n";
    }
    
    return explanation;
}

} // namespace algebraic_hashing