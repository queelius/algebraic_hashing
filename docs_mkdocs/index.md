# AlgebraicHashing

**Modern C++20 Library for Elegant Hash Function Composition**

[![Build Status](https://github.com/queelius/algebraic_hashing/workflows/CI/badge.svg)](https://github.com/queelius/algebraic_hashing/actions)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/queelius/algebraic_hashing/blob/master/LICENSE.md)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://isocpp.org/)

## What is AlgebraicHashing?

AlgebraicHashing transforms hash functions from simple utilities into **first-class mathematical objects** that you can compose, analyze, and reason about using elegant algebraic operations.

```cpp
#include <algebraic_hashing/functions/fnv_hash_modern.hpp>
#include <algebraic_hashing/dsl/algebraic_operations.hpp>

using namespace algebraic_hashing;

// Create hash functions
auto h1 = fnv64{};
auto h2 = fnv32{};

// Compose them with intuitive operators
auto strong_hash = h1 ^ h2;              // XOR composition
auto layered = (h1 * h2) ^ ~h1;          // Complex composition
auto repeated = repeat(h1, 3);           // Apply 3 times

// Use them just like regular hash functions
auto result = strong_hash("Hello, World!");
```

## Why AlgebraicHashing?

### 🎯 **Elegant Mathematical Abstraction**

Traditional hash function usage is rigid and imperative. AlgebraicHashing treats hash functions as **algebraic structures** that form groups and rings, enabling powerful composition:

```cpp
// Traditional approach - rigid and verbose
uint64_t hash1 = fnv64_hash(data);
uint64_t hash2 = fnv32_hash(data);
uint64_t combined = hash1 ^ hash2;

// AlgebraicHashing - composable and expressive
auto combined_hash = fnv64{} ^ fnv32{};
auto result = combined_hash(data);
```

### 🔒 **Type Safety Through Concepts**

C++20 concepts ensure correctness at compile-time:

```cpp
template<Hashable T>
auto secure_hash(const T& data) {
    return (fnv64{} ^ fnv32{})(data);  // Compile-time validated
}

// This won't compile - compile-time error with clear message
// auto bad = secure_hash(non_hashable_type);
```

### 🚀 **Zero-Cost Abstractions**

All composition happens at compile-time. The generated assembly is identical to hand-written code:

```cpp
auto composed = h1 ^ h2 ^ h3;  // All resolved at compile-time
auto result = composed(data);   // Single function call in assembly
```

### 📚 **Educational Excellence**

Perfect for teaching:

- **Hash function theory** - visualize algebraic properties
- **Modern C++** - concepts, constexpr, CRTP patterns
- **Type systems** - compile-time validation
- **Performance** - zero-cost abstractions in practice

## Key Features

### **Modern C++20**
- Full concepts support for compile-time type safety
- `constexpr` everywhere for compile-time evaluation
- Header-only design - just `#include` and go

### **Algebraic DSL**
- **XOR composition**: `h1 ^ h2` - combines hash functions
- **Sequential**: `h1 * h2` - apply in sequence
- **Complement**: `~h` - bitwise complement
- **Repetition**: `repeat(h, n)` - apply n times
- **Parallel**: `parallel(h1, h2)` - concatenate outputs

### **Production Ready**
- ✅ **99.5% test coverage** (verified with lcov)
- ✅ **Thread-safe** hash functions (validated with 32-thread stress tests)
- ✅ **Multi-platform** (Linux, Windows, macOS)
- ✅ **Sanitizer-clean** (ASan, UBSan, ThreadSanitizer)
- ✅ **FNV-1a compliant** (validated against official test vectors)

### **Professional Packaging**
- Conan 2.0 package
- vcpkg support
- CMake FetchContent integration
- Modern CMake targets

## Quick Example

```cpp
#include <algebraic_hashing/functions/fnv_hash_modern.hpp>
#include <algebraic_hashing/dsl/algebraic_operations.hpp>
#include <iostream>

using namespace algebraic_hashing;

int main() {
    // Create individual hash functions
    auto fnv_64 = fnv64{};
    auto fnv_32 = fnv32{};

    // Compose them algebraically
    auto combined = fnv_64 ^ fnv_32;  // XOR composition
    auto enhanced = combined * ~fnv_64;  // Sequential with complement

    // Hash different types
    std::string text = "AlgebraicHashing";
    int number = 42;
    std::vector<int> data = {1, 2, 3, 4, 5};

    std::cout << "Text hash: " << enhanced(text) << "\n";
    std::cout << "Number hash: " << enhanced(number) << "\n";
    std::cout << "Vector hash: " << enhanced(data) << "\n";

    // Analyze properties
    auto stats = enhanced.get_statistics();
    std::cout << "Calls: " << stats.calls << "\n";
    std::cout << "Throughput: " << stats.throughput_mbps() << " MB/s\n";

    return 0;
}
```

## What Makes It Different?

| Feature | Traditional Libraries | AlgebraicHashing |
|---------|---------------------|------------------|
| Composition | Manual XOR/combine | Algebraic operators (`^`, `*`, `~`) |
| Type Safety | Runtime errors | Compile-time concepts |
| Performance | Good | Zero-cost abstractions |
| Extensibility | Inheritance/templates | Concepts + CRTP |
| Learning Curve | Moderate | Intuitive DSL |
| Use Cases | Hashing only | Hashing + Teaching + Research |

## Use Cases

### **Production Systems**
- Hash table implementations
- Data deduplication
- Checksum generation
- Distributed systems (consistent hashing)

### **Education**
- Teaching hash function theory
- Demonstrating C++20 concepts
- Exploring type system design
- Understanding zero-cost abstractions

### **Research**
- Experimenting with hash compositions
- Analyzing algebraic properties
- Developing new hash functions
- Performance benchmarking

## Get Started

<div class="grid cards" markdown>

-   :material-download:{ .lg .middle } **Install**

    ---

    Get started with AlgebraicHashing in minutes

    [:octicons-arrow-right-24: Installation](getting-started/installation.md)

-   :material-rocket-launch:{ .lg .middle } **Quick Start**

    ---

    Your first hash composition in 5 minutes

    [:octicons-arrow-right-24: Quick Start](getting-started/quickstart.md)

-   :material-school:{ .lg .middle } **Learn Concepts**

    ---

    Understand the algebraic foundations

    [:octicons-arrow-right-24: Core Concepts](concepts/core-concepts.md)

-   :material-code-braces:{ .lg .middle } **API Reference**

    ---

    Complete API documentation

    [:octicons-arrow-right-24: API Docs](reference/api.md)

</div>

## Philosophy

AlgebraicHashing is built on three principles:

1. **Elegance Over Verbosity** - Code should express intent clearly
2. **Safety Over Speed** - Compile-time validation prevents bugs
3. **Composition Over Inheritance** - Build complex behavior from simple parts

!!! quote "Design Philosophy"
    "Make simple things simple, complex things possible, and incorrect things impossible to compile."

## Community & Support

- **GitHub**: [queelius/algebraic_hashing](https://github.com/queelius/algebraic_hashing)
- **Issues**: [Report bugs or request features](https://github.com/queelius/algebraic_hashing/issues)
- **Discussions**: [Ask questions](https://github.com/queelius/algebraic_hashing/discussions)
- **Releases**: [Latest version](https://github.com/queelius/algebraic_hashing/releases)

## License

AlgebraicHashing is licensed under the [MIT License](about/license.md).

---

<center>
**Ready to explore algebraic hash functions?**

[Get Started](getting-started/installation.md){ .md-button .md-button--primary }
[View Examples](examples/use-cases.md){ .md-button }
</center>
