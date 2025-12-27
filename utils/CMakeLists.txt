include(GNUInstallDirs)

# When cross-compiling, we need a native reswrap that can run on the build machine
# Users should either:
#   1. Set RESWRAP_EXECUTABLE to point to a pre-built native reswrap
#   2. Or build reswrap natively first, then use it for cross-compilation
if(CMAKE_CROSSCOMPILING)
  # Allow user to specify path to native reswrap
  set(RESWRAP_EXECUTABLE "" CACHE FILEPATH "Path to native reswrap executable for cross-compilation")

  if(NOT RESWRAP_EXECUTABLE)
    message(FATAL_ERROR
      "Cross-compiling requires a native reswrap executable.\n"
      "Please either:\n"
      "  1. Build reswrap natively first: cmake -B build-native && cmake --build build-native --target reswrap\n"
      "  2. Set RESWRAP_EXECUTABLE to point to the native reswrap, e.g.:\n"
      "     -DRESWRAP_EXECUTABLE=/path/to/native/reswrap\n"
    )
  endif()

  # Create an imported executable target
  add_executable(reswrap IMPORTED)
  set_target_properties(reswrap PROPERTIES IMPORTED_LOCATION "${RESWRAP_EXECUTABLE}")

  message(STATUS "Using native reswrap for cross-compilation: ${RESWRAP_EXECUTABLE}")
else()
  # Normal build: compile reswrap for the target platform
  add_executable(reswrap reswrap.cpp)
  install(TARGETS reswrap RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# Set up reswrap command and dependency for use in custom commands
# These variables are exported to parent scope for use by applications and library
if(CMAKE_CROSSCOMPILING)
  set(RESWRAP_CMD "${RESWRAP_EXECUTABLE}" PARENT_SCOPE)
  set(RESWRAP_DEPENDS "${RESWRAP_EXECUTABLE}" PARENT_SCOPE)
else()
  set(RESWRAP_CMD $<TARGET_FILE:reswrap> PARENT_SCOPE)
  set(RESWRAP_DEPENDS reswrap PARENT_SCOPE)
endif()
