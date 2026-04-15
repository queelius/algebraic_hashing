# Basic Usage Tutorial

This tutorial covers the fundamentals of using AlgebraicHashing for common hashing tasks.

## Including the Library

```cpp
// Include everything
#include <algebraic_hashing/algebraic_hashing.hpp>

// Or include specific components
#include <algebraic_hashing/functions/fnv_hash_modern.hpp>
#include <algebraic_hashing/functions/murmur3_hash.hpp>
#include <algebraic_hashing/functions/xxhash.hpp>
#include <algebraic_hashing/functions/sha256_hash.hpp>

using namespace algebraic_hashing;
using namespace algebraic_hashing::functions;
```

## Creating Hash Functions

### FNV-1a (Fast, Good Quality)

```cpp
// 64-bit FNV-1a (most common)
fnv64 hasher64;
auto h = hasher64("hello");
std::cout << h.to_hex() << "\n";

// 32-bit for smaller footprint
fnv32 hasher32;
auto h32 = hasher32("hello");
```

### MurmurHash3 (Excellent Distribution)

```cpp
// 64-bit MurmurHash3
murmur3_64 murmur;
auto h = murmur("hello");

// With custom seed
murmur3_64 seeded(42);
auto h2 = seeded("hello");

// 128-bit variant
murmur3_128 murmur128;
auto h128 = murmur128("hello");
```

### xxHash (Maximum Speed)

```cpp
// 64-bit xxHash (recommended)
xxhash64 xx;
auto h = xx("hello");

// 32-bit variant
xxhash32 xx32;
auto h32 = xx32("hello");

// With seed
xxhash64 seeded(12345);
```

### SHA-256 (Cryptographic)

```cpp
sha256 hasher;
auto h = hasher("hello");
std::cout << h.to_hex() << "\n";

// Streaming interface for large data
sha256 streaming;
streaming.update(data1, len1);
streaming.update(data2, len2);
auto result = streaming.finalize();
```

## Hashing Different Types

### Strings

```cpp
fnv64 h;

// String literals
auto h1 = h("hello");

// std::string
std::string s = "world";
auto h2 = h(s);

// std::string_view
std::string_view sv = "test";
auto h3 = h(sv);
```

### Numbers

```cpp
fnv64 h;

// Integers
auto h1 = h(42);
auto h2 = h(-17);
auto h3 = h(0xDEADBEEF);

// Floating point
auto h4 = h(3.14159);
auto h5 = h(2.71828);
```

### Containers

```cpp
fnv64 h;

// std::vector
std::vector<int> v = {1, 2, 3, 4, 5};
auto h1 = h(v);

// std::array
std::array<char, 4> a = {'t', 'e', 's', 't'};
auto h2 = h(a);

// Raw bytes
std::vector<std::byte> bytes = {std::byte{0x01}, std::byte{0x02}};
auto h3 = h(bytes);
```

## Working with Hash Values

### Comparison

```cpp
fnv64 h;
auto h1 = h("hello");
auto h2 = h("hello");
auto h3 = h("world");

if (h1 == h2) {
    std::cout << "Same input produces same hash\n";
}

if (h1 != h3) {
    std::cout << "Different inputs produce different hashes\n";
}

// Ordering (for use in std::map, etc.)
if (h1 < h3) {
    std::cout << "h1 comes before h3\n";
}
```

### Display

```cpp
hash64 h = fnv64{}("hello");

// Hexadecimal (most common)
std::cout << h.to_hex() << "\n";

// Binary
std::cout << h.to_binary() << "\n";

// Individual bytes
for (auto byte : h) {
    std::cout << std::hex << static_cast<int>(byte) << " ";
}
```

### Algebraic Operations

```cpp
fnv64 h;
auto h1 = h("hello");
auto h2 = h("world");

// XOR (group operation)
auto combined = h1 ^ h2;

// Self-inverse property
auto zero = h1 ^ h1;
assert(zero.is_zero());

// Complement
auto flipped = ~h1;
assert(~~h1 == h1);  // Involution

// AND, OR for masking
auto masked = h1 & hash64::ones();
```

### Analysis

```cpp
hash64 h = fnv64{}("hello");

// Count set bits (Hamming weight)
std::size_t ones = h.popcount();
std::cout << "Set bits: " << ones << "/" << (8 * sizeof(hash64)) << "\n";

// Check for zero
if (h.is_zero()) {
    std::cout << "Hash is all zeros\n";
}
```

## Using in Standard Containers

```cpp
#include <unordered_map>
#include <unordered_set>

// hash_value has std::hash specialization
std::unordered_set<hash64> seen_hashes;

fnv64 h;
seen_hashes.insert(h("hello"));
seen_hashes.insert(h("world"));

if (seen_hashes.contains(h("hello"))) {
    std::cout << "Already seen!\n";
}

// As map key
std::unordered_map<hash64, std::string> hash_to_string;
hash_to_string[h("hello")] = "hello";
```

## Performance Tips

### Reuse Hash Function Objects

```cpp
// Good: reuse the hasher
fnv64 h;
for (const auto& item : items) {
    hashes.push_back(h(item));
}

// Less optimal: creates new hasher each time
for (const auto& item : items) {
    hashes.push_back(fnv64{}(item));
}
```

### Choose the Right Hash Function

| Use Case | Recommended |
|----------|-------------|
| Hash tables | `xxhash64` or `murmur3_64` |
| Bloom filters | `murmur3_64` |
| Checksums | `xxhash64` |
| File hashing | `sha256` |
| Quick prototype | `fnv64` |

## Next Steps

- [Composition Operators](composition.md) - Combine hash functions
- [Type Safety](type-safety.md) - Work with concepts
- [Performance](performance.md) - Optimization techniques
