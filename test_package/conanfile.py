#!/usr/bin/env python3
"""
Test package for AlgebraicHashing library.

This validates that the package can be properly consumed by Conan users.
"""

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps, CMake, cmake_layout
from conan.tools.build import can_run


class AlgebraicHashingTestConan(ConanFile):
    """Test package to validate AlgebraicHashing can be consumed properly."""
    
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    
    def layout(self):
        cmake_layout(self)
    
    def requirements(self):
        """Reference the package being tested."""
        self.requires(self.tested_reference_str)
    
    def build(self):
        """Build the test consumer application."""
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
    
    def test(self):
        """Run the test to validate the package works."""
        if can_run(self):
            cmd = os.path.join(self.cpp.build.bindirs[0], "test_algebraic_hashing")
            self.run(cmd, env="conanrun")