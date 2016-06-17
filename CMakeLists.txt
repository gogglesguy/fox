cmake_minimum_required(VERSION 3.3.1)

set(CMAKE_VERBOSE_MAKEFILE TRUE)

include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG(-Wall       HAS_CXX_WALL)
CHECK_CXX_COMPILER_FLAG(-Wextra     HAS_CXX_WEXTRA)
CHECK_CXX_COMPILER_FLAG(-Wformat=2  HAS_CXX_WFORMAT)
CHECK_CXX_COMPILER_FLAG(-Wfatal-errors  HAS_CXX_WFATAL)
CHECK_CXX_COMPILER_FLAG(-Og         HAS_CXX_OPTIMIZE_DEBUG)

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


add_subdirectory(utils)
add_subdirectory(lib)
add_subdirectory(tests)

