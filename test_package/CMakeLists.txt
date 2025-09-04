cmake_minimum_required(VERSION 3.20)
project(TestAlgebraicHashing CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find the package
find_package(AlgebraicHashing REQUIRED)

# Create test executable
add_executable(test_algebraic_hashing test_basic_usage.cpp)

# Link against the library
target_link_libraries(test_algebraic_hashing 
    PRIVATE 
        AlgebraicHashing::algebraic_hashing
)

# Set properties
set_target_properties(test_algebraic_hashing
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)