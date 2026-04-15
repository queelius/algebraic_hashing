# Type Safety with Concepts

This tutorial explains how AlgebraicHashing uses C++20 concepts to provide compile-time guarantees and clear error messages.

## Why Concepts?

Before C++20 concepts, template errors were notoriously cryptic. Concepts provide:

1. **Clear requirements**: What a type must provide
2. **Early error detection**: Fail at the point of use, not deep in implementation
3. **Better error messages**: Explain what's missing
4. **Self-documenting code**: The concept IS the specification

## The Concept Hierarchy

```
Hashable
    └── Any type that can be converted to bytes

HashValue
    └── Output type with algebraic operations

HashFunction
    └── Callable that maps Hashable → HashValue

ComposableHashFunction
    └── HashFunction that supports algebraic composition
```

## The `Hashable` Concept

A type is `Hashable` if the library can convert it to bytes:

```cpp
template<typename T>
concept Hashable = requires(T const& t) {
    requires std::is_array_v<T> || std::copy_constructible<T>;
};
```

### What's Hashable?

```cpp
fnv64 h;

// All of these work (Hashable types)
h(42);                          // int
h(3.14);                        // double
h("hello");                     // const char[]
h(std::string("world"));        // std::string
h(std::string_view("test"));    // std::string_view
h(std::vector<int>{1,2,3});     // std::vector

// Custom types work via raw memory representation
struct Point { int x, y; };
Point p{10, 20};
h(p);  // Hashes the raw bytes of the struct
```

## The `HashValue` Concept

Hash values must support algebraic operations:

```cpp
template<typename H>
concept HashValue = requires(H h1, H h2) {
    requires std::regular<H>;           // Copy, move, equality
    { h1 ^ h2 } -> std::same_as<H>;    // XOR operation
    h1 ^= h2;                           // XOR assignment
    { ~h1 } -> std::same_as<H>;        // Complement
    { h1.to_hex() } -> std::convertible_to<std::string>;
};
```

### Using HashValue

```cpp
void process_hash(HashValue auto h) {
    std::cout << h.to_hex() << "\n";
    auto flipped = ~h;
    // ...
}

hash64 h = fnv64{}("hello");
process_hash(h);  // Works!

// Won't compile - int doesn't satisfy HashValue
// process_hash(42);
```

## The `ComposableHashFunction` Concept

For algebraic composition, hash functions need additional properties:

```cpp
template<typename F>
concept ComposableHashFunction = HashFunction<F> && requires {
    typename std::remove_cvref_t<F>::hash_type;
    requires HashValue<typename std::remove_cvref_t<F>::hash_type>;
    requires std::is_default_constructible_v<std::remove_cvref_t<F>>;
};
```

### Type-Safe Composition

The composition operators only work on `ComposableHashFunction`:

```cpp
fnv64 f;      // Satisfies ComposableHashFunction
murmur3_64 g; // Satisfies ComposableHashFunction

// This compiles:
auto composed = f ^ g;

// Custom function must satisfy concept:
class my_hasher : public hash_function_base<my_hasher, hash64> {
public:
    using hash_type = hash64;  // Required!

    hash64 hash_impl(auto const& input) const {
        // Implementation
    }
};

my_hasher m;
auto also_works = f ^ m;  // Custom hasher works with composition
```

## Compile-Time Type Safety

### Preventing Size Mismatches

The library prevents combining incompatible hash sizes:

```cpp
fnv32 f32;   // 32-bit output
fnv64 f64;   // 64-bit output

// Compile error! Different output sizes
// auto bad = f32 ^ f64;

// Error message (concept-based):
// "Hash functions must produce same hash type for XOR composition"
```

### Concept Error Messages

When concepts fail, you get helpful messages:

```cpp
// Trying to hash an unhashable type
struct NoHash {
    NoHash(const NoHash&) = delete;  // Not copy constructible
};

fnv64 h;
NoHash x;
// h(x);  // Error: 'NoHash' does not satisfy 'Hashable'
```

## Writing Concept-Constrained Functions

### Basic Template Constraints

```cpp
// Function that works with any hash function
template<ComposableHashFunction F>
auto hash_all(F hasher, std::span<std::string> items) {
    std::vector<typename F::hash_type> results;
    results.reserve(items.size());
    for (const auto& item : items) {
        results.push_back(hasher(item));
    }
    return results;
}

// Usage
fnv64 h;
std::vector<std::string> items = {"a", "b", "c"};
auto hashes = hash_all(h, items);
```

### Combining Multiple Constraints

```cpp
template<ComposableHashFunction F1, ComposableHashFunction F2>
    requires std::same_as<typename F1::hash_type, typename F2::hash_type>
auto safe_xor(F1&& f1, F2&& f2) {
    return std::forward<F1>(f1) ^ std::forward<F2>(f2);
}

// Works: same output type
auto ok = safe_xor(fnv64{}, murmur3_64{});

// Won't compile: different output types
// auto bad = safe_xor(fnv32{}, fnv64{});
```

## Creating Custom Hash Functions

To create a hash function that works with the library:

```cpp
#include <algebraic_hashing/core/hash_function_base.hpp>

class my_hash : public hash_function_base<my_hash, hash64> {
public:
    // Required: expose hash type
    using hash_type = hash64;

    // Required: implement hash_impl
    template<concepts::Hashable T>
    hash64 hash_impl(T const& input) const {
        // Your implementation here
        hash64 result;
        // ... compute hash ...
        return result;
    }

    // Optional: name for debugging
    std::string name_impl() const {
        return "MyHash-64";
    }

    // Optional: entropy estimate
    double entropy_impl() const {
        return 64.0 * 0.95;
    }
};

// Now it works with all composition operators!
my_hash m;
fnv64 f;
auto composed = m ^ f;  // Works!
```

## Static Assertions

Use concepts in static assertions for clear errors:

```cpp
template<typename F>
class hash_wrapper {
    static_assert(ComposableHashFunction<F>,
        "F must be a composable hash function");

    F hasher_;

public:
    // ...
};

// Clear error if used with wrong type
// hash_wrapper<int> bad;  // Error: "F must be a composable hash function"
```

## Best Practices

### Use `auto` with Concepts

```cpp
// Let the compiler deduce, but constrain:
ComposableHashFunction auto h = fnv64{};

// Or in function parameters:
void process(ComposableHashFunction auto& hasher) {
    // ...
}
```

### Prefer Concepts Over SFINAE

```cpp
// Old way (SFINAE) - hard to read
template<typename F,
         typename = std::enable_if_t<is_hash_function_v<F>>>
void old_style(F f);

// New way (concepts) - clear and readable
template<ComposableHashFunction F>
void new_style(F f);

// Or even cleaner:
void cleanest(ComposableHashFunction auto f);
```

### Document Concept Requirements

```cpp
/**
 * @brief Hash all items in a range
 *
 * @tparam F Must satisfy ComposableHashFunction
 * @tparam R Must satisfy std::ranges::input_range with Hashable elements
 */
template<ComposableHashFunction F, std::ranges::input_range R>
    requires Hashable<std::ranges::range_value_t<R>>
auto hash_range(F hasher, R&& range);
```

## Next Steps

- [Performance Tutorial](performance.md) - Optimize your code
- [Core Concepts](../concepts/core-concepts.md) - Deep dive into the type system
- [Design Philosophy](../concepts/philosophy.md) - Why we chose concepts
