#!/usr/bin/env python3
"""
Conan package recipe for AlgebraicHashing library.

This is a modern C++20 header-only library for algebraic hash function composition.
It provides an elegant DSL for combining hash functions and exploring their mathematical properties.
"""

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps, cmake_layout, CMake
from conan.tools.files import copy, save, load
from conan.tools.scm import Version
import os
import textwrap


class AlgebraicHashingConan(ConanFile):
    """
    Conan package for AlgebraicHashing - Modern C++ library for algebraic hash function composition.
    
    This header-only library provides:
    - Modern C++20 concepts for type-safe hash function composition
    - Elegant DSL for algebraic operations on hash functions  
    - Educational framework for understanding hash function mathematics
    - High-performance implementations with zero-cost abstractions
    """
    
    name = "algebraic_hashing"
    version = "2.0.0"
    
    # Package Metadata
    description = "Modern C++20 library for algebraic hash function composition"
    license = "MIT"
    author = "Algebraic Hashing Project"
    url = "https://github.com/queelius/algebraic_hashing"
    homepage = "https://github.com/queelius/algebraic_hashing"
    topics = ("hash-functions", "cryptography", "algorithms", "cpp20", "header-only", "mathematics")
    
    # Binary Configuration
    package_type = "header-library"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "build_tests": [True, False],
        "build_examples": [True, False],
        "build_benchmarks": [True, False],
        "enable_concepts_checking": [True, False],
        "enable_statistics": [True, False]
    }
    default_options = {
        "build_tests": False,
        "build_examples": True,
        "build_benchmarks": False,
        "enable_concepts_checking": True,
        "enable_statistics": True
    }
    
    # Exports
    exports = "LICENSE.md"
    exports_sources = (
        "CMakeLists.txt",
        "cmake/*",
        "include/*",
        "examples/*",
        "tests/*",
        "README.md",
        "LICENSE.md"
    )
    
    # No binary dependencies for header-only library
    # But we need build dependencies for testing
    def requirements(self):
        """Define runtime dependencies (none for header-only library)."""
        pass
    
    def build_requirements(self):
        """Define build-time dependencies."""
        if self.options.build_tests:
            self.test_requires("gtest/1.14.0")
    
    def layout(self):
        """Define the layout for the package."""
        cmake_layout(self)
    
    def validate(self):
        """Validate the configuration."""
        # Check C++20 support
        min_cpp_standard = 20
        if hasattr(self.settings, "compiler") and hasattr(self.settings.compiler, "cppstd"):
            if Version(str(self.settings.compiler.cppstd)) < min_cpp_standard:
                raise ConanInvalidConfiguration(f"AlgebraicHashing requires C++{min_cpp_standard}")
        
        # Check compiler versions for C++20 concepts support
        compiler = self.settings.compiler
        if compiler == "gcc":
            if Version(self.settings.compiler.version) < "10":
                raise ConanInvalidConfiguration("AlgebraicHashing requires GCC 10+ for C++20 concepts support")
        elif compiler == "clang":
            if Version(self.settings.compiler.version) < "12":
                raise ConanInvalidConfiguration("AlgebraicHashing requires Clang 12+ for C++20 concepts support")
        elif compiler == "Visual Studio":
            if Version(self.settings.compiler.version) < "16":
                raise ConanInvalidConfiguration("AlgebraicHashing requires Visual Studio 2019+ for C++20 concepts support")
        elif compiler == "msvc":
            if Version(self.settings.compiler.version) < "192":
                raise ConanInvalidConfiguration("AlgebraicHashing requires MSVC 19.2+ for C++20 concepts support")
    
    def generate(self):
        """Generate the necessary files for the build."""
        # Generate CMake toolchain
        tc = CMakeToolchain(self)
        
        # Set C++20 standard
        tc.variables["CMAKE_CXX_STANDARD"] = "20"
        tc.variables["CMAKE_CXX_STANDARD_REQUIRED"] = "ON"
        tc.variables["CMAKE_CXX_EXTENSIONS"] = "OFF"
        
        # Configure build options
        tc.variables["BUILD_TESTING"] = self.options.build_tests
        tc.variables["BUILD_EXAMPLES"] = self.options.build_examples
        tc.variables["BUILD_BENCHMARKS"] = self.options.build_benchmarks
        tc.variables["ENABLE_CONCEPTS_CHECKING"] = self.options.enable_concepts_checking
        tc.variables["ENABLE_STATISTICS"] = self.options.enable_statistics
        
        # Platform-specific optimizations
        if self.settings.build_type == "Release":
            tc.variables["CMAKE_INTERPROCEDURAL_OPTIMIZATION"] = "ON"  # Enable LTO
        
        tc.generate()
        
        # Generate CMake dependencies
        deps = CMakeDeps(self)
        deps.generate()
    
    def build(self):
        """Build the package (mostly validation and examples for header-only)."""
        cmake = CMake(self)
        cmake.configure()
        
        # Build examples to validate the library
        if self.options.build_examples:
            cmake.build(target="algebraic_hashing_tutorial")
        
        # Build and run tests if requested
        if self.options.build_tests:
            cmake.build(target="test_modern_architecture")
            cmake.build(target="test_comprehensive_coverage")
            
            # Run tests during build to validate the package
            if not self.conf.get("tools.build:skip_test", False):
                self.run(os.path.join(self.build_folder, "test_modern_architecture"))
                self.run(os.path.join(self.build_folder, "test_comprehensive_coverage"))
        
        # Build benchmarks if requested
        if self.options.build_benchmarks:
            cmake.build(target="benchmarks")
    
    def package(self):
        """Package the library files."""
        # Copy license
        copy(self, "LICENSE.md", dst=os.path.join(self.package_folder, "licenses"), src=self.source_folder)
        
        # Install using CMake
        cmake = CMake(self)
        cmake.install()
        
        # Copy additional documentation
        copy(self, "README.md", dst=os.path.join(self.package_folder, "docs"), src=self.source_folder)
        copy(self, "REFACTORING_SUMMARY.md", dst=os.path.join(self.package_folder, "docs"), src=self.source_folder)
        
        # Copy examples
        if self.options.build_examples:
            copy(self, "*.cpp", dst=os.path.join(self.package_folder, "examples"), 
                 src=os.path.join(self.source_folder, "examples"))
    
    def package_info(self):
        """Define how consumers should use this package."""
        # Header-only library - no compiled libraries to link
        self.cpp_info.libs = []
        
        # Set component name for modern CMake usage
        self.cpp_info.set_property("cmake_module_name", "AlgebraicHashing")
        self.cpp_info.set_property("cmake_target_name", "AlgebraicHashing::algebraic_hashing")
        self.cpp_info.set_property("cmake_file_name", "AlgebraicHashing")
        
        # Legacy CMake support
        self.cpp_info.names["cmake_find_package"] = "AlgebraicHashing"
        self.cpp_info.names["cmake_find_package_multi"] = "AlgebraicHashing"
        
        # Include directories
        self.cpp_info.includedirs = ["include"]
        
        # Required C++20 features
        self.cpp_info.cppstd = "20"
        
        # System libraries (threading)
        if self.settings.os in ["Linux", "FreeBSD"]:
            self.cpp_info.system_libs.append("pthread")
        
        # Compiler-specific flags
        if self.settings.compiler == "gcc" or self.settings.compiler == "clang":
            # Enable concepts and constexpr support
            self.cpp_info.cppflags.extend(["-fconcepts", "-fconstexpr-steps=1000000"])
            
            # Optimization flags for release builds
            if self.settings.build_type == "Release":
                self.cpp_info.cppflags.extend(["-O3", "-DNDEBUG"])
        
        # Preprocessor definitions based on options
        if self.options.enable_concepts_checking:
            self.cpp_info.defines.append("ALGEBRAIC_HASHING_ENABLE_CONCEPTS_CHECKING")
        
        if self.options.enable_statistics:
            self.cpp_info.defines.append("ALGEBRAIC_HASHING_ENABLE_STATISTICS")
        
        # Package metadata for tools
        self.cpp_info.set_property("component_version", self.version)
    
    def package_id(self):
        """Define what affects the package binary compatibility."""
        # Header-only library - package ID only depends on options that affect headers
        self.info.clear()
        # Include options that affect the interface
        self.info.options.enable_concepts_checking = self.options.enable_concepts_checking
        self.info.options.enable_statistics = self.options.enable_statistics


def ConanInvalidConfiguration(message):
    """Helper for invalid configuration exception."""
    class InvalidConfig(Exception):
        pass
    return InvalidConfig(message)