# Design Philosophy

AlgebraicHashing embodies specific design principles that shape every aspect of the library. Understanding these principles helps you use the library effectively and extend it correctly.

## Core Principles

### 1. Mathematical Rigor

Every abstraction in the library corresponds to a well-defined mathematical concept:

- **Hash values** are elements of an abelian group
- **Hash functions** are morphisms in a category
- **Composition** preserves algebraic properties
- **Operators** mirror mathematical notation

This isn't just academic elegance—it provides guarantees:

```cpp
// Mathematical properties mean predictable behavior
auto h = f ^ g;
assert(h ^ h == zero_hash);  // Always true by group theory
```

### 2. Concepts as Contracts

C++20 concepts express mathematical requirements as type constraints:

```cpp
template<typename T>
concept HashValue = requires(T h1, T h2) {
    { h1 ^ h2 } -> std::same_as<T>;    // Group operation
    { ~h1 } -> std::same_as<T>;         // Complement
    { h1.to_hex() } -> std::convertible_to<std::string>;
};
```

Benefits:

- **Compile-time verification**: Errors are caught early
- **Clear error messages**: Concepts explain what's missing
- **Self-documenting**: The code IS the specification

### 3. Zero-Cost Abstractions

The library uses template metaprogramming to ensure composition has no runtime overhead:

```cpp
// This composed function
auto h = fnv64{} ^ murmur3_64{};

// Compiles to the same code as
// manually writing: fnv(x) ^ murmur(x)
```

Techniques used:

- **CRTP** for static polymorphism
- **Inline everything** for optimizer visibility
- **constexpr** where possible for compile-time evaluation

### 4. Pedagogical Value

The library is designed to teach as well as perform:

- **Clear naming**: `xor_composition`, `sequential_composition`
- **Extensive documentation**: Every function explains the "why"
- **Educational demos**: Built-in examples of hash properties
- **Mathematical comments**: Formulas in the code

```cpp
/**
 * @brief XOR Composition: h₁ ⊕ h₂
 *
 * Mathematical properties:
 * - Commutative: h₁ ⊕ h₂ = h₂ ⊕ h₁
 * - Associative: (h₁ ⊕ h₂) ⊕ h₃ = h₁ ⊕ (h₂ ⊕ h₃)
 * - Has identity: h ⊕ zero_hash = h
 * - Self-inverse: h ⊕ h = zero_hash
 */
```

## Design Decisions

### Why XOR for Composition?

XOR was chosen as the primary composition operation because:

1. **Algebraic structure**: Forms a proper group
2. **Bitwise operation**: Extremely fast
3. **Self-inverse**: Enables interesting patterns
4. **No overflow**: Unlike addition/multiplication

### Why Operator Overloading?

Mathematical notation makes the DSL intuitive:

```cpp
// With operators (clear mathematical intent)
auto h = (f ^ g) * k;

// Without operators (verbose and unclear)
auto h = sequential_composition(
    xor_composition(f, g), k
);
```

### Why CRTP over Virtual Functions?

The Curiously Recurring Template Pattern provides:

- **No vtable overhead**: Important for tight loops
- **Inlining**: Optimizer can see through the abstraction
- **Static dispatch**: Known at compile time

```cpp
template<typename Derived, typename HashType>
class hash_function_base {
    auto operator()(auto const& input) {
        // Statistics, validation, etc.
        return static_cast<Derived*>(this)->hash_impl(input);
    }
};
```

### Why Header-Only?

Header-only design enables:

- **Easy integration**: Just include and use
- **Template instantiation**: Full generic programming
- **Compiler optimization**: Everything visible
- **No link errors**: Common pain point eliminated

## Influences and Inspirations

### Haskell Type Classes

The concept hierarchy mirrors Haskell's type class approach:

```haskell
-- Haskell equivalent
class HashValue a where
    xor :: a -> a -> a
    complement :: a -> a
```

### Category Theory

The library's structure reflects categorical thinking:

- Objects → Types
- Morphisms → Functions
- Composition → Sequential application

### Unix Philosophy

Each component does one thing well:

- `hash_value`: Represent digests
- `fnv_hash`: Compute FNV hashes
- `xor_composition`: Combine via XOR

## Trade-offs

### Compile Time vs Runtime

Heavy template use increases compile time. This is acceptable because:

- Hash functions are typically instantiated once
- Runtime performance is critical
- Modern compilers handle templates well

### Flexibility vs Safety

The library restricts operations that don't make mathematical sense:

```cpp
// Compile error: can't XOR different hash sizes
fnv32{} ^ fnv64{};  // Error!

// This is intentional—it prevents subtle bugs
```

### Simplicity vs Features

The library focuses on composition fundamentals rather than every possible hash function. Users can easily add their own:

```cpp
class my_hash : public hash_function_base<my_hash, hash64> {
    hash64 hash_impl(auto const& input) const {
        // Your implementation
    }
};
// Now my_hash works with all composition operators
```

## Evolution Strategy

The library follows semantic versioning with these principles:

1. **Core abstractions are stable**: `HashValue`, `ComposableHashFunction`
2. **New functions can be added**: Won't break existing code
3. **Deprecation before removal**: Time to migrate
4. **Mathematical correctness over features**: We won't add broken operations

## Further Reading

- [Core Concepts](core-concepts.md) - The type system in detail
- [Algebraic Structure](algebraic-structure.md) - Mathematical foundations
- [Contributing](../reference/contributing.md) - How to extend the library
