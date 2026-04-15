# Performance & Statistics Tutorial

This tutorial covers performance optimization, benchmarking, and using the built-in statistics system.

## Hash Function Performance Comparison

Different hash functions have different performance characteristics:

| Function | Speed | Quality | Best For |
|----------|-------|---------|----------|
| `xxhash64` | Fastest | Excellent | Performance-critical code |
| `murmur3_64` | Very Fast | Excellent | Hash tables, Bloom filters |
| `fnv64` | Fast | Good | General purpose, simple code |
| `sha256` | Slow | Cryptographic | Security applications |

### Approximate Throughput (x86-64)

| Function | GB/s (large data) | M ops/s (small strings) |
|----------|-------------------|-------------------------|
| `xxhash64` | 10-15 | 100-200 |
| `murmur3_128` | 5-8 | 50-100 |
| `fnv64` | 2-4 | 30-60 |
| `sha256` | 0.2-0.5 | 2-5 |

*Actual performance varies by CPU, compiler, and data patterns.*

## Using Statistics

The library tracks hash function usage through the CRTP base class:

```cpp
#include <algebraic_hashing/functions/fnv_hash_modern.hpp>
using namespace algebraic_hashing::functions;

fnv64 hasher;

// Hash some data
for (int i = 0; i < 10000; ++i) {
    hasher(std::to_string(i));
}

// Get statistics
auto stats = hasher.get_stats();
std::cout << "Call count: " << stats.call_count << "\n";
std::cout << "Bytes hashed: " << stats.bytes_hashed << "\n";
```

### Available Statistics

```cpp
struct statistics {
    std::size_t call_count = 0;    // Number of hash invocations
    std::size_t bytes_hashed = 0;  // Total bytes processed
};
```

### Resetting Statistics

```cpp
fnv64 hasher;

hasher("test1");
hasher("test2");
// stats.call_count == 2

hasher.reset_stats();
// stats.call_count == 0
```

### Statistics in Multi-Threaded Code

!!! warning "Thread Safety"
    Statistics collection is NOT thread-safe. In multi-threaded code:

    ```cpp
    // Option 1: Disable statistics at compile time
    // cmake -DCMAKE_CXX_FLAGS="-DALGEBRAIC_HASHING_DISABLE_STATISTICS"

    // Option 2: Use separate hasher per thread
    void worker_thread() {
        fnv64 local_hasher;  // Thread-local instance
        // Use local_hasher...
    }
    ```

## Composition Performance

Composed hash functions have minimal overhead:

```cpp
fnv64 f;
murmur3_64 g;

// XOR composition: runs both functions, XORs results
auto xor_composed = f ^ g;
// Cost: f(x) + g(x) + XOR operation

// Sequential composition: chains functions
auto seq_composed = f * g;
// Cost: g(x) + f(result)
```

### Zero-Cost Abstraction

The CRTP pattern ensures composition has no virtual function overhead:

```cpp
// This composed function:
auto h = fnv64{} ^ murmur3_64{};
auto result = h("test");

// Compiles to essentially:
auto result = fnv64{}("test") ^ murmur3_64{}("test");
```

## Optimization Techniques

### 1. Reuse Hash Function Objects

```cpp
// Good: create once, use many times
fnv64 hasher;
for (const auto& item : items) {
    hashes.push_back(hasher(item));
}

// Avoid: creates object each iteration
for (const auto& item : items) {
    hashes.push_back(fnv64{}(item));
}
```

### 2. Choose the Right Hash Size

```cpp
// For hash tables (collisions handled):
fnv32 small;  // 32 bits is usually sufficient

// For checksums/deduplication:
fnv64 medium;  // 64 bits for good collision resistance

// For extra safety:
murmur3_128 large;  // 128 bits for very low collision probability
```

### 3. Batch Processing

```cpp
// Process in batches for cache efficiency
xxhash64 hasher;
std::vector<hash64> results;
results.reserve(items.size());

for (const auto& item : items) {
    results.push_back(hasher(item));
}
```

### 4. Prefer xxHash for Raw Speed

```cpp
// When performance matters most:
xxhash64 fast_hasher;

// Benchmark comparison
auto start = std::chrono::high_resolution_clock::now();
for (int i = 0; i < 1000000; ++i) {
    volatile auto h = fast_hasher(data);
}
auto end = std::chrono::high_resolution_clock::now();
```

### 5. Use Seeds for Partitioning

```cpp
// Create multiple independent hash functions with seeds
xxhash64 h1(0);
xxhash64 h2(1);
xxhash64 h3(2);

// Use for parallel processing, Bloom filters, etc.
```

## Benchmarking Your Code

### Simple Timing

```cpp
#include <chrono>

template<typename F>
double benchmark(F hasher, const std::string& data, int iterations) {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        volatile auto h = hasher(data);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start);

    return iterations / duration.count();  // ops per second
}

// Usage
double fnv_ops = benchmark(fnv64{}, "test string", 1000000);
double xx_ops = benchmark(xxhash64{}, "test string", 1000000);

std::cout << "FNV: " << fnv_ops / 1e6 << " M ops/s\n";
std::cout << "xxHash: " << xx_ops / 1e6 << " M ops/s\n";
```

### Using the Built-in Benchmark

The library includes a comprehensive benchmark:

```bash
# Build with benchmarks enabled
cmake -B build -DBUILD_BENCHMARKS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run benchmark
./build/benchmark_comprehensive
```

### Throughput Measurement

```cpp
double measure_throughput(auto hasher, std::size_t data_size, int iterations) {
    std::string data(data_size, 'x');

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        volatile auto h = hasher(data);
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto seconds = std::chrono::duration<double>(end - start).count();
    auto bytes = data_size * iterations;

    return bytes / seconds / 1e9;  // GB/s
}

// Compare throughput
std::cout << "FNV throughput: " << measure_throughput(fnv64{}, 1024*1024, 100) << " GB/s\n";
std::cout << "xxHash throughput: " << measure_throughput(xxhash64{}, 1024*1024, 100) << " GB/s\n";
```

## Memory Considerations

### Hash Value Sizes

| Type | Size | Typical Use |
|------|------|-------------|
| `hash32` | 4 bytes | Hash tables with few entries |
| `hash64` | 8 bytes | General purpose |
| `hash128` | 16 bytes | Low collision requirements |
| `hash256` | 32 bytes | Cryptographic digests |

### Composition Overhead

Composed hash functions store their components:

```cpp
sizeof(fnv64)                    // ~1 byte (empty class optimization)
sizeof(fnv64 ^ murmur3_64)       // ~2 bytes (stores both)
sizeof((f ^ g) * k)              // ~3 bytes (nested composition)
```

## Common Pitfalls

### Don't Hash Too Much

```cpp
// Bad: hashing entire large object unnecessarily
struct BigObject {
    std::array<char, 10000> data;
    std::string key;  // This is what identifies it
};

// Better: hash just the identifying parts
fnv64{}(obj.key);
```

### Avoid Redundant Hashing

```cpp
// Bad: hash computed multiple times
if (cache.contains(fnv64{}(key))) {
    return cache[fnv64{}(key)];  // Hashes again!
}

// Better: compute once
auto h = fnv64{}(key);
if (cache.contains(h)) {
    return cache[h];
}
```

### Profile Before Optimizing

```cpp
// Don't assume - measure!
// xxHash is faster, but FNV might be "fast enough"
// and simpler to understand
```

## Next Steps

- [Benchmarks Directory](https://github.com/queelius/algebraic_hashing/tree/master/benchmarks) - Full benchmark suite
- [Core Concepts](../concepts/core-concepts.md) - Understand the architecture
- [Design Philosophy](../concepts/philosophy.md) - Why zero-cost abstractions matter
