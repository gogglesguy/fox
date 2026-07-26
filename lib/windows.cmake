# Windows-specific configuration for FOX
# Based on Visual Studio project settings from windows/foxlib and windows/foxdll

# Windows-specific compile definitions
# WIN32 must be PUBLIC because it affects ABI (FXID type, vtable layouts)
# UNICODE and _CRT_SECURE_NO_WARNINGS are also ABI-affecting
target_compile_definitions(FOX PUBLIC
    UNICODE
    WIN32
    _CRT_SECURE_NO_WARNINGS
)

# MSVC-specific configuration
if(MSVC)
  # MSVC Runtime Library selection
  # Default to dynamic runtime (/MD or /MDd) to match typical Windows DLL usage
  # Users can override with -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded for static runtime
  if(NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" CACHE STRING "MSVC runtime library")
  endif()

  # FOX applications use main() entry point, not WinMain()
  # Set console subsystem and correct entry point for all executables linking to FOX
  # These options are ignored for library targets, only applied to executables
  target_link_options(FOX INTERFACE
    /SUBSYSTEM:CONSOLE
    /ENTRY:mainCRTStartup
  )
endif()

# Note: BUILD_WINDOWS was in VS projects but appears unused in source code
# Omitting it unless Windows developers report issues

# For shared library builds, add FOXDLL definitions
if(BUILD_SHARED_LIBS)
    target_compile_definitions(FOX PUBLIC FOXDLL)
    target_compile_definitions(FOX PRIVATE FOXDLL_EXPORTS)
endif()

# Windows system libraries
# These are the core Win32 API libraries needed by FOX
target_link_libraries(FOX_DEPS INTERFACE
    kernel32    # Core Windows kernel functions
    user32      # User interface functions
    gdi32       # Graphics Device Interface
    winspool    # Print spooler
    comdlg32    # Common dialogs (File Open, Save, etc.)
    advapi32    # Advanced Windows API (registry, etc.)
    shell32     # Shell API
    uuid        # UUID functions
    odbc32      # ODBC database
    odbccp32    # ODBC control panel
    imm32       # Input Method Manager
    msimg32     # Image manipulation
    ws2_32      # Winsock 2 (networking)
)

# OpenGL support (always enabled on Windows in VS projects)
if(WITH_OPENGL)
    # Find OpenGL - Windows provides opengl32.lib and glu32.lib
    find_package(OpenGL REQUIRED)

    target_link_libraries(FOX_DEPS INTERFACE
        opengl32  # OpenGL
    )

    # Note, FOX itself never calls any glu* functions so it does not need to link to it
    # But fx3d does have a HAVE_GLU_H macro so we add it here to the interface.
    target_compile_definitions(FOX INTERFACE
        HAVE_GL_H
        HAVE_GLU_H
    )
endif()

# Track Windows dependencies for fox-config.cmake
# Windows system libraries don't need find_dependency() - they're always available
# Only OpenGL needs to be found by consumers
set(FOX_LINKED_DEPENDENCIES "" CACHE INTERNAL "List of dependencies linked to FOX_DEPS")

if(WITH_OPENGL)
    list(APPEND FOX_LINKED_DEPENDENCIES "OpenGL")
endif()

# Note: Image libraries (JPEG, PNG, TIFF, WEBP, etc.) would need to be added
# similar to unix.cmake if/when Windows CMake builds support them.
# The VS projects don't show explicit image library linking, so they may be
# statically linked or not included in the base FOX library on Windows.
