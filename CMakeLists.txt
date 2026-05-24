cmake_minimum_required(VERSION 4.0)

project(FOX VERSION 1.7.90 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

option(WITH_NATIVE "Compile for native architecture" OFF)

# Standard CMake option for shared/static libraries
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_STATIC_PIC "Build static libraries with Position Independent Code" ON)

# Build components
option(FOX_BUILD_APPS "Build FOX applications (adie, pathfinder, etc.)" ON)
option(FOX_BUILD_TESTS "Build FOX test programs" ON)
option(FOX_BUILD_CHART "Build FOX chart library" ON)

if(BUILD_SHARED_LIBS)
  message(STATUS "Building FOX as SHARED library")
else()
  message(STATUS "Building FOX as STATIC library (Position Independent Code: ${BUILD_STATIC_PIC})")
  set(CMAKE_POSITION_INDEPENDENT_CODE ${BUILD_STATIC_PIC})
endif()


# Set project-wide compiler flags
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerFlags)


# Core libraries (always built)
add_subdirectory(utils)
add_subdirectory(lib)

# Optional chart library
if(FOX_BUILD_CHART)
  add_subdirectory(chart)
endif()

# Optional tests
if(FOX_BUILD_TESTS)
  add_subdirectory(tests)
  add_subdirectory(glviewer)
endif()

# Optional applications
if(FOX_BUILD_APPS)
  add_subdirectory(adie)
  add_subdirectory(calculator)
  add_subdirectory(controlpanel)
  add_subdirectory(pathfinder)
  add_subdirectory(shutterbug)
endif()
