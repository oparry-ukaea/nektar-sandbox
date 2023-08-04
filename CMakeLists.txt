cmake_minimum_required(VERSION 3.21)

project(
  NEKTAR_SANDBOX
  VERSION 0.0.1
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Set a default install location
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX
      "${CMAKE_BINARY_DIR}/dist"
      CACHE PATH "Default install path" FORCE)
  message("Setting install location to ${CMAKE_INSTALL_PREFIX}")
else()
  message("Install location already set to ${CMAKE_INSTALL_PREFIX}")
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# Set runpath(s) same as link path(s)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Options to build different apps/tests
option(HELMSOLVE_TESTS "Build tests for Helmsolve" ON)

# Various sanitizers, including coverage and address sanitizer
include(cmake/Sanitizers.cmake)

# ##############################################################################
# Find dependencies
# ##############################################################################
# Use a custom FindNektar++ script to provide an interface target
find_package(Nektar++ REQUIRED)
if(NOT TARGET Nektar++::nektar++)
  message(FATAL_ERROR "Can't continue without Nektar")
endif()
# Find MPI
find_package(MPI REQUIRED)

# ##############################################################################
# Set the default build type (Options are Debug, Release, RelWithDebInfo and
# MinSizeRel)
# ##############################################################################
set(CMAKE_BUILD_TYPE
    Release
    CACHE STRING "CMake build type.")

# ##############################################################################
# Set compile flags for each build type
# ##############################################################################
# Defaults
set(OPT_FLAG "-O2")
set(DBSYM_FLAG "-g")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  # Turn off optimisation in Debug
  set(OPT_FLAG "-O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  # Turn off debug symbols in Release
  set(DBSYM_FLAG "")
endif()
set(BUILD_TYPE_COMPILE_FLAGS "${DBSYM_FLAG};${OPT_FLAG}")

# ##############################################################################
# Add subdirectories for all apps/tests that are enabled
# ##############################################################################
if(HELMSOLVE_TESTS)
  add_subdirectory(helmsolve_tests)
endif()
