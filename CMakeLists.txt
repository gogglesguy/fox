cmake_minimum_required(VERSION 3.3.1)

set(WITH_NATIVE OFF CACHE STRING "Compile for native architecture")

set(CMAKE_VERBOSE_MAKEFILE TRUE)

include(CheckCXXCompilerFlag)

check_cxx_compiler_flag(-Wall           HAS_CXX_WALL)
check_cxx_compiler_flag(-Wextra         HAS_CXX_WEXTRA)
check_cxx_compiler_flag(-Wformat=2      HAS_CXX_WFORMAT)
check_cxx_compiler_flag(-Wfatal-errors  HAS_CXX_WFATAL)
check_cxx_compiler_flag(-Og             HAS_CXX_OPTIMIZE_DEBUG)

if(WITH_NATIVE)
  check_cxx_compiler_flag(-march=native   HAS_CXX_OPTIMIZE_NATIVE)
endif()

if(HAS_CXX_WALL)
    add_compile_options(-Wall)
endif()

if(HAS_CXX_WEXTRA)
    add_compile_options(-Wextra)
endif()

if(HAS_CXX_WFORMAT)
    add_compile_options(-Wformat=2)
endif()

if(HAS_CXX_WFATAL)
    add_compile_options(-Wfatal-errors)
endif()

if(HAS_CXX_OPTIMIZE_NATIVE AND WITH_NATIVE)
    message(STATUS "Native compilation enabled")
    add_compile_options(-march=native)
endif()

# Libraries
add_subdirectory(utils)
add_subdirectory(lib)
add_subdirectory(chart)

# Tests
add_subdirectory(tests)
add_subdirectory(glviewer)

# Programs
add_subdirectory(adie)
add_subdirectory(calculator)
add_subdirectory(controlpanel)
add_subdirectory(pathfinder)
add_subdirectory(shutterbug)


