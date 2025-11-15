
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was AlgebraicHashingConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

# AlgebraicHashing CMake Configuration File
# This file defines the AlgebraicHashing::algebraic_hashing target

include(CMakeFindDependencyMacro)

# Find required dependencies
find_dependency(Threads)

# Check required C++ features
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()

if(CMAKE_CXX_STANDARD LESS 20)
    message(FATAL_ERROR "AlgebraicHashing requires C++20 or later")
endif()

# Include the target file
include("${CMAKE_CURRENT_LIST_DIR}/AlgebraicHashingTargets.cmake")

# Verify that the target is available
check_required_components(AlgebraicHashing)

# Set useful variables
set(AlgebraicHashing_VERSION 2.0.0)
set(AlgebraicHashing_VERSION_MAJOR 2)
set(AlgebraicHashing_VERSION_MINOR 0)
set(AlgebraicHashing_VERSION_PATCH 0)

# Print status message
message(STATUS "Found AlgebraicHashing: ${AlgebraicHashing_VERSION}")
message(STATUS "  - Header-only C++20 library for algebraic hash function composition")
message(STATUS "  - Provides concepts, DSL, and modern hash function implementations")
