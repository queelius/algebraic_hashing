# Installation

AlgebraicHashing is a **header-only C++20 library** with multiple installation options.

## Requirements

- **C++20 compiler**:
    - GCC 10+ (recommended: GCC 13)
    - Clang 12+ (recommended: Clang 15)
    - MSVC 2019+ (Visual Studio 16.0+)
- **CMake 3.20+** (for building tests/examples)

## Quick Install Options

=== "CMake FetchContent"

    The easiest way to integrate into your CMake project:

    ```cmake
    include(FetchContent)

    FetchContent_Declare(
        algebraic_hashing
        GIT_REPOSITORY https://github.com/queelius/algebraic_hashing.git
        GIT_TAG v2.0.1
    )

    FetchContent_MakeAvailable(algebraic_hashing)

    target_link_libraries(your_target PRIVATE AlgebraicHashing::algebraic_hashing)
    ```

=== "Conan"

    For Conan package management:

    ```bash
    # Add to conanfile.txt
    [requires]
    algebraic_hashing/2.0.1

    [generators]
    CMakeDeps
    CMakeToolchain
    ```

    Then in CMakeLists.txt:

    ```cmake
    find_package(algebraic_hashing REQUIRED)
    target_link_libraries(your_target PRIVATE algebraic_hashing::algebraic_hashing)
    ```

=== "vcpkg"

    Using vcpkg package manager:

    ```bash
    vcpkg install algebraic-hashing
    ```

    Then in CMakeLists.txt:

    ```cmake
    find_package(algebraic_hashing CONFIG REQUIRED)
    target_link_libraries(your_target PRIVATE algebraic_hashing::algebraic_hashing)
    ```

=== "Manual"

    Clone and install manually:

    ```bash
    git clone https://github.com/queelius/algebraic_hashing.git
    cd algebraic_hashing
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build
    sudo cmake --install build
    ```

## Verify Installation

Create a test file `test_install.cpp`:

```cpp
#include <algebraic_hashing/functions/fnv_hash_modern.hpp>
#include <algebraic_hashing/dsl/algebraic_operations.hpp>
#include <iostream>

int main() {
    using namespace algebraic_hashing;

    auto hash = fnv64{};
    auto result = hash("AlgebraicHashing");

    std::cout << "Hash result: " << result << "\n";
    std::cout << "Installation successful!\n";

    return 0;
}
```

Compile and run:

```bash
# With CMake
cmake -B build
cmake --build build
./build/test_install

# Or directly with g++
g++ -std=c++20 -I/path/to/algebraic_hashing/include test_install.cpp -o test_install
./test_install
```

Expected output:
```
Hash result: [some hash value]
Installation successful!
```

## Building from Source

### Clone the Repository

```bash
git clone https://github.com/queelius/algebraic_hashing.git
cd algebraic_hashing
```

### Build Options

```bash
# Release build (optimized)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Debug build (with sanitizers)
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# With tests enabled
cmake -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure

# With examples
cmake -B build -DBUILD_EXAMPLES=ON
cmake --build build
./build/algebraic_hashing_tutorial

# With coverage reporting
cmake -B build -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target coverage
# Open build/coverage_html/index.html
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_TESTING` | ON | Build test suite |
| `BUILD_EXAMPLES` | ON | Build tutorial examples |
| `BUILD_BENCHMARKS` | OFF | Build performance benchmarks |
| `ENABLE_COVERAGE` | OFF | Enable code coverage reporting |
| `ENABLE_CONCEPTS_CHECKING` | ON | Enable extensive concepts validation |
| `ENABLE_STATISTICS` | ON | Enable performance statistics collection |

## Using in Your Project

### With CMake

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option 1: Find installed package
find_package(algebraic_hashing REQUIRED)

# Option 2: Use FetchContent (see above)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE AlgebraicHashing::algebraic_hashing)
```

### Header-Only (Manual)

If you prefer not to use a build system:

```cpp
// Simply include the headers you need
#include <algebraic_hashing/functions/fnv_hash_modern.hpp>
#include <algebraic_hashing/dsl/algebraic_operations.hpp>

// Compile with C++20
// g++ -std=c++20 -I/path/to/include main.cpp
```

## Troubleshooting

### Compiler Not Found

```bash
# Specify compiler explicitly
cmake -B build -DCMAKE_CXX_COMPILER=g++-13
```

### C++20 Not Supported

```bash
# Check your compiler version
g++ --version    # Should be 10+
clang++ --version  # Should be 12+
```

If your compiler is too old, install a newer version:

```bash
# Ubuntu/Debian
sudo apt install g++-13

# macOS with Homebrew
brew install gcc@13

# Windows
# Download from https://winlibs.com/
```

### Concepts Not Working

Make sure you're actually using C++20:

```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

### Tests Failing

This usually means a compiler or standard library issue:

```bash
# Run tests with verbose output
cd build
ctest -V

# Run specific test
./test_modern_architecture --gtest_filter=YourTestName
```

## Next Steps

- [Quick Start Guide](quickstart.md) - Your first hash composition
- [Core Concepts](../concepts/core-concepts.md) - Understand the foundations
- [Examples](../examples/use-cases.md) - See practical applications

## Platform-Specific Notes

### Linux

AlgebraicHashing works out-of-the-box on all modern Linux distributions. Use your package manager's GCC or Clang.

### macOS

Xcode's default compiler should work, but for best results use Homebrew's GCC or Clang:

```bash
brew install gcc@13
export CXX=g++-13
```

### Windows

Recommended setup:

1. Install Visual Studio 2022 (includes MSVC with C++20 support)
2. Or use MSYS2/MinGW-w64 with GCC 13+

```bash
# With MSYS2
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-cmake
```

---

Need help? [Open an issue](https://github.com/queelius/algebraic_hashing/issues) on GitHub.
