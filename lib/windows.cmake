# Windows-specific configuration for FOX
# Based on Visual Studio project settings from windows/foxlib and windows/foxdll

# Windows-specific compile definitions
target_compile_definitions(FOX_XINCS INTERFACE
    UNICODE
    WIN32
    _CRT_SECURE_NO_WARNINGS
)

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
    Imm32       # Input Method Manager
    Msimg32     # Image manipulation
    Ws2_32      # Winsock 2 (networking)
)

# OpenGL support (always enabled on Windows in VS projects)
if(WITH_OPENGL)
    # Find OpenGL - Windows provides opengl32.lib and glu32.lib
    find_package(OpenGL REQUIRED)

    target_link_libraries(FOX_DEPS INTERFACE
        opengl32  # OpenGL
        glu32     # OpenGL Utility Library
    )

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
