#pragma once

#include "../core/concepts.hpp"
#include "../core/hash_value.hpp"
#include <functional>
#include <utility>
#include <type_traits>
#include <cmath>

namespace algebraic_hashing::dsl {

/**
 * @brief Algebraic Operations DSL for Hash Functions
 * 
 * This module provides an elegant domain-specific language for composing
 * hash functions algebraically. The design philosophy follows mathematical
 * principles where hash functions can be:
 * 
 * 1. **Composed**: f ∘ g creates a new hash function
 * 2. **Combined**: f ⊕ g combines outputs algebraically  
 * 3. **Transformed**: Functions like lift, chain, repeat
 * 4. **Analyzed**: Properties like entropy, collision probability
 * 
 * The DSL uses operator overloading and template metaprogramming to create
 * an intuitive mathematical syntax for hash function algebra.
 */

// ==================== Forward Declarations ====================

template<concepts::ComposableHashFunction F1, concepts::ComposableHashFunction F2>
class xor_composition;

template<concepts::ComposableHashFunction F1, concepts::ComposableHashFunction F2>
class sequential_composition;

template<concepts::ComposableHashFunction F, std::size_t N>
class repeated_application;

template<concepts::ComposableHashFunction F>
class complemented_hash;

// ==================== XOR Composition ====================

/**
 * @brief XOR Composition: h₁ ⊕ h₂
 * 
 * Creates a new hash function that computes h₁(x) ⊕ h₂(x).
 * This is the fundamental algebraic operation in our hash function algebra.
 * 
 * Mathematical properties:
 * - Commutative: h₁ ⊕ h₂ = h₂ ⊕ h₁
 * - Associative: (h₁ ⊕ h₂) ⊕ h₃ = h₁ ⊕ (h₂ ⊕ h₃)
 * - Has identity: h ⊕ zero_hash = h
 * - Self-inverse: h ⊕ h = zero_hash
 */
template<concepts::ComposableHashFunction F1, concepts::ComposableHashFunction F2>
class xor_composition {
public:
    using hash_type = typename F1::hash_type;
    static_assert(std::same_as<hash_type, typename F2::hash_type>, 
        "Hash functions must produce same hash type for XOR composition");

private:
    F1 f1_;
    F2 f2_;

public:
    /**
     * @brief Default constructor (needed for concept compliance)
     */
    constexpr xor_composition() = default;
    
    /**
     * @brief Construct XOR composition from two hash functions
     */
    constexpr xor_composition(F1 f1, F2 f2) 
        : f1_(std::move(f1)), f2_(std::move(f2)) {}
    
    /**
     * @brief Apply the composed hash function
     * 
     * Computes f1(input) ⊕ f2(input) where ⊕ is the XOR operation
     * on hash values.
     */
    template<concepts::Hashable T>
    constexpr hash_type operator()(T const& input) const {
        return f1_(input) ^ f2_(input);
    }
    
    /**
     * @brief Access to component functions (for introspection)
     */
    constexpr F1 const& first() const noexcept { return f1_; }
    constexpr F2 const& second() const noexcept { return f2_; }
    
    /**
     * @brief Entropy estimate (conservative bound)
     * 
     * The entropy of XOR composition is at most the minimum of
     * the component entropies, but could be less due to correlation.
     */
    double entropy() const 
        requires requires { f1_.entropy(); f2_.entropy(); } {
        return std::min(f1_.entropy(), f2_.entropy());
    }
};

// ==================== Sequential Composition ====================

/**
 * @brief Sequential Composition: h₁ ∘ h₂
 * 
 * Creates a hash function that computes h₁(h₂(x)).
 * This represents function composition in the traditional sense.
 * 
 * Note: This typically reduces entropy since the intermediate hash
 * creates an information bottleneck.
 */
template<concepts::ComposableHashFunction F1, concepts::ComposableHashFunction F2>
class sequential_composition {
public:
    using hash_type = typename F1::hash_type;

private:
    F1 f1_;
    F2 f2_;

public:
    constexpr sequential_composition() = default;
    
    constexpr sequential_composition(F1 f1, F2 f2)
        : f1_(std::move(f1)), f2_(std::move(f2)) {}
    
    template<concepts::Hashable T>
    constexpr hash_type operator()(T const& input) const {
        auto intermediate = f2_(input);
        return f1_(intermediate);
    }
    
    constexpr F1 const& outer() const noexcept { return f1_; }
    constexpr F2 const& inner() const noexcept { return f2_; }
    
    double entropy() const 
        requires requires { f1_.entropy(); f2_.entropy(); } {
        // Sequential composition typically has lower entropy
        return std::min(f1_.entropy(), f2_.entropy()) * 0.9; // Conservative estimate
    }
};

// ==================== Repeated Application ====================

/**
 * @brief Repeated Application: h^n
 * 
 * Applies a hash function n times: h(h(h(...h(x)...)))
 * Useful for creating hash stretching functions.
 */
template<concepts::ComposableHashFunction F, std::size_t N>
class repeated_application {
public:
    using hash_type = typename F::hash_type;
    static_assert(N > 0, "Repetition count must be positive");

private:
    F f_;

public:
    constexpr repeated_application() = default;
    
    constexpr explicit repeated_application(F f) : f_(std::move(f)) {}
    
    template<concepts::Hashable T>
    constexpr hash_type operator()(T const& input) const {
        auto result = f_(input);
        for (std::size_t i = 1; i < N; ++i) {
            result = f_(result);
        }
        return result;
    }
    
    constexpr F const& function() const noexcept { return f_; }
    static constexpr std::size_t repetitions() noexcept { return N; }
    
    double entropy() const 
        requires requires { f_.entropy(); } {
        // Entropy typically decreases with repetition
        return f_.entropy() * std::pow(0.95, N - 1); // Empirical estimate
    }
};

// ==================== Complemented Hash ====================

/**
 * @brief Complemented Hash: ~h
 * 
 * Creates a hash function that computes ~h(x) where ~ is bitwise complement.
 * This is an involution: ~~h = h.
 */
template<concepts::ComposableHashFunction F>
class complemented_hash {
public:
    using hash_type = typename F::hash_type;

private:
    F f_;

public:
    constexpr complemented_hash() = default;
    
    constexpr explicit complemented_hash(F f) : f_(std::move(f)) {}
    
    template<concepts::Hashable T>
    constexpr hash_type operator()(T const& input) const {
        return ~f_(input);
    }
    
    constexpr F const& function() const noexcept { return f_; }
    
    double entropy() const 
        requires requires { f_.entropy(); } {
        return f_.entropy(); // Complement preserves entropy
    }
};

// ==================== Operator Overloads for DSL ====================

/**
 * @brief XOR operator: h1 ^ h2 creates XOR composition
 */
template<concepts::ComposableHashFunction F1, concepts::ComposableHashFunction F2>
constexpr auto operator^(F1&& f1, F2&& f2) {
    return xor_composition{std::forward<F1>(f1), std::forward<F2>(f2)};
}

/**
 * @brief Composition operator: h1 * h2 creates sequential composition
 * 
 * Using * for composition follows mathematical convention where
 * * often represents function composition.
 */
template<concepts::ComposableHashFunction F1, concepts::ComposableHashFunction F2>
constexpr auto operator*(F1&& f1, F2&& f2) {
    return sequential_composition{std::forward<F1>(f1), std::forward<F2>(f2)};
}

/**
 * @brief Complement operator: ~h creates complemented hash
 */
template<concepts::ComposableHashFunction F>
constexpr auto operator~(F&& f) {
    return complemented_hash{std::forward<F>(f)};
}

// ==================== Utility Functions ====================

/**
 * @brief Create repeated application: repeat<N>(h)
 */
template<std::size_t N, concepts::ComposableHashFunction F>
constexpr auto repeat(F&& f) {
    return repeated_application<std::decay_t<F>, N>{std::forward<F>(f)};
}

/**
 * @brief Chain multiple hash functions with XOR: chain(h1, h2, ..., hn)
 */
template<concepts::ComposableHashFunction F>
constexpr auto chain(F&& f) {
    return std::forward<F>(f);
}

template<concepts::ComposableHashFunction F1, concepts::ComposableHashFunction F2, 
         concepts::ComposableHashFunction... Rest>
constexpr auto chain(F1&& f1, F2&& f2, Rest&&... rest) {
    if constexpr (sizeof...(rest) == 0) {
        return std::forward<F1>(f1) ^ std::forward<F2>(f2);
    } else {
        return std::forward<F1>(f1) ^ chain(std::forward<F2>(f2), std::forward<Rest>(rest)...);
    }
}

/**
 * @brief Create parallel composition: parallel(h1, h2, ...) 
 * 
 * Applies all functions and concatenates their outputs.
 */
template<concepts::ComposableHashFunction... Fs>
class parallel_composition {
public:
    using hash_type = decltype(concatenate(std::declval<typename Fs::hash_type>()...));

private:
    std::tuple<Fs...> functions_;

public:
    constexpr explicit parallel_composition(Fs... fs) 
        : functions_(std::move(fs)...) {}
    
    template<concepts::Hashable T>
    constexpr hash_type operator()(T const& input) const {
        return apply_all(input, std::index_sequence_for<Fs...>{});
    }

private:
    template<concepts::Hashable T, std::size_t... Is>
    constexpr hash_type apply_all(T const& input, std::index_sequence<Is...>) const {
        return concatenate(std::get<Is>(functions_)(input)...);
    }
};

template<concepts::ComposableHashFunction... Fs>
constexpr auto parallel(Fs&&... fs) {
    return parallel_composition<std::decay_t<Fs>...>{std::forward<Fs>(fs)...};
}

// ==================== Analysis Functions ====================

/**
 * @brief Estimate collision probability for a hash function
 * 
 * Uses the birthday paradox formula: P ≈ n²/(2 × 2^k) where
 * n is number of inputs and k is hash bit length.
 */
template<concepts::ComposableHashFunction F>
double collision_probability(F const& f, std::size_t num_inputs) 
    requires requires { f.entropy(); } {
    double entropy_bits = f.entropy();
    double n = static_cast<double>(num_inputs);
    return (n * n) / (2.0 * std::pow(2.0, entropy_bits));
}

/**
 * @brief Analyze hash function properties
 */
template<concepts::ComposableHashFunction F>
struct hash_analysis {
    double entropy;
    std::size_t output_bits;
    std::string type_name;
    
    // Add more analysis properties as needed
    double expected_collisions_for(std::size_t inputs) const {
        return collision_probability_from_entropy(entropy, inputs);
    }

private:
    static double collision_probability_from_entropy(double entropy, std::size_t inputs) {
        double n = static_cast<double>(inputs);
        return (n * n) / (2.0 * std::pow(2.0, entropy));
    }
};

template<concepts::ComposableHashFunction F>
auto analyze(F const& f) -> hash_analysis<F> 
    requires requires { f.entropy(); } {
    return hash_analysis<F>{
        .entropy = f.entropy(),
        .output_bits = sizeof(typename F::hash_type) * 8,
        .type_name = typeid(F).name()
    };
}

} // namespace algebraic_hashing::dsl