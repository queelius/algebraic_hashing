# Composition Operators Tutorial

This tutorial explores the algebraic composition operators that make AlgebraicHashing unique.

## The Three Operators

AlgebraicHashing provides three composition operators:

| Operator | Name | Effect |
|----------|------|--------|
| `^` | XOR composition | `(f ^ g)(x) = f(x) ^ g(x)` |
| `*` | Sequential composition | `(f * g)(x) = f(g(x))` |
| `~` | Complement | `(~f)(x) = ~f(x)` |

## XOR Composition (`^`)

XOR composition creates a new hash function that XORs the outputs of two functions:

```cpp
#include <algebraic_hashing/functions/fnv_hash_modern.hpp>
#include <algebraic_hashing/functions/murmur3_hash.hpp>
#include <algebraic_hashing/dsl/algebraic_operations.hpp>

using namespace algebraic_hashing;
using namespace algebraic_hashing::functions;
using namespace algebraic_hashing::dsl;

fnv64 f;
murmur3_64 g;

// Create XOR composition
auto h = f ^ g;

// Use it like any hash function
auto result = h("hello");

// Equivalent to:
auto manual = f("hello") ^ g("hello");
assert(result == manual);
```

### Algebraic Properties

XOR composition forms an **abelian group**:

```cpp
fnv64 f;
murmur3_64 g;
xxhash64 k;

// Commutative: f ^ g = g ^ f
auto fg = f ^ g;
auto gf = g ^ f;
assert(fg("test") == gf("test"));

// Associative: (f ^ g) ^ k = f ^ (g ^ k)
auto fg_k = (f ^ g) ^ k;
auto f_gk = f ^ (g ^ k);
assert(fg_k("test") == f_gk("test"));

// Self-inverse: f ^ f = zero
auto ff = f ^ f;
assert(ff("test").is_zero());  // Always produces zero hash
```

### Use Cases

**Combining independent hash functions:**

```cpp
// If f and g are independent, their XOR has lower collision probability
auto strong = fnv64{} ^ murmur3_64{} ^ xxhash64{};
```

**Creating derived hash functions:**

```cpp
fnv64 base;
murmur3_64 modifier;

auto derived1 = base ^ modifier;
auto derived2 = base ^ ~modifier;  // Different from derived1
```

## Sequential Composition (`*`)

Sequential composition chains hash functions:

```cpp
fnv64 f;
murmur3_64 g;

// Create sequential composition: f(g(x))
auto h = f * g;

// Use it
auto result = h("hello");

// Equivalent to:
auto intermediate = g("hello");
auto manual = f(intermediate);
assert(result == manual);
```

### Properties

Sequential composition forms a **monoid**:

```cpp
fnv64 f;
murmur3_64 g;
xxhash64 k;

// Associative: (f * g) * k = f * (g * k)
auto fg_k = (f * g) * k;
auto f_gk = f * (g * k);
// Both produce the same composed function

// NOT commutative: f * g ≠ g * f (usually)
auto fg = f * g;  // f(g(x))
auto gf = g * f;  // g(f(x))
// These are different functions!
```

### Use Cases

**Hash stretching:**

```cpp
// Apply hash multiple times
auto stretched = f * f * f;  // f(f(f(x)))

// Or use the repeat helper
auto repeated = repeat<10>(fnv64{});
```

**Pipeline processing:**

```cpp
// Process through multiple stages
murmur3_64 preprocess;  // Initial mixing
fnv64 finalize;          // Final hash

auto pipeline = finalize * preprocess;
```

## Complement (`~`)

The complement operator flips all bits in the hash output:

```cpp
fnv64 f;

// Create complemented function
auto neg_f = ~f;

// Result has all bits flipped
auto h = f("hello");
auto neg_h = neg_f("hello");
assert(h == ~~neg_h);  // Double complement = identity
```

### Properties

Complement is an **involution**:

```cpp
fnv64 f;

// ~~f = f
auto neg_neg_f = ~~f;
assert(neg_neg_f("test") == f("test"));

// Preserves entropy
// ~f has the same distribution quality as f
```

### Use Cases

**Creating orthogonal hash functions:**

```cpp
fnv64 f;
auto f1 = f;
auto f2 = ~f;
auto f3 = f ^ murmur3_64{};
auto f4 = ~(f ^ murmur3_64{});

// All four are distinct hash functions
```

## Combining Operators

The real power comes from combining operators:

```cpp
fnv64 f;
murmur3_64 g;
xxhash64 k;

// Complex compositions
auto h1 = (f ^ g) * k;           // XOR f and g, then apply k
auto h2 = f * (g ^ k);           // Apply f to XOR of g and k
auto h3 = ~(f ^ g);              // Complement of XOR
auto h4 = (f * g) ^ (g * f);     // Symmetric composition
```

### Operator Precedence

Operators follow C++ precedence:

1. `~` (highest - unary)
2. `*` (multiplication)
3. `^` (XOR - lowest)

```cpp
// This:
auto h = f ^ g * k;

// Is parsed as:
auto h = f ^ (g * k);

// Use parentheses for clarity:
auto h = (f ^ g) * k;  // XOR first, then sequential
```

## The `chain` Helper

For chaining multiple XOR compositions:

```cpp
// Instead of:
auto h = f ^ g ^ k ^ m ^ n;

// Use chain:
auto h = chain(f, g, k, m, n);
```

## The `repeat` Helper

For repeated application:

```cpp
// Apply f 100 times
auto stretched = repeat<100>(fnv64{});

// Compile-time count ensures type safety
```

## Introspection

Composed functions provide access to their components:

```cpp
auto composed = f ^ g;

// Access components
const auto& first = composed.first();   // f
const auto& second = composed.second(); // g

// For sequential composition
auto seq = f * g;
const auto& outer = seq.outer();  // f
const auto& inner = seq.inner();  // g
```

## Entropy Analysis

Composed functions provide entropy estimates:

```cpp
fnv64 f;
murmur3_64 g;

auto composed = f ^ g;
double entropy = composed.entropy();

// Entropy is bounded by min(entropy(f), entropy(g))
// for XOR composition
```

## Best Practices

### Do: Use XOR for Independence

```cpp
// Good: combine different algorithms
auto robust = fnv64{} ^ murmur3_64{};
```

### Don't: XOR Same Function

```cpp
// Bad: always produces zero!
auto useless = fnv64{} ^ fnv64{};  // h(x) = f(x) ^ f(x) = 0
```

### Do: Consider Order for Sequential

```cpp
// These are different:
auto prep_then_hash = fnv64{} * murmur3_64{};
auto hash_then_prep = murmur3_64{} * fnv64{};
```

### Don't: Over-Compose

```cpp
// Probably overkill:
auto paranoid = f ^ g ^ k ^ m ^ n ^ p ^ q ^ r;

// Usually sufficient:
auto good = fnv64{} ^ murmur3_64{};
```

## Next Steps

- [Type Safety](type-safety.md) - Concepts and compile-time checks
- [Performance](performance.md) - Benchmark your compositions
- [Algebraic Structure](../concepts/algebraic-structure.md) - Mathematical foundations
