#pragma once

#include <concepts>
#include <type_traits>
#include <ranges>
#include <functional>

namespace algebraic_hashing::concepts {

/**
 * @brief Core concept defining what it means to be hashable
 * 
 * A type is Hashable if it can be converted to a sequence of bytes
 * for processing by hash functions. This is the foundational concept
 * for our algebraic hashing system.
 */
template<typename T>
concept Hashable = requires(T const& t) {
    // Allow const char arrays (string literals) and other types
    requires std::is_array_v<T> || std::copy_constructible<T>;
    
    // Should have some way to be converted to bytes (implementation detail)
    // This will be handled by hash function implementations
};

/**
 * @brief Concept defining the structure of a hash digest/value
 * 
 * A HashValue represents the output of a hash function. It must support
 * algebraic operations that allow composition and combination.
 */
template<typename H>
concept HashValue = requires(H h1, H h2) {
    // Must be regular (copyable, movable, equality comparable)
    requires std::regular<H>;
    
    // Must support XOR operation (fundamental for algebraic composition)
    { h1 ^ h2 } -> std::same_as<H>;
    h1 ^= h2;
    
    // Must support complement (important for algebraic properties)
    { ~h1 } -> std::same_as<H>;
    
    // Must have hex string conversion for display/debugging  
    { h1.to_hex() } -> std::convertible_to<std::string>;
};

/**
 * @brief Fundamental concept for hash functions in our algebra
 * 
 * A HashFunction is a callable that maps Hashable types to HashValues.
 * This forms the basis of our algebraic operations.
 */
template<typename F>
concept HashFunction = requires(F f) {
    // Must be callable
    // Must be callable with some input
    // Note: We can't check invocable without knowing arguments
    
    // Must be copyable for composition operations
    requires std::copy_constructible<F>;
};

/**
 * @brief Concept for hash functions that can be algebraically combined
 * 
 * ComposableHashFunction extends HashFunction with requirements for
 * algebraic composition operations.
 */
template<typename F>
concept ComposableHashFunction = HashFunction<F> && requires {
    // Must expose its output hash type
    typename std::remove_cvref_t<F>::hash_type;
    
    // Output type must satisfy HashValue concept
    requires HashValue<typename std::remove_cvref_t<F>::hash_type>;
    
    // Must be default constructible for algebraic identity elements
    requires std::is_default_constructible_v<std::remove_cvref_t<F>>;
};

/**
 * @brief Concept for cryptographic hash functions
 * 
 * Extends ComposableHashFunction with cryptographic properties and
 * streaming interface for large inputs.
 */
template<typename F>
concept CryptographicHashFunction = ComposableHashFunction<F> && requires(F f) {
    // Must support streaming updates
    { f.update(std::declval<std::byte const*>(), std::declval<std::size_t>()) } -> std::same_as<F&>;
    
    // Must be able to finalize and get digest
    { f.finalize() } -> HashValue;
    
    // Must provide entropy estimate
    { f.entropy() } -> std::convertible_to<double>;
    
    // Must be resettable
    f.reset();
};

/**
 * @brief Concept for perfect hash functions
 * 
 * Perfect hash functions provide collision-free hashing for a specific
 * set of inputs.
 */
template<typename F>
concept PerfectHashFunction = ComposableHashFunction<F> && requires(F f) {
    // Must provide information about the domain size
    { f.domain_size() } -> std::convertible_to<std::size_t>;
    
    // Must provide range information
    { f.range_size() } -> std::convertible_to<std::size_t>;
    
    // Must indicate if it's minimal (range size == domain size)
    { f.is_minimal() } -> std::convertible_to<bool>;
};

/**
 * @brief Concept for algebraic operations on hash functions
 * 
 * Defines the interface for combining hash functions algebraically.
 */
template<typename Op, typename F1, typename F2>
concept AlgebraicOperation = requires(Op op, F1 f1, F2 f2) {
    requires ComposableHashFunction<F1>;
    requires ComposableHashFunction<F2>;
    
    // Must produce a new composable hash function
    requires ComposableHashFunction<decltype(op(f1, f2))>;
    
    // Must be associative and have algebraic properties (verified by tests)
};

} // namespace algebraic_hashing::concepts