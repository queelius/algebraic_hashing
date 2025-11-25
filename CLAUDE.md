# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

AlgebraicHashing is a modern C++20 header-only library for algebraic hash function composition. It provides an elegant DSL for combining hash functions using mathematical operators and exploring their algebraic properties.

**Key Features:**
- Modern C++20 with concepts for type safety
- Header-only library with zero-cost abstractions
- Comprehensive test coverage with extensive validation
- Multi-platform support (Linux, Windows, macOS)
- Professional packaging (Conan, vcpkg, CMake)

## Build System

The project uses **CMake** as the primary build system. There are legacy manual compilation scripts in `tests/` that should be ignored in favor of the CMake workflow.

### Quick Start - Build and Test

```bash
# Configure with tests enabled (default)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build the library and tests
cmake --build build

# Run all tests
cd build && ctest --output-on-failure

# Or run tests with verbose output
cd build && ctest -V
```

### Common Build Configurations

```bash
# Debug build with sanitizers (for development)
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build && cd build && ctest

# Release build with optimizations
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3"
cmake --build build

# Build with benchmarks
cmake -B build -DBUILD_BENCHMARKS=ON
cmake --build build
./build/benchmark_comprehensive

# Build with examples
cmake -B build -DBUILD_EXAMPLES=ON
cmake --build build
./build/algebraic_hashing_tutorial

# Test coverage analysis
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage"
cmake --build build && cd build && ctest
lcov --capture --directory . --output-file coverage.info
lcov --list coverage.info
```

### CMake Build Options

- `BUILD_TESTING=ON/OFF` - Build test suite (default: ON)
- `BUILD_EXAMPLES=ON/OFF` - Build tutorial examples (default: ON)
- `BUILD_BENCHMARKS=ON/OFF` - Build performance benchmarks (default: OFF)
- `ENABLE_CONCEPTS_CHECKING=ON/OFF` - Enable extensive concepts validation (default: ON)
- `ENABLE_STATISTICS=ON/OFF` - Enable performance statistics collection (default: ON)
- `ENABLE_COVERAGE=ON/OFF` - Enable code coverage reporting with lcov/genhtml (default: OFF)

### Coverage Reporting

```bash
# Build with coverage enabled
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build

# Run tests
cd build && ctest

# Generate coverage report (HTML)
cmake --build . --target coverage

# Open coverage report in browser
cmake --build . --target coverage_open
```

Coverage reports are generated in `build/coverage_html/index.html`.

### Package Management

```bash
# Install via Conan (recommended for development)
conan install --requires=algebraic_hashing/2.0.1@

# Create Conan package locally
conan create . --build=missing

# Install via vcpkg
vcpkg install algebraic-hashing
```

## Architecture

The library has a **dual architecture** due to ongoing modernization:

### Modern Architecture (Primary - Use This)

Located in `include/algebraic_hashing/`:

```
core/                           # Foundation layer
├── concepts.hpp               # C++20 concepts for type safety
├── hash_value.hpp             # Mathematical hash value type with algebraic operations
└── hash_function_base.hpp     # CRTP base class with statistics and instrumentation

functions/                     # Hash function implementations
└── fnv_hash_modern.hpp        # Modern FNV-1a implementation (fnv32, fnv64)

dsl/                           # Domain-specific language
└── algebraic_operations.hpp   # Operators for composition (^, *, ~)
```

**Design Principles:**
- **Layered architecture**: DSL → Composition → Concepts → Core
- **Concepts-first**: All operations validated at compile-time
- **CRTP pattern**: `hash_function_base<Derived>` for zero-cost statistics
- **Algebraic semantics**: Hash values form abelian groups under XOR
- **Template composition**: Functions compose through template parameters

### Legacy Architecture (Deprecated - Being Phased Out)

Located in `include/algebraic_hashing/`:

```
hashing/                       # Old hash implementations
├── fnv_hash.hpp              # Legacy FNV implementation
├── proracle.hpp              # Pseudo-random oracle
└── hash_value.hpp            # Legacy hash value type

perfect_hashing/              # Perfect hash functions (theoretical)
├── rd_phf.hpp                # Simple PHF (slow construction)
├── rd_phf_builder.hpp
├── rd_phf_lvl2.hpp           # Two-level PHF (faster)
└── rd_phf_lvl2_builder.hpp

cryptographic_hashing/        # Cryptographic hash interfaces
├── cryptographic_hash.hpp
└── cryptographic_hash_fn.hpp

algebra/                      # Legacy composition
└── xor_hash_fn_compose.hpp

cmph/                         # External CMPH library integration
├── cmph.hpp
└── cmph_builder.hpp
```

**When working on new features, use the modern architecture.** The legacy code is maintained for backward compatibility but should not be extended.

## Testing Strategy

### Modern Tests (Primary)

Located in `tests/`:
- `test_modern_architecture.cpp` - Core functionality tests
- `test_comprehensive_coverage.cpp` - Extensive coverage tests including:
  - FNV-1a official test vectors (32-bit and 64-bit)
  - Thread safety tests (concurrent hashing, shared hasher instances)
  - Numeric hashing consistency regression tests
  - Algebraic property validation

Run via CMake/CTest:
```bash
cmake -B build -DBUILD_TESTING=ON
cmake --build build
cd build && ctest --output-on-failure
```

### Legacy Tests (Deprecated)

Old test files with manual compilation scripts:
- `test_fnv_hash.cpp`, `compile_test_fnv_hash`
- `test_rd_phf.cpp`, `compile_test_rd_phf`
- `test_proracle.cpp`, `compile_test_proracle`

**NOTE**: The `compile_test_*` scripts have incorrect include paths pointing to `~/sources/algebraic_hash_functions/include`. They should not be used. Use the CMake test workflow instead.

### Coverage Requirements

The project maintains **comprehensive test coverage**. When adding new code:
1. Write tests first (TDD approach)
2. Run coverage analysis: `cmake -DCMAKE_CXX_FLAGS="--coverage"`
3. Ensure new code is fully covered
4. Update `test_comprehensive_coverage.cpp` for extensive validation

## Development Workflow

### Adding a New Hash Function

1. Implement in `include/algebraic_hashing/functions/your_hash.hpp`
2. Derive from `hash_function_base<YourHash>` (CRTP)
3. Implement `operator()` for `Hashable` types
4. Return `hash_value<N>` where N is the bit size
5. Add tests to `test_modern_architecture.cpp`
6. Add coverage tests to `test_comprehensive_coverage.cpp`
7. Run: `cmake --build build && cd build && ctest`

### Modifying Core Concepts

1. Edit `include/algebraic_hashing/core/concepts.hpp`
2. Ensure all existing code still compiles (concepts are compile-time checks)
3. Update affected implementations
4. Run comprehensive tests to catch concept violations

### Performance Optimization

1. Add benchmarks to `benchmarks/benchmark_comprehensive.cpp`
2. Build with: `cmake -B build -DBUILD_BENCHMARKS=ON -DCMAKE_BUILD_TYPE=Release`
3. Run baseline: `./build/benchmark_comprehensive > before.txt`
4. Make optimizations
5. Compare: `./build/benchmark_comprehensive > after.txt`
6. Document results in `benchmarks/RESULTS.md`

## Code Quality Standards

### Compiler Requirements

- **GCC 10+** (recommended: GCC 13)
- **Clang 12+** (recommended: Clang 15)
- **MSVC 2019+** (Visual Studio 16.0+)
- **C++20 required** with full concepts support

### Static Analysis

```bash
# Format code
find include tests examples -name "*.hpp" -o -name "*.cpp" | xargs clang-format -i

# Run clang-tidy (integrated in CMake)
cmake -B build -DCMAKE_CXX_CLANG_TIDY=clang-tidy
cmake --build build
```

### Sanitizers (Debug Builds)

Debug builds automatically enable:
- AddressSanitizer (ASan) - detects memory errors
- UndefinedBehaviorSanitizer (UBSan) - detects undefined behavior

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build && cd build && ctest
# Any sanitizer violations will be reported
```

## Key Design Patterns

### CRTP for Zero-Cost Statistics

```cpp
template<typename Derived>
class hash_function_base {
    auto operator()(const auto& data) {
        // Statistics collection (zero-cost when disabled)
        return static_cast<Derived*>(this)->hash_impl(data);
    }
};

class fnv64 : public hash_function_base<fnv64> {
    auto hash_impl(const auto& data) { /* ... */ }
};
```

**Thread Safety Note**: Hash functions are thread-safe for concurrent hashing, but statistics collection is NOT thread-safe. To disable statistics entirely (recommended for multi-threaded applications), compile with:
```bash
cmake -B build -DCMAKE_CXX_FLAGS="-DALGEBRAIC_HASHING_DISABLE_STATISTICS"
```

### Concepts for Type Safety

```cpp
template<Hashable T>
auto hash(const T& data) {
    // Compile-time guarantee that T is hashable
    return fnv64{}(data);
}
```

### Algebraic Operators

```cpp
auto h1 = fnv64{};
auto h2 = fnv32{};
auto composed = h1 ^ h2;        // XOR composition
auto chained = h1 * h2;         // Sequential composition
auto complement = ~h1;          // Bitwise complement
```

## Documentation

The project has two documentation systems:
- **MkDocs**: Conceptual documentation, tutorials, and motivation - https://queelius.github.io/algebraic_hashing/
- **Doxygen**: API reference documentation

### MkDocs Documentation

Located in `docs_mkdocs/`:
- `index.md` - Landing page with features and quick example
- `concepts/motivation.md` - Why algebraic hash composition matters
- `getting-started/` - Installation and quickstart guides
- `reference/api.md` - API overview

```bash
# Serve locally for development
mkdocs serve

# Deploy to GitHub Pages
mkdocs gh-deploy
```

### Generate API Docs (Doxygen)

```bash
# Install Doxygen first
sudo apt-get install doxygen

# Generate documentation
cmake -B build
cmake --build build --target docs

# Open in browser
cmake --build build --target docs_open
# Or manually: xdg-open build/docs/html/index.html
```

### Documentation Standards

- All public APIs must have Doxygen comments
- Include `@brief`, `@param`, `@return`, `@tparam` as appropriate
- Provide usage examples in comments for complex APIs
- Mathematical concepts should reference algebraic properties

## Common Issues

### Issue: "Concepts not supported"
**Solution**: Ensure compiler is GCC 10+, Clang 12+, or MSVC 2019+. Check with `g++ --version` or `clang++ --version`.

### Issue: Legacy test scripts fail with include errors
**Solution**: Ignore the manual `compile_test_*` scripts. Use CMake: `cmake -B build && cmake --build build && cd build && ctest`

### Issue: Test failures with sanitizers
**Solution**: This indicates real bugs (memory leaks, undefined behavior). Fix the code, don't disable sanitizers.

### Issue: Poor hash distribution in perfect hashing
**Solution**: PHFs are theoretical implementations. For production, use the modern FNV implementations or external libraries like CMPH.

## CI/CD Pipeline

The GitHub Actions CI runs:
1. Code quality checks (clang-format, clang-tidy)
2. Multi-platform builds (Linux, Windows, macOS)
3. Multi-compiler tests (GCC, Clang, MSVC)
4. Sanitizer validation (ASan, UBSan)
5. Coverage reporting (codecov.io)
6. Conan package validation

Tests must pass on all platforms before merging.
