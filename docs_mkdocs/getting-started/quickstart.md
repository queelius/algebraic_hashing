# Quick Start

Get up and running with AlgebraicHashing in 5 minutes!

## Your First Hash

```cpp
#include <algebraic_hashing/functions/fnv_hash_modern.hpp>
#include <iostream>

using namespace algebraic_hashing;

int main() {
    // Create a hash function
    auto hash = fnv64{};

    // Hash different types
    auto h1 = hash("Hello, World!");
    auto h2 = hash(42);
    auto h3 = hash(std::vector{1, 2, 3, 4, 5});

    std::cout << "String hash: " << h1 << "\n";
    std::cout << "Integer hash: " << h2 << "\n";
    std::cout << "Vector hash: " << h3 << "\n";
}
```

## Your First Composition

The real power comes from composition:

```cpp
#include <algebraic_hashing/functions/fnv_hash_modern.hpp>
#include <algebraic_hashing/dsl/algebraic_operations.hpp>

using namespace algebraic_hashing;

int main() {
    // Combine hash functions with operators
    auto strong_hash = fnv64{} ^ fnv32{};  // XOR composition

    // Use it just like a regular hash function
    auto result = strong_hash("My data");

    std::cout << "Combined hash: " << result << "\n";
}
```

## Common Patterns

### Pattern 1: XOR Composition for Strength

```cpp
// Combine multiple hash functions for better distribution
auto robust_hash = fnv64{} ^ fnv32{};
```

**When to use**: When you need better hash distribution than a single function provides.

### Pattern 2: Sequential Composition for Layers

```cpp
// Apply hash functions in sequence
auto layered_hash = fnv64{} * fnv32{};
```

**When to use**: When you want to apply multiple transformations in order.

### Pattern 3: Complement for Variation

```cpp
// Bitwise complement creates a related but different hash
auto variant_hash = ~fnv64{};
```

**When to use**: When you need a variation of a hash function (e.g., for double hashing).

### Pattern 4: Repetition for Strength

```cpp
// Apply the same hash multiple times
auto strong_hash = repeat(fnv64{}, 3);
```

**When to use**: When you want cryptographic-style iteration (though this is NOT a cryptographic hash).

### Pattern 5: Combining It All

```cpp
// Complex composition
auto sophisticated = (fnv64{} ^ fnv32{}) * ~fnv64{};
```

## Complete Example: Hash Table with Custom Hash

```cpp
#include <algebraic_hashing/functions/fnv_hash_modern.hpp>
#include <algebraic_hashing/dsl/algebraic_operations.hpp>
#include <unordered_map>
#include <string>

using namespace algebraic_hashing;

// Define your hash function type
struct User {
    std::string name;
    int id;
};

// Specialize std::hash for your type
namespace std {
    template<>
    struct hash<User> {
        size_t operator()(const User& user) const {
            // Use algebraic composition
            auto hash_func = fnv64{} ^ fnv32{};

            // Combine fields
            auto name_hash = hash_func(user.name);
            auto id_hash = hash_func(user.id);

            return (name_hash ^ id_hash).to_uint64();
        }
    };
}

int main() {
    std::unordered_map<User, std::string> user_data;

    user_data[{"Alice", 1}] = "Engineer";
    user_data[{"Bob", 2}] = "Designer";

    std::cout << "Alice: " << user_data[{"Alice", 1}] << "\n";
    std::cout << "Bob: " << user_data[{"Bob", 2}] << "\n";
}
```

## Working with Hash Values

Hash values are first-class objects with algebraic operations:

```cpp
auto h1 = fnv64{}("data1");
auto h2 = fnv64{}("data2");

// Algebraic operations on hash values
auto xor_result = h1 ^ h2;      // XOR
auto complement = ~h1;           // Bitwise NOT
bool equal = (h1 == h2);         // Equality

// Convert to standard types
uint64_t as_uint = h1.to_uint64();
std::array<uint8_t, 8> as_bytes = h1.data();

// Check properties
bool is_zero = h1.is_zero();
int bit_count = h1.popcount();  // Number of 1 bits
```

## Type Safety with Concepts

One of the key benefits is compile-time type safety:

```cpp
template<Hashable T>
auto safe_hash(const T& data) {
    return fnv64{}(data);
}

// This compiles - std::string is Hashable
safe_hash(std::string("Hello"));

// This compiles - int is Hashable
safe_hash(42);

// This won't compile - NonHashable doesn't satisfy Hashable
// safe_hash(NonHashable{});
```

## Performance Considerations

AlgebraicHashing uses zero-cost abstractions:

```cpp
// All composition happens at compile-time
auto composed = fnv64{} ^ fnv32{};

// At runtime, this is as fast as hand-written code
auto result = composed(data);  // Single function call
```

To verify performance, use the built-in statistics (single-threaded only):

```cpp
auto hash = fnv64{};

// Hash lots of data
for (int i = 0; i < 1000000; ++i) {
    hash(std::to_string(i));
}

// Check statistics
auto stats = hash.get_statistics();
std::cout << "Calls: " << stats.calls << "\n";
std::cout << "Avg time: " << stats.average_time_ns() << " ns\n";
std::cout << "Throughput: " << stats.throughput_mbps() << " MB/s\n";
```

!!! warning "Statistics Not Thread-Safe"
    Statistics collection is NOT thread-safe. For multi-threaded applications, either:

    1. Use separate hash instances per thread
    2. Disable statistics: `#define ALGEBRAIC_HASHING_DISABLE_STATISTICS`

## Next Steps

Now that you've seen the basics, dive deeper:

- [Core Concepts](../concepts/core-concepts.md) - Understand the algebraic foundations
- [Composition Tutorial](../tutorials/composition.md) - Master all composition operators
- [Examples](../examples/use-cases.md) - See real-world use cases
- [API Reference](../reference/api.md) - Complete API documentation

## Common Gotchas

### 1. Don't forget `using namespace`

```cpp
// Without namespace
algebraic_hashing::functions::fnv_hash<algebraic_hashing::hash64> hash;

// Much better
using namespace algebraic_hashing;
auto hash = fnv64{};
```

### 2. Hash values need explicit conversion

```cpp
auto hash_val = fnv64{}("data");

// This won't compile
// uint64_t x = hash_val;

// Do this instead
uint64_t x = hash_val.to_uint64();
```

### 3. Composition creates new types

```cpp
auto h1 = fnv64{};
auto h2 = fnv32{};
auto composed = h1 ^ h2;

// composed is a different type than h1 or h2
static_assert(!std::same_as<decltype(composed), decltype(h1)>);
```

This is intentional - each composition creates a unique type with its own properties.

---

Ready to learn more? [Explore Core Concepts →](../concepts/core-concepts.md)
