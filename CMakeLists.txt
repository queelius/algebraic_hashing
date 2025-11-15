cmake_minimum_required(VERSION 3.20)

# ==================== Project Configuration ====================

project(AlgebraicHashing
    VERSION 2.0.0
    DESCRIPTION "Modern C++ library for algebraic hash function composition"
    LANGUAGES CXX
)

# Set C++20 as the minimum standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ==================== Build Configuration ====================

# Enable debug info for all build types
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    
    # Only add conversion warnings for library development, not for consumers
    if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        add_compile_options(-Wconversion -Wshadow)
    endif()
    
    # Enable optimizations for Release (avoid march=native for package distribution)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    
    # Enable debugging and sanitizers for Debug
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -fsanitize=address,undefined -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address,undefined")
endif()

# Enable concepts and ranges support
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "10.0")
        message(FATAL_ERROR "GCC 10+ is required for C++20 concepts support")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "12.0")
        message(FATAL_ERROR "Clang 12+ is required for C++20 concepts support")  
    endif()
endif()

# ==================== Dependencies ====================

# Find threading library
find_package(Threads REQUIRED)

# Build options
option(BUILD_TESTING "Build the testing tree" ON)
option(BUILD_EXAMPLES "Build examples and tutorials" ON) 
option(BUILD_BENCHMARKS "Build performance benchmarks" OFF)
option(ENABLE_CONCEPTS_CHECKING "Enable extensive concepts checking" ON)
option(ENABLE_STATISTICS "Enable performance statistics collection" ON)

# Google Test for testing (optional)
if(BUILD_TESTING OR BUILD_BENCHMARKS)
    find_package(GTest QUIET)
    if(NOT GTest_FOUND)
        message(STATUS "GTest not found, building from source")
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.12.1
        )
        FetchContent_MakeAvailable(googletest)
    endif()
endif()

# ==================== Library Definition ====================

# Header-only library
add_library(algebraic_hashing INTERFACE)
add_library(AlgebraicHashing::algebraic_hashing ALIAS algebraic_hashing)

# Set include directories
target_include_directories(algebraic_hashing
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Set compile features
target_compile_features(algebraic_hashing
    INTERFACE
        cxx_std_20
)

# Link threading
target_link_libraries(algebraic_hashing
    INTERFACE
        Threads::Threads
)

# Add preprocessor definitions based on options
if(ENABLE_CONCEPTS_CHECKING)
    target_compile_definitions(algebraic_hashing 
        INTERFACE 
            ALGEBRAIC_HASHING_ENABLE_CONCEPTS_CHECKING
    )
endif()

if(ENABLE_STATISTICS)
    target_compile_definitions(algebraic_hashing 
        INTERFACE 
            ALGEBRAIC_HASHING_ENABLE_STATISTICS
    )
endif()

# ==================== Executable Targets ====================

# Tutorial executable (conditional)
if(BUILD_EXAMPLES)
    add_executable(algebraic_hashing_tutorial
        examples/algebraic_hashing_tutorial.cpp
    )

    target_link_libraries(algebraic_hashing_tutorial
        PRIVATE
            algebraic_hashing
    )

    # Set properties for executables
    set_target_properties(algebraic_hashing_tutorial
        PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
    )
endif()

# Benchmark executables (conditional)
if(BUILD_BENCHMARKS)
    add_executable(benchmark_comprehensive
        benchmarks/benchmark_comprehensive.cpp
    )
    
    target_link_libraries(benchmark_comprehensive
        PRIVATE
            algebraic_hashing
            GTest::gtest_main
    )
    
    # Set properties for benchmark
    set_target_properties(benchmark_comprehensive
        PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
    )
    
    # Custom target for running benchmarks
    add_custom_target(run_benchmarks
        COMMAND benchmark_comprehensive
        DEPENDS benchmark_comprehensive
        COMMENT "Running comprehensive performance benchmarks"
    )
endif()

# ==================== Testing ====================

if(BUILD_TESTING)
    enable_testing()
    
    # Modern architecture tests
    add_executable(test_modern_architecture
        tests/test_modern_architecture.cpp
    )
    
    target_link_libraries(test_modern_architecture
        PRIVATE
            algebraic_hashing
            GTest::gtest_main
            GTest::gmock_main
    )
    
    # Register test with CTest
    add_test(
        NAME ModernArchitectureTests
        COMMAND test_modern_architecture
    )
    
    # Set test properties
    set_tests_properties(ModernArchitectureTests
        PROPERTIES
            TIMEOUT 60
            LABELS "unit;core"
    )
    
    # Comprehensive coverage tests
    add_executable(test_comprehensive_coverage
        tests/test_comprehensive_coverage.cpp
    )
    
    target_link_libraries(test_comprehensive_coverage
        PRIVATE
            algebraic_hashing
            GTest::gtest_main
            GTest::gmock_main
    )
    
    # Register comprehensive tests with CTest
    add_test(
        NAME ComprehensiveCoverageTests
        COMMAND test_comprehensive_coverage
    )
    
    # Set test properties
    set_tests_properties(ComprehensiveCoverageTests
        PROPERTIES
            TIMEOUT 120
            LABELS "unit;coverage;extensive"
    )
    
    # Performance benchmarks (separate from regular tests)
    add_executable(benchmarks
        tests/test_modern_architecture.cpp
    )
    
    target_compile_definitions(benchmarks
        PRIVATE
            BENCHMARKING_MODE=1
    )
    
    target_link_libraries(benchmarks
        PRIVATE
            algebraic_hashing
            GTest::gtest_main
    )
    
    # Custom test target for running all tests with verbose output
    add_custom_target(test_verbose
        COMMAND ${CMAKE_CTEST_COMMAND} --verbose --parallel ${CMAKE_BUILD_PARALLEL_LEVEL}
        DEPENDS test_modern_architecture test_comprehensive_coverage
        COMMENT "Running all tests with verbose output"
    )

    # ==================== Code Coverage ====================

    option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

    if(ENABLE_COVERAGE)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            message(STATUS "Code coverage enabled")

            # Add coverage flags to test targets
            target_compile_options(test_modern_architecture PRIVATE --coverage)
            target_link_options(test_modern_architecture PRIVATE --coverage)

            target_compile_options(test_comprehensive_coverage PRIVATE --coverage)
            target_link_options(test_comprehensive_coverage PRIVATE --coverage)

            # Find lcov and genhtml for coverage reporting
            find_program(LCOV lcov)
            find_program(GENHTML genhtml)

            if(LCOV AND GENHTML)
                # Custom target to generate coverage report
                add_custom_target(coverage
                    COMMAND ${CMAKE_COMMAND} -E echo "Running tests for coverage..."
                    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
                    COMMAND ${CMAKE_COMMAND} -E echo "Capturing coverage data..."
                    COMMAND ${LCOV} --capture --directory . --output-file coverage.info --ignore-errors mismatch,inconsistent
                    COMMAND ${LCOV} --remove coverage.info '/usr/*' '*/tests/*' '*/googletest/*' --output-file coverage_filtered.info --ignore-errors mismatch,inconsistent
                    COMMAND ${CMAKE_COMMAND} -E echo "Generating HTML report..."
                    COMMAND ${GENHTML} coverage_filtered.info --output-directory coverage_html --ignore-errors mismatch,inconsistent
                    COMMAND ${CMAKE_COMMAND} -E echo "Coverage report generated in coverage_html/index.html"
                    COMMAND ${CMAKE_COMMAND} -E echo "Coverage summary:"
                    COMMAND ${LCOV} --summary coverage_filtered.info || true
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                    DEPENDS test_modern_architecture test_comprehensive_coverage
                    COMMENT "Generating code coverage report"
                )

                # Target to open coverage report in browser
                add_custom_target(coverage_open
                    COMMAND xdg-open ${CMAKE_BINARY_DIR}/coverage_html/index.html || open ${CMAKE_BINARY_DIR}/coverage_html/index.html
                    DEPENDS coverage
                    COMMENT "Opening coverage report in browser"
                )
            else()
                message(WARNING "lcov and/or genhtml not found. Install with: sudo apt-get install lcov")
            endif()
        else()
            message(WARNING "Code coverage only supported with GCC or Clang")
        endif()
    endif()
endif()

# ==================== Documentation ====================

# Find Doxygen for documentation generation
find_package(Doxygen QUIET)
if(Doxygen_FOUND)
    # Configure Doxygen
    set(DOXYGEN_INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs)
    set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
    
    # Custom Doxygen configuration
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_MAN NO)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_CLASS_DIAGRAMS YES)
    set(DOXYGEN_HAVE_DOT NO)
    set(DOXYGEN_CLASS_GRAPH YES)
    set(DOXYGEN_COLLABORATION_GRAPH YES)
    set(DOXYGEN_UML_LOOK YES)
    set(DOXYGEN_UML_LIMIT_NUM_FIELDS 50)
    set(DOXYGEN_TEMPLATE_RELATIONS YES)
    set(DOXYGEN_DOT_GRAPH_MAX_NODES 100)
    set(DOXYGEN_MAX_DOT_GRAPH_DEPTH 0)
    set(DOXYGEN_DOT_TRANSPARENT YES)
    set(DOXYGEN_CALL_GRAPH YES)
    set(DOXYGEN_CALLER_GRAPH YES)
    set(DOXYGEN_EXTRACT_PRIVATE YES)
    set(DOXYGEN_EXTRACT_STATIC YES)
    set(DOXYGEN_SORT_MEMBER_DOCS NO)
    
    doxygen_add_docs(docs
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/examples
        COMMENT "Generating API documentation with Doxygen"
    )
    
    add_custom_target(docs_open
        COMMAND xdg-open ${DOXYGEN_INDEX_FILE}
        DEPENDS docs
        COMMENT "Opening documentation in web browser"
    )
else()
    message(STATUS "Doxygen not found, documentation target not available")
endif()

# ==================== Installation ====================

include(GNUInstallDirs)

# Install headers
install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# Install targets
install(
    TARGETS algebraic_hashing
    EXPORT AlgebraicHashingTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install examples
install(
    TARGETS algebraic_hashing_tutorial
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Create and install CMake config files
install(
    EXPORT AlgebraicHashingTargets
    FILE AlgebraicHashingTargets.cmake
    NAMESPACE AlgebraicHashing::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AlgebraicHashing
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/AlgebraicHashingConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/AlgebraicHashingConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AlgebraicHashing
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/AlgebraicHashingConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/AlgebraicHashingConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/AlgebraicHashingConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AlgebraicHashing
)

# ==================== Package Configuration ====================

set(CPACK_PACKAGE_NAME "AlgebraicHashing")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "Algebraic Hashing Project")
set(CPACK_PACKAGE_CONTACT "algebraic-hashing@example.com")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# Source package
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
    "/\\.git/"
    "/build/"
    "/\\.vscode/"
    "/\\.idea/"
    "\\.gitignore$"
    "\\.DS_Store$"
)

include(CPack)

# ==================== Development Targets ====================

# Format code using clang-format
find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp
    )
    
    add_custom_target(format
        COMMAND ${CLANG_FORMAT} -i ${ALL_CXX_SOURCE_FILES}
        COMMENT "Formatting source code with clang-format"
    )
endif()

# Static analysis with clang-tidy
find_program(CLANG_TIDY clang-tidy)
if(CLANG_TIDY)
    set_target_properties(algebraic_hashing_tutorial
        PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY};-checks=*,-fuchsia-default-arguments-calls,-modernize-use-trailing-return-type"
    )
endif()

# ==================== Status Messages ====================

message(STATUS "")
message(STATUS "==================== Configuration Summary ====================")
message(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
message(STATUS "Features:")
message(STATUS "  Testing: ${BUILD_TESTING}")
message(STATUS "  Examples: ${BUILD_EXAMPLES}")
message(STATUS "  Benchmarks: ${BUILD_BENCHMARKS}")
message(STATUS "  Concepts checking: ${ENABLE_CONCEPTS_CHECKING}")
message(STATUS "  Statistics: ${ENABLE_STATISTICS}")
message(STATUS "  Documentation: ${Doxygen_FOUND}")
message(STATUS "  Code formatting: ${CLANG_FORMAT}")
message(STATUS "  Static analysis: ${CLANG_TIDY}")
message(STATUS "")
message(STATUS "Targets available:")
if(BUILD_EXAMPLES)
    message(STATUS "  algebraic_hashing_tutorial - Educational tutorial")
endif()
if(BUILD_TESTING)
    message(STATUS "  test_modern_architecture - Comprehensive test suite")
    message(STATUS "  test_comprehensive_coverage - Coverage test suite")  
    message(STATUS "  test_verbose - Run tests with verbose output")
endif()
if(BUILD_BENCHMARKS)
    message(STATUS "  benchmark_comprehensive - Performance benchmarks")
    message(STATUS "  run_benchmarks - Execute benchmark suite")
endif()
if(Doxygen_FOUND)
    message(STATUS "  docs - Generate API documentation")
    message(STATUS "  docs_open - Generate and open documentation")
endif()
if(CLANG_FORMAT)
    message(STATUS "  format - Format source code")
endif()
message(STATUS "==============================================================")