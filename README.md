# AlgebraicHashing

[![CI](https://github.com/queelius/algebraic_hashing/workflows/Continuous%20Integration/badge.svg)](https://github.com/queelius/algebraic_hashing/actions/workflows/ci.yml)
[![Conan Center](https://img.shields.io/conan/v/algebraic_hashing?logo=conan)](https://conan.io/center/algebraic_hashing)
[![vcpkg](https://img.shields.io/vcpkg/v/algebraic-hashing?logo=vcpkg)](https://vcpkg.io/en/package/algebraic-hashing)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![codecov](https://codecov.io/gh/queelius/algebraic_hashing/branch/master/graph/badge.svg)](https://codecov.io/gh/queelius/algebraic_hashing)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](https://queelius.github.io/algebraic_hashing/)

> *Modern C++20 library for algebraic hash function composition*

AlgebraicHashing is a **header-only C++20 library** that provides an elegant framework for composing hash functions using mathematical operations. Built with modern C++ concepts and designed for both **educational purposes** and **production use**, it offers zero-cost abstractions for sophisticated hash function algebra.

## ✨ Key Features

### 🎯 **Modern C++20 Design**
- **Concepts-based API** for type safety and clear error messages
- **constexpr evaluation** for compile-time hash computation
- **Zero-cost abstractions** through template metaprogramming
- **Header-only** - just include and use

### 🧮 **Elegant Mathematical Framework**
- **Hash values as algebraic structures** (abelian groups, rings)
- **Intuitive DSL** for function composition: `h1 ^ h2`, `h1 * h2`, `~h1`
- **Mathematical rigor** with provable properties
- **Educational value** with comprehensive examples

### ⚡ **High Performance**
- **Optimized algorithms** with platform-specific code paths
- **Compile-time dispatch** for maximum efficiency  
- **SIMD-friendly** data layouts
- **Minimal memory footprint**

### 🔧 **Production Ready**
- **Comprehensive test coverage** with extensive validation
- **Multi-platform support** (Linux, Windows, macOS)
- **Professional packaging** (Conan, vcpkg, CMake)
- **Extensive documentation** and tutorials

## 🚀 Quick Start

### Installation

#### Using Conan (Recommended)
```bash
# Add to your conanfile.txt
[requires]
algebraic_hashing/2.0.0

# Or install directly  
conan install --requires=algebraic_hashing/2.0.0@
```

#### Using vcpkg
```bash
vcpkg install algebraic-hashing
```

#### Using CMake FetchContent
```cmake
include(FetchContent)
FetchContent_Declare(
    algebraic_hashing
    GIT_REPOSITORY https://github.com/queelius/algebraic_hashing.git
    GIT_TAG v2.0.0
)
FetchContent_MakeAvailable(algebraic_hashing)

target_link_libraries(your_target PRIVATE AlgebraicHashing::algebraic_hashing)
```

### Basic Usage

```cpp
#include <algebraic_hashing/core/hash_value.hpp>
#include <algebraic_hashing/functions/fnv_hash_modern.hpp>
#include <algebraic_hashing/dsl/algebraic_operations.hpp>

using namespace algebraic_hashing;

int main() {
    // Create hash functions
    auto fnv1 = fnv64{};
    auto fnv2 = fnv32{};
    
    // Elegant composition using mathematical operators
    auto composed = fnv1 ^ fnv2;        // XOR composition  
    auto sequential = fnv1 * fnv2;      // Sequential composition
    auto complement = ~fnv1;            // Bitwise complement
    
    // Hash some data
    std::string data = "Hello, AlgebraicHashing!";
    auto result = composed(data);
    
    // Hash values support full algebraic operations
    auto h1 = fnv1(data);
    auto h2 = fnv2(data);
    auto combined = h1 ^ h2;  // XOR of hash values
    
    std::cout << "Hash result: " << result.to_hex() << std::endl;
    return 0;
}
```

## 🧠 Core Concepts

### Hash Values as Mathematical Objects

Hash values in AlgebraicHashing form **abelian groups** under XOR operation:

```cpp
auto h1 = hash_value<64>::from_string("data1");
auto h2 = hash_value<64>::from_string("data2");

// Abelian group properties
auto zero = hash_value<64>::zero();        // Identity element
assert((h1 ^ zero) == h1);                 // Identity property
assert((h1 ^ h1) == zero);                 // Inverse property  
assert((h1 ^ h2) == (h2 ^ h1));           // Commutativity
```

### Sophisticated Function Composition

Create complex hash functions through composition:

```cpp
auto base = fnv64{};

// XOR composition - preserves entropy of stronger component
auto diversified = base ^ (~base);

// Sequential composition - applies functions in sequence  
auto chained = base * repeat<3>(base);

// Complex expressions read naturally
auto robust = (fnv1 ^ fnv2) * (~fnv3) ^ repeat<2>(fnv4);
```

### Type-Safe Concepts

All operations are validated at compile-time using C++20 concepts:

```cpp
template<Hashable T>
void process_data(const T& data) {
    static_assert(ComposableHashFunction<decltype(fnv64{})>);
    static_assert(HashValue<hash_value<64>>);
    
    auto hash = fnv64{}(data);  // Type-safe hashing
}
```

## 📚 Advanced Examples

### Bloom Filter Hash Diversification
```cpp
// Create multiple independent hash functions for Bloom filters
auto h1 = fnv64{};
auto h2 = h1 ^ (~h1);                    // Independent through complement
auto h3 = h1 * fnv32{};                  // Different avalanche pattern
auto h4 = repeat<2>(h1);                 // Hash stretching

std::array hash_functions = {h1, h2, h3, h4};
```

### Cryptographic Hash Chaining
```cpp
// Chain hash functions for increased security
auto sha_like = chain(fnv1, fnv2, fnv3);  // Variadic XOR composition
auto result = sha_like("sensitive data");

// Parallel composition for maximum entropy
auto robust_hash = parallel(fnv1, fnv2, fnv3);  // Concatenated outputs
```

### Performance Analysis
```cpp
auto hash_func = fnv64{};

// Collect performance statistics
hash_func.reset_statistics();
for (const auto& data : large_dataset) {
    auto result = hash_func(data);
}

auto stats = hash_func.get_statistics();
std::cout << "Throughput: " << stats.throughput_mbps() << " MB/s" << std::endl;
std::cout << "Avg time: " << stats.average_time_ns() << " ns" << std::endl;
```

## 🏗️ Architecture

AlgebraicHashing is built on a **layered architecture** that separates concerns:

```
┌─────────────────────────────────────┐
│           DSL Layer                 │  ← Intuitive operators (^, *, ~)
├─────────────────────────────────────┤
│        Composition Layer            │  ← Function composition logic  
├─────────────────────────────────────┤
│         Concepts Layer              │  ← Type safety and validation
├─────────────────────────────────────┤
│        Core Hash Layer              │  ← Hash values and base functions
└─────────────────────────────────────┘
```

### Key Components

- **`core/concepts.hpp`** - C++20 concepts for type safety
- **`core/hash_value.hpp`** - Mathematical hash value type
- **`core/hash_function_base.hpp`** - CRTP base with statistics
- **`functions/fnv_hash_modern.hpp`** - Modern FNV implementation
- **`dsl/algebraic_operations.hpp`** - Composition operators

## 🔬 Testing & Quality

AlgebraicHashing maintains **exceptional quality standards**:

- **Comprehensive test suite** with extensive test cases covering core functionality
- **Multi-platform CI** (Linux, Windows, macOS)
- **Multiple compilers** (GCC 10+, Clang 12+, MSVC 2019+)
- **Sanitizer validation** (AddressSanitizer, UBSan, ThreadSanitizer)
- **Performance benchmarks** and regression testing
- **Static analysis** (clang-tidy, cppcheck)

### Running Tests

```bash
# Build with tests
cmake -B build -DBUILD_TESTING=ON
cmake --build build

# Run test suite
cd build && ctest --output-on-failure

# Run with coverage
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage"
cmake --build build && cd build && ctest
lcov --capture --directory . --output-file coverage.info
```

### Performance Benchmarks

```bash
# Build benchmarks  
cmake -B build -DBUILD_BENCHMARKS=ON
cmake --build build

# Run comprehensive benchmarks
./build/benchmark_comprehensive
```

## 📖 Documentation

### 🎓 **Learning Resources**
- **[Interactive Tutorial](examples/algebraic_hashing_tutorial.cpp)** - Hands-on learning
- **[API Reference](https://queelius.github.io/algebraic_hashing/)** - Complete API docs
- **[Mathematical Foundation](docs/mathematical_foundation.md)** - Theory behind the library
- **[Performance Guide](docs/performance_guide.md)** - Optimization techniques

### 🛠️ **Developer Resources**  
- **[Contributing Guide](CONTRIBUTING.md)** - How to contribute
- **[Package Distribution](PACKAGE_DISTRIBUTION_GUIDE.md)** - Distribution strategy
- **[Architecture Overview](REFACTORING_SUMMARY.md)** - Design decisions

### 📋 **References**
- **[Changelog](CHANGELOG.md)** - Version history
- **[License](LICENSE.md)** - MIT License terms

## 🌟 Use Cases

### **Educational**
- **Computer Science Courses** - Hash function theory and implementation
- **Cryptography Learning** - Understanding composition and entropy
- **Algorithm Design** - Modern C++ design patterns

### **Production**
- **Bloom Filters** - Multiple independent hash functions  
- **Hash Tables** - Custom hash function design
- **Cryptographic Applications** - Hash function diversification
- **Data Structures** - Perfect hash functions and minimal hashing

### **Research**
- **Hash Function Analysis** - Mathematical property exploration
- **Performance Studies** - Benchmarking and optimization research
- **Algorithm Development** - New composition techniques

## ⚙️ Requirements

### **Minimum Requirements**
- **C++20 compatible compiler**:
  - GCC 10+ (recommended: GCC 13)
  - Clang 12+ (recommended: Clang 15)  
  - MSVC 2019+ (Visual Studio 16.0+)
- **CMake 3.20+**

### **Optional Dependencies**
- **Google Test** (for testing) - automatically downloaded if not found
- **Doxygen** (for documentation)
- **Conan 2.0+** (for package management)

## 🤝 Contributing

We welcome contributions! AlgebraicHashing is designed to be:

- **Educational** - Help others learn hash function theory
- **High-quality** - Maintain exceptional code standards  
- **Mathematical** - Preserve theoretical foundations
- **Practical** - Solve real-world problems

See our [Contributing Guide](CONTRIBUTING.md) for detailed information.

### Ways to Contribute
- 🐛 **Bug Reports** - Help us identify and fix issues
- 💡 **Feature Requests** - Suggest new functionality
- 📝 **Documentation** - Improve guides and examples
- 🔬 **Research** - Contribute mathematical insights
- 🧪 **Testing** - Expand test coverage and scenarios

## 📊 Project Status

| Metric | Status |
|--------|--------|
| **Build Status** | [![CI](https://github.com/queelius/algebraic_hashing/workflows/Continuous%20Integration/badge.svg)](https://github.com/queelius/algebraic_hashing/actions) |
| **Test Coverage** | [![codecov](https://codecov.io/gh/queelius/algebraic_hashing/branch/master/graph/badge.svg)](https://codecov.io/gh/queelius/algebraic_hashing) |
| **Code Quality** | ![A+](https://img.shields.io/badge/Grade-A%2B-brightgreen) |
| **Documentation** | ![Complete](https://img.shields.io/badge/Docs-Complete-success) |
| **Package Status** | ![Ready](https://img.shields.io/badge/Package-Ready-success) |

## 📜 License

AlgebraicHashing is released under the [MIT License](LICENSE.md).

```
Copyright (c) 2025 Algebraic Hashing Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
```

## 🙏 Acknowledgments

- **C++ Community** - For advancing the language with concepts and constexpr
- **Hash Function Researchers** - For the mathematical foundations
- **Open Source Projects** - For inspiration and best practices
- **Contributors** - For making this project possible

---

<div align="center">

**🌟 Star this repository if you find it useful! 🌟**

[**📖 Read the Docs**](https://queelius.github.io/algebraic_hashing/) • [**🐛 Report Bug**](https://github.com/queelius/algebraic_hashing/issues) • [**💡 Request Feature**](https://github.com/queelius/algebraic_hashing/issues) • [**💬 Discuss**](https://github.com/queelius/algebraic_hashing/discussions)

</div>