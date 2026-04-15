# Core Concepts

AlgebraicHashing is built on a foundation of well-defined abstractions that make hash function composition both type-safe and mathematically rigorous. This page explains the key concepts that power the library.

## The Type Hierarchy

```
Hashable           <- Any type that can be hashed
    ↓
HashValue          <- The output of a hash function
    ↓
HashFunction       <- A callable that maps Hashable → HashValue
    ↓
ComposableHashFunction  <- Can be combined algebraically
```

### Hashable

A type is `Hashable` if it can be converted to a sequence of bytes for processing. The library supports:

- **Fundamental types**: integers, floating-point numbers
- **String types**: `std::string`, `std::string_view`, C-strings
- **Containers**: `std::vector`, `std::array`, and other contiguous ranges
- **Custom types**: Anything with byte representation

```cpp
// All of these are Hashable
fnv64 hasher;
hasher(42);                      // integers
hasher(3.14159);                 // floating-point
hasher("hello");                 // string literals
hasher(std::string("world"));    // std::string
hasher(std::vector<int>{1,2,3}); // containers
```

### HashValue

A `HashValue` is the output of a hash function. It must satisfy:

1. **Regular type**: copyable, movable, equality comparable
2. **XOR operation**: `h1 ^ h2` returns another HashValue
3. **Complement**: `~h` returns the bitwise complement
4. **Hex conversion**: `h.to_hex()` for display

```cpp
hash64 h1 = fnv64{}("hello");
hash64 h2 = fnv64{}("world");

auto combined = h1 ^ h2;    // XOR composition
auto flipped = ~h1;         // bitwise complement
std::cout << h1.to_hex();   // display as hex string
```

The library provides these hash value types:

| Type | Size | Use Case |
|------|------|----------|
| `hash32` | 32 bits | Memory-constrained, hash tables |
| `hash64` | 64 bits | General purpose (recommended) |
| `hash128` | 128 bits | Higher collision resistance |
| `hash256` | 256 bits | Cryptographic applications |

### HashFunction

A `HashFunction` is any callable that transforms hashable inputs into hash values. The library provides several implementations:

| Function | Speed | Quality | Use Case |
|----------|-------|---------|----------|
| `fnv64` | Fast | Good | General purpose |
| `xxhash64` | Very Fast | Excellent | Performance-critical |
| `murmur3_64` | Fast | Excellent | Hash tables, Bloom filters |
| `sha256` | Slow | Cryptographic | Security applications |

### ComposableHashFunction

A `ComposableHashFunction` extends `HashFunction` with:

1. **Type alias**: `hash_type` exposing the output type
2. **Default constructible**: For algebraic identity elements
3. **HashValue output**: The result satisfies the `HashValue` concept

This enables the algebraic composition operators to work:

```cpp
// All of these are ComposableHashFunction
fnv64 f1;
murmur3_64 f2;
xxhash64 f3;

// Composition operators work on ComposableHashFunction
auto composed = f1 ^ f2;      // XOR composition
auto chained = f1 * f2;       // Sequential composition
auto complement = ~f1;        // Complemented hash
```

## The Composition DSL

The library provides three composition operators:

### XOR Composition (`^`)

Creates a new hash function that XORs the outputs:

```cpp
auto h = f1 ^ f2;
// h(x) computes f1(x) ^ f2(x)
```

**Properties:**

- Commutative: `f1 ^ f2 = f2 ^ f1`
- Associative: `(f1 ^ f2) ^ f3 = f1 ^ (f2 ^ f3)`
- Self-inverse: `f ^ f = zero_hash`

### Sequential Composition (`*`)

Creates a new hash function by chaining:

```cpp
auto h = f1 * f2;
// h(x) computes f1(f2(x))
```

**Properties:**

- Associative: `(f1 * f2) * f3 = f1 * (f2 * f3)`
- NOT commutative: `f1 * f2 ≠ f2 * f1` (in general)

### Complement (`~`)

Creates a hash function with bitwise-complemented output:

```cpp
auto h = ~f1;
// h(x) computes ~f1(x)
```

**Properties:**

- Involution: `~~f = f`
- Preserves entropy: same distribution as original

## Statistics and Instrumentation

Each hash function tracks usage statistics through the CRTP base class:

```cpp
fnv64 hasher;

// Hash some data
for (int i = 0; i < 1000; ++i) {
    hasher(i);
}

// Get statistics
auto stats = hasher.get_stats();
std::cout << "Calls: " << stats.call_count << "\n";
std::cout << "Bytes: " << stats.bytes_hashed << "\n";
```

!!! warning "Thread Safety"
    Statistics collection is NOT thread-safe. For multi-threaded applications, either:

    - Use separate hasher instances per thread
    - Compile with `-DALGEBRAIC_HASHING_DISABLE_STATISTICS`

## Next Steps

- [Algebraic Structure](algebraic-structure.md) - The mathematical foundations
- [Design Philosophy](philosophy.md) - Why we made these choices
- [Tutorials](../tutorials/basic-usage.md) - Hands-on examples
