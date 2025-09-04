# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.0.0] - 2025-01-29

### Added
- **Modern C++20 Architecture**: Complete rewrite using C++20 concepts and constexpr
- **Elegant DSL**: Intuitive operators for hash function composition (`^`, `*`, `~`)
- **Core Concepts Framework**: Type-safe hash function composition with concepts
- **Sophisticated Hash Value Type**: `hash_value<N>` with full algebraic operations
- **Advanced Composition Operators**: XOR, sequential, repeated, complement, chain, and parallel composition
- **Educational Framework**: CRTP base class with statistics and analysis tools
- **Modern FNV Implementation**: Multi-size FNV hash with compile-time evaluation
- **Comprehensive Testing**: 99.4% test coverage with 48 test cases
- **Professional Build System**: CMake 3.20+ with proper package configuration
- **Documentation**: Doxygen integration with comprehensive API documentation
- **Package Management**: Full Conan 2.0 support with test package
- **CI/CD Pipeline**: GitHub Actions with multi-platform testing and coverage
- **Code Quality Tools**: clang-format, clang-tidy, and static analysis integration

### Changed
- **Breaking API Changes**: Complete API redesign for modern C++20 idioms
- **Performance Improvements**: Zero-cost abstractions and compile-time optimizations
- **Mathematical Foundation**: Hash functions now form proper algebraic structures
- **Type Safety**: Concepts-based design prevents common usage errors

### Deprecated
- Legacy C++11/14 APIs (will be removed in v3.0.0)
- Direct XOR composition functions (use operator^ instead)

### Removed
- Legacy build system dependencies
- Non-standard extensions and workarounds
- Outdated hash function implementations

### Fixed
- **Thread Safety**: All hash functions are now thread-safe by design
- **Undefined Behavior**: Eliminated all sources of UB through careful design
- **Memory Safety**: RAII and smart pointer usage throughout
- **Portability**: Proper platform abstraction for all supported systems

### Security
- **Constant-time Operations**: Where cryptographically relevant
- **Secure Random Number Generation**: For salt generation and testing
- **Input Validation**: Robust handling of edge cases and malformed input

## [1.x.x] - Legacy Versions

Previous versions followed a different architecture and are not directly comparable.
See git history for detailed changes in the legacy codebase.

---

**Note**: This changelog starts with version 2.0.0 as it represents a complete 
architectural overhaul of the library. Previous versions used different APIs
and design principles.