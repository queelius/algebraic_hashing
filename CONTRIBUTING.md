# Contributing to AlgebraicHashing

Thank you for your interest in contributing to AlgebraicHashing! This document provides guidelines and information for contributors.

## Code of Conduct

This project adheres to a code of conduct adapted from the [Contributor Covenant](https://www.contributor-covenant.org/). By participating, you are expected to uphold this code.

## Getting Started

### Prerequisites

- **C++20 compatible compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake 3.20+**
- **Git**
- **Conan 2.0+** (for package testing)

### Setting up the Development Environment

1. **Clone the repository**:
   ```bash
   git clone https://github.com/algebraic-hashing/algebraic_hashing.git
   cd algebraic_hashing
   ```

2. **Build the project**:
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
   cmake --build build
   ```

3. **Run tests**:
   ```bash
   cd build && ctest --output-on-failure
   ```

4. **Run examples**:
   ```bash
   ./build/algebraic_hashing_tutorial
   ```

## Development Workflow

### Branch Management

- **main/master**: Stable release branch
- **develop**: Integration branch for features
- **feature/***: Individual feature branches
- **bugfix/***: Bug fix branches
- **hotfix/***: Critical fixes for released versions

### Making Changes

1. **Create a feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes following the coding standards**

3. **Run the test suite**:
   ```bash
   cmake --build build && cd build && ctest
   ```

4. **Format your code**:
   ```bash
   find include tests examples -name "*.hpp" -o -name "*.cpp" | xargs clang-format -i
   ```

5. **Run static analysis**:
   ```bash
   clang-tidy include/algebraic_hashing/**/*.hpp
   ```

## Coding Standards

### C++ Guidelines

- **C++20 Standard**: Use modern C++20 features appropriately
- **Concepts**: Prefer concepts over SFINAE for template constraints
- **const-correctness**: Always maintain const-correctness
- **RAII**: Use RAII for resource management
- **Zero-cost abstractions**: Prefer compile-time over runtime polymorphism

### Code Style

We use **clang-format** with a custom configuration based on Google style:

- **Indentation**: 4 spaces (no tabs)
- **Line length**: 100 characters maximum
- **Braces**: Attached style for functions and classes
- **Naming conventions**:
  - Types: `lower_case` or `CamelCase` for template parameters
  - Functions/variables: `lower_case`
  - Constants: `CamelCase`
  - Private members: suffix with `_`

### Documentation Standards

- **API Documentation**: All public interfaces must have Doxygen comments
- **Code Comments**: Explain the "why", not the "what"
- **Examples**: Include usage examples for complex features
- **Mathematical notation**: Use LaTeX for mathematical expressions

```cpp
/**
 * @brief Computes the XOR composition of two hash functions
 * 
 * Given hash functions f₁ and f₂, creates a new hash function h such that:
 * h(x) = f₁(x) ⊕ f₂(x)
 * 
 * This operation preserves the entropy of the stronger component function
 * and provides mathematical properties useful for Bloom filters and other
 * probabilistic data structures.
 * 
 * @tparam F1 Type of first hash function (must satisfy ComposableHashFunction)
 * @tparam F2 Type of second hash function (must satisfy ComposableHashFunction)
 * @param f1 First hash function
 * @param f2 Second hash function
 * @return XOR composition of the two hash functions
 * 
 * @code
 * auto fnv1 = fnv64{};
 * auto fnv2 = fnv32{};
 * auto composed = fnv1 ^ fnv2;  // XOR composition
 * auto result = composed("Hello, World!");
 * @endcode
 */
template<ComposableHashFunction F1, ComposableHashFunction F2>
constexpr auto operator^(F1&& f1, F2&& f2);
```

## Testing Guidelines

### Test Categories

1. **Unit Tests**: Test individual components in isolation
2. **Integration Tests**: Test component interactions
3. **Property Tests**: Test mathematical properties (commutativity, associativity)
4. **Performance Tests**: Validate performance characteristics
5. **Regression Tests**: Prevent known issues from reoccurring

### Writing Tests

```cpp
TEST(HashValueTest, XorCommutativity) {
    // Test that XOR operation is commutative: a ⊕ b = b ⊕ a
    auto h1 = hash_value<64>::from_string("test1");
    auto h2 = hash_value<64>::from_string("test2");
    
    EXPECT_EQ(h1 ^ h2, h2 ^ h1);
}
```

### Test Coverage

- Aim for **95%+ coverage** for core functionality
- All public APIs must have comprehensive tests
- Edge cases and error conditions must be tested
- Performance tests should validate algorithmic complexity

## Performance Considerations

### Benchmarking

- Use consistent benchmarking methodology
- Test on multiple platforms and compilers
- Measure both throughput and latency
- Include memory usage analysis

### Optimization Guidelines

- **Profile first**: Don't optimize without measuring
- **Prefer algorithms over micro-optimizations**
- **Use constexpr**: Enable compile-time computation where possible
- **Template metaprogramming**: Eliminate runtime overhead
- **SIMD**: Consider vectorization for bulk operations

## Submitting Changes

### Pull Request Process

1. **Ensure all tests pass** locally and in CI
2. **Update documentation** for any API changes
3. **Add tests** for new functionality
4. **Update CHANGELOG.md** following semantic versioning
5. **Ensure code is formatted** and passes static analysis

### Pull Request Template

```markdown
## Description
Brief description of changes and motivation.

## Type of Change
- [ ] Bug fix (non-breaking change that fixes an issue)
- [ ] New feature (non-breaking change that adds functionality)
- [ ] Breaking change (fix or feature that would cause existing functionality to change)
- [ ] Documentation update
- [ ] Performance improvement
- [ ] Code refactoring

## Testing
- [ ] Unit tests pass
- [ ] Integration tests pass
- [ ] New tests added for new functionality
- [ ] Manual testing performed

## Documentation
- [ ] API documentation updated
- [ ] Examples added/updated
- [ ] README updated if needed

## Performance Impact
Describe any performance implications, including benchmark results if applicable.

## Breaking Changes
List any breaking changes and migration path for users.
```

## Release Process

### Semantic Versioning

We follow [Semantic Versioning 2.0.0](https://semver.org/):

- **MAJOR**: Incompatible API changes
- **MINOR**: Backward-compatible functionality additions
- **PATCH**: Backward-compatible bug fixes

### Release Checklist

1. **Update version numbers** in CMakeLists.txt and conanfile.py
2. **Update CHANGELOG.md** with release notes
3. **Ensure all CI checks pass**
4. **Tag the release**: `git tag -a v2.1.0 -m "Release v2.1.0"`
5. **Push tags**: `git push origin --tags`
6. **GitHub Actions will handle** the rest (artifacts, documentation, etc.)

## Getting Help

### Communication Channels

- **GitHub Issues**: Bug reports and feature requests
- **GitHub Discussions**: General questions and design discussions
- **Email**: For security-related issues

### Documentation

- **API Documentation**: Generated with Doxygen
- **Examples**: See `examples/` directory
- **Tutorial**: `examples/algebraic_hashing_tutorial.cpp`

## Recognition

Contributors will be acknowledged in:
- **CONTRIBUTORS.md** file
- **GitHub contributors page**
- **Release notes** for significant contributions
- **Academic papers** for research contributions

## Mathematical Contributions

This library has strong mathematical foundations. Contributions involving:
- **New hash functions** with proven properties
- **Algebraic operations** on hash functions
- **Theoretical analysis** of composition properties
- **Formal verification** of mathematical claims

are especially welcome and should include appropriate mathematical documentation and references.

Thank you for contributing to AlgebraicHashing!