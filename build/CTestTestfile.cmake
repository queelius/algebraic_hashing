# CMake generated Testfile for 
# Source directory: /home/spinoza/github/released/algebraic_hashing
# Build directory: /home/spinoza/github/released/algebraic_hashing/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[ModernArchitectureTests]=] "/home/spinoza/github/released/algebraic_hashing/build/test_modern_architecture")
set_tests_properties([=[ModernArchitectureTests]=] PROPERTIES  LABELS "unit;core" TIMEOUT "60" _BACKTRACE_TRIPLES "/home/spinoza/github/released/algebraic_hashing/CMakeLists.txt;189;add_test;/home/spinoza/github/released/algebraic_hashing/CMakeLists.txt;0;")
add_test([=[ComprehensiveCoverageTests]=] "/home/spinoza/github/released/algebraic_hashing/build/test_comprehensive_coverage")
set_tests_properties([=[ComprehensiveCoverageTests]=] PROPERTIES  LABELS "unit;coverage;extensive" TIMEOUT "120" _BACKTRACE_TRIPLES "/home/spinoza/github/released/algebraic_hashing/CMakeLists.txt;214;add_test;/home/spinoza/github/released/algebraic_hashing/CMakeLists.txt;0;")
