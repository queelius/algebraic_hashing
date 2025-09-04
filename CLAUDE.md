# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a C++20 library implementing various algebraic hash functions, both cryptographic and non-cryptographic. The library provides hash function composition and algebraic operations over hash functions, including standard hash functions, perfect hash functions (PHFs), and cryptographic hashes.

## Build Commands

The project uses manual compilation with g++ (no CMake/Make). Tests are compiled individually:

```bash
# Compile tests (from tests/ directory)
g++ -std=c++20 -O3 -o test_rd_phf test_rd_phf.cpp -I ../include -pthread
g++ -std=c++20 -O3 -o test_rd_phf_lvl2 test_rd_phf_lvl2.cpp -I ../include -pthread
g++ -std=c++20 -o test_fnv_hash test_fnv_hash.cpp -I ../include
g++ -std=c++20 -o test_proracle test_proracle.cpp -I ../include

# Run individual tests
./test_rd_phf
./test_rd_phf_lvl2
./test_fnv_hash
./test_proracle
```

Note: The compile scripts in tests/ have incorrect include paths pointing to `~/sources/algebraic_hash_functions/include`. Use `-I ../include` instead when compiling from the tests directory.

## Code Architecture

### Core Components

**Namespace**: All code is in the `algebraic_hashing` namespace.

**Hash Functions (`include/algebraic_hashing/hashing/`)**
- `fnv_hash.hpp`: FNV-1a hash implementation with support for various types
- `proracle.hpp`: Pseudo-random oracle implementation
- `hash_value.hpp`: Core hash value type definitions

**Perfect Hashing (`include/algebraic_hashing/perfect_hashing/`)**
- `rd_phf.hpp`: Simple PHF implementation (theoretical interest)
- `rd_phf_builder.hpp`: Builder pattern for constructing PHFs
- `rd_phf_lvl2.hpp`: Two-level PHF - faster construction, slightly slower queries
- `rd_phf_lvl2_builder.hpp`: Builder for two-level PHF

**Cryptographic Hashing (`include/algebraic_hashing/cryptographic_hashing/`)**
- `cryptographic_hash.hpp`: Base cryptographic hash interface
- `cryptographic_hash_fn.hpp`: Function interface for cryptographic hashes

**Algebraic Operations (`include/algebraic_hashing/algebra/`)**
- `xor_hash_fn_compose.hpp`: Hash function composition using XOR

**CMPH Integration (`include/algebraic_hashing/cmph/`)**
- Integration with external CMPH library for minimal perfect hashing

### Key Design Patterns

- **Builder Pattern**: PHF builders use method chaining for configuration
- **Template-based Hash Selection**: Hash function type is a template parameter
- **Header-only Library**: All implementations are in header files

### Testing Approach

Tests are individual C++ files in `tests/` that can be compiled and run independently. Each test file includes the necessary headers and a main() function. The `utils.hpp` file provides shared testing utilities like random string generation.