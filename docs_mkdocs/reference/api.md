# API Reference

Complete API documentation for AlgebraicHashing.

## Doxygen Documentation

The full API reference is available in our **Doxygen-generated documentation**:

**[📚 Complete API Documentation →](https://queelius.github.io/algebraic_hashing/doxygen/)**

## Quick Reference

### Core Headers

```cpp
#include <algebraic_hashing/core/concepts.hpp>          // Type concepts
#include <algebraic_hashing/core/hash_value.hpp>        // Hash value type
#include <algebraic_hashing/core/hash_function_base.hpp> // Base class
```

### Hash Functions

```cpp
#include <algebraic_hashing/functions/fnv_hash_modern.hpp>  // FNV-1a hash
```

### DSL & Composition

```cpp
#include <algebraic_hashing/dsl/algebraic_operations.hpp>  // Operators
```

## Core Concepts

### `Hashable`

Types that can be hashed:

```cpp
template<typename T>
concept Hashable = /* ... */;
```

**Satisfied by**:
- All arithmetic types (`int`, `float`, etc.)
- `std::string`, `std::string_view`
- Contiguous ranges (`std::vector`, `std::array`, `std::span`)
- Types with `std::ranges::contiguous_range` support

### `HashValue`

Types that represent hash values:

```cpp
template<typename T>
concept HashValue = /* ... */;
```

**Satisfied by**:
- `hash_value<N>` where N is the size in bytes
- Types providing XOR operations and byte access

### `HashFunction`

Types that are hash functions:

```cpp
template<typename F>
concept HashFunction = /* ... */;
```

**Satisfied by**:
- Types providing `operator()(Hashable) -> HashValue`
- Hash function compositions

### `ComposableHashFunction`

Hash functions that can be composed:

```cpp
template<typename F>
concept ComposableHashFunction = HashFunction<F> && /* ... */;
```

## Hash Value Types

### `hash32` / `hash_value<4>`

32-bit hash value (4 bytes):

```cpp
using hash32 = hash_value<4>;

hash32 h;
h.to_uint32();           // Convert to uint32_t
h.data();                // Get std::array<uint8_t, 4>
h.is_zero();             // Check if all zero
h.popcount();            // Count 1 bits
```

### `hash64` / `hash_value<8>`

64-bit hash value (8 bytes):

```cpp
using hash64 = hash_value<8>;

hash64 h;
h.to_uint64();           // Convert to uint64_t
```

### `hash128` / `hash_value<16>`

128-bit hash value (16 bytes):

```cpp
using hash128 = hash_value<16>;
```

### `hash256` / `hash_value<32>`

256-bit hash value (32 bytes):

```cpp
using hash256 = hash_value<32>;
```

## Hash Functions

### `fnv32`

32-bit FNV-1a hash:

```cpp
auto hash = fnv32{};
auto result = hash(data);  // Returns hash32
```

### `fnv64`

64-bit FNV-1a hash:

```cpp
auto hash = fnv64{};
auto result = hash(data);  // Returns hash64
```

**Properties**:
- Fast, non-cryptographic
- Good distribution
- FNV-1a compliant (validated against official test vectors)
- Thread-safe (hash function itself, not statistics)

## Composition Operators

### XOR Composition (`operator^`)

```cpp
auto composed = h1 ^ h2;
```

Combines hash functions by XORing their results.

**Properties**:
- Commutative: `h1 ^ h2 == h2 ^ h1`
- Associative: `(h1 ^ h2) ^ h3 == h1 ^ (h2 ^ h3)`
- Identity exists
- Invertible

### Sequential Composition (`operator*`)

```cpp
auto composed = h1 * h2;
```

Applies hash functions in sequence: `h2(h1(data))`.

**Properties**:
- Non-commutative: `h1 * h2 != h2 * h1` (usually)
- Associative: `(h1 * h2) * h3 == h1 * (h2 * h3)`
- Identity exists (identity hash)

### Complement (`operator~`)

```cpp
auto complemented = ~h;
```

Bitwise complement of hash result.

**Properties**:
- Involutive: `~~h == h`
- Related to original but different distribution

### Repetition (`repeat`)

```cpp
auto repeated = repeat(h, 3);  // Apply h three times
```

Equivalent to `h * h * h`.

### Parallel Composition (`parallel`)

```cpp
auto parallel_hash = parallel(h1, h2);
```

Concatenates hash results.

## Base Class

### `hash_function_base<Derived, HashType>`

CRTP base class for hash functions:

```cpp
template<typename Derived>
class MyHash : public hash_function_base<MyHash, hash64> {
public:
    hash64 hash_impl(const auto& data) const {
        // Your implementation
    }
};
```

**Provides**:
- `operator()` with statistics collection
- `get_statistics()` / `reset_statistics()`
- `name()`, `entropy()`
- `measure_avalanche_effect()`, `measure_distribution_quality()`

## Statistics

### `statistics` struct

```cpp
struct statistics {
    std::size_t calls;
    std::size_t total_input_bytes;
    std::chrono::nanoseconds total_time;

    double average_time_ns() const;
    double throughput_mbps() const;
};
```

!!! warning
    Statistics are **NOT thread-safe**. For multi-threaded use:

    - Use separate hash instances per thread, OR
    - Disable with `#define ALGEBRAIC_HASHING_DISABLE_STATISTICS`

## CMake Integration

### Find Package

```cmake
find_package(algebraic_hashing REQUIRED)
target_link_libraries(your_target PRIVATE AlgebraicHashing::algebraic_hashing)
```

### Compile Options

```cmake
# Disable statistics for multi-threaded production use
target_compile_definitions(your_target PRIVATE
    ALGEBRAIC_HASHING_DISABLE_STATISTICS
)
```

## Namespace Structure

```cpp
namespace algebraic_hashing {
    // Core types
    template<std::size_t N> class hash_value;
    template<typename D, typename H> class hash_function_base;

    namespace concepts {
        template<typename T> concept Hashable;
        template<typename T> concept HashValue;
        template<typename F> concept HashFunction;
        template<typename F> concept ComposableHashFunction;
    }

    namespace functions {
        template<typename HashType> class fnv_hash;
        using fnv32 = fnv_hash<hash32>;
        using fnv64 = fnv_hash<hash64>;
    }

    namespace dsl {
        // Composition operators
        template<typename H1, typename H2> class xor_composition;
        template<typename H1, typename H2> class sequential_composition;
        // ...
    }
}
```

## Examples

See the [Examples section](../examples/use-cases.md) for practical usage patterns.

## Full Documentation

For complete, detailed API documentation including all classes, functions, and implementation details:

**[View Full Doxygen Documentation →](https://queelius.github.io/algebraic_hashing/doxygen/)**

---

Need something specific? [Search the Doxygen docs](https://queelius.github.io/algebraic_hashing/doxygen/) or [ask on GitHub](https://github.com/queelius/algebraic_hashing/discussions).
