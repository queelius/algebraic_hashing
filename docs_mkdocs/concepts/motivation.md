# Why AlgebraicHashing?

## The Problem with Traditional Hash Functions

Traditional hash function libraries treat hashing as a **utility** - a means to an end. You call a function, get a number back, and move on. While functional, this approach has several limitations:

### 1. **Limited Composability**

Want to combine two hash functions for extra strength? You're writing boilerplate:

```cpp
// Traditional approach
uint64_t hash_combine(const std::string& data) {
    uint64_t h1 = fnv1a_64(data);
    uint32_t h2 = fnv1a_32(data);
    return h1 ^ static_cast<uint64_t>(h2);
}
```

Every composition requires a new function. Need a different combination? Write another function. Need to experiment with various compositions? Prepare for lots of boilerplate.

### 2. **No Type Safety**

```cpp
// Easy to make mistakes
uint64_t h1 = some_hash(data1);
uint32_t h2 = other_hash(data2);
auto combined = h1 ^ h2;  // Implicit conversion - is this what you wanted?
```

The type system doesn't help you. Hash values are just integers, and the compiler can't tell if you're combining them correctly.

### 3. **Rigid Implementation**

```cpp
// Want to experiment with different hash combinations?
// Need to modify and recompile each time
#define USE_STRONG_HASH 1

#if USE_STRONG_HASH
    return fnv64(data) ^ murmur(data);
#else
    return fnv64(data);
#endif
```

Changing your hashing strategy means editing code and recompiling. There's no way to build hash functions dynamically from smaller pieces.

### 4. **Educational Limitations**

When teaching hash functions, you want to demonstrate:

- How different hash functions behave
- How composition affects distribution
- The algebraic properties of hashing

But with traditional libraries, students are stuck with opaque function calls. The mathematical beauty is hidden behind imperative code.

## The AlgebraicHashing Solution

AlgebraicHashing treats hash functions as **first-class algebraic objects** that you can compose, transform, and reason about mathematically.

### 1. **Elegant Composition**

```cpp
using namespace algebraic_hashing;

// Define once, use anywhere
auto strong_hash = fnv64{} ^ fnv32{};

// Compose further
auto extra_strong = strong_hash * ~fnv64{};

// Use just like any hash function
auto result = extra_strong(data);
```

Hash functions become building blocks. Composition is natural and expressive.

### 2. **Compile-Time Type Safety**

```cpp
template<Hashable T>
auto secure_hash(const T& data) {
    auto h = fnv64{} ^ fnv32{};
    return h(data);  // Type-checked at compile-time
}

// This won't compile - NonHashable doesn't satisfy Hashable concept
// secure_hash(NonHashable{});
```

C++20 concepts ensure correctness. If it compiles, it's type-safe.

### 3. **Flexible Architecture**

```cpp
// Build hash functions dynamically
auto make_hash(int strength) {
    if (strength == 1) return fnv64{};
    if (strength == 2) return fnv64{} ^ fnv32{};
    return repeat(fnv64{} ^ fnv32{}, strength);
}

// All resolved at compile-time with template instantiation
auto hash = make_hash(3);
```

Change your hashing strategy without recompiling. Experiment freely.

### 4. **Educational Value**

```cpp
// Demonstrate hash properties visually
auto h1 = fnv64{};
auto h2 = ~h1;  // Complement

// Show that XOR is commutative
static_assert(std::same_as<decltype(h1 ^ h2), decltype(h2 ^ h1)>);

// Analyze behavior
std::cout << "Avalanche effect: " << h1.measure_avalanche_effect(data) << "\n";
std::cout << "Distribution quality: " << h1.measure_distribution_quality(dataset) << "\n";
```

Students can **see** and **experiment** with hash function properties.

## The Mathematical Foundation

### Hash Functions as Algebraic Structures

In mathematics, hash functions have beautiful properties:

**Group Structure (under XOR)**:
- **Identity**: `h ^ zero_hash = h`
- **Inverse**: `h ^ h = zero_hash`
- **Associativity**: `(h1 ^ h2) ^ h3 = h1 ^ (h2 ^ h3)`
- **Commutativity**: `h1 ^ h2 = h2 ^ h1`

AlgebraicHashing makes these properties **executable code**:

```cpp
auto h1 = fnv64{};
auto h2 = fnv32{};
auto h3 = fnv64{};

// Mathematical properties become code
auto identity = h1 ^ hash_value<8>::zero();  // Returns h1
auto inverse = h1 ^ h1;  // Returns zero hash
auto associative = (h1 ^ h2) ^ h3;  // Same as h1 ^ (h2 ^ h3)
```

### Why This Matters

1. **Correctness**: Mathematical laws guarantee correct behavior
2. **Composability**: Algebraic properties enable safe composition
3. **Reasoning**: You can reason about combined hash functions mathematically
4. **Optimization**: Compiler can use algebraic laws for optimization

## Real-World Impact

### For Application Developers

```cpp
// Before: Scattered hashing logic
class UserCache {
    uint64_t compute_key(const User& user) {
        auto h1 = std::hash<std::string>{}(user.name);
        auto h2 = std::hash<int>{}(user.id);
        return h1 ^ (h2 << 32);  // Hope this is good enough
    }
};

// After: Clear, type-safe, composable
class UserCache {
    static constexpr auto key_hash = fnv64{} ^ fnv32{};

    auto compute_key(const User& user) {
        return key_hash(user);  // Type-safe, tested, validated
    }
};
```

### For Researchers

```cpp
// Experiment with novel compositions
auto experiment_1 = fnv64{} ^ murmur3{};
auto experiment_2 = repeat(fnv64{}, 3) ^ murmur3{};
auto experiment_3 = parallel(fnv64{}, fnv32{});

// Benchmark them
benchmark_distribution(experiment_1, dataset);
benchmark_distribution(experiment_2, dataset);
benchmark_distribution(experiment_3, dataset);
```

### For Educators

```cpp
// Show students hash function internals
class StudentFriendlyHash : public hash_function_base<StudentFriendlyHash, hash64> {
    hash64 hash_impl(const auto& data) const {
        // Step-by-step, visible implementation
        hash64 result = initial_value;
        for (auto byte : get_bytes(data)) {
            result ^= byte;    // XOR step
            result *= prime;   // Multiplication step
        }
        return result;
    }
};

// Students can modify, experiment, learn
auto student_hash = StudentFriendlyHash{};
auto composition = student_hash ^ fnv64{};
```

## When to Use AlgebraicHashing

### ✅ **Perfect For**

- Applications needing flexible hash composition
- Educational settings teaching hash functions or modern C++
- Research into hash function properties
- Systems requiring type-safe hashing
- Projects valuing expressive, maintainable code

### 🤔 **Consider Alternatives If**

- You need a simple, standard hash (just use `std::hash`)
- You're on a C++17 or older compiler (needs C++20)
- You need cryptographic hashes (use a crypto library like OpenSSL)
- You want the absolute minimum binary size (header-only = more code in binary)

## The Vision

AlgebraicHashing demonstrates that **infrastructure libraries can be both practical and beautiful**. Hash functions don't have to be opaque utilities - they can be:

- **Mathematical objects** you can reason about
- **Composable components** you can build with
- **Educational tools** you can learn from
- **Research platforms** you can experiment on

All while maintaining **zero-cost abstractions** and **production-ready quality**.

---

Ready to explore the concepts further?

[Core Concepts →](core-concepts.md){ .md-button }
[See Examples →](../examples/use-cases.md){ .md-button }
