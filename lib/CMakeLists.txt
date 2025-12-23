include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)
include(CheckCXXSymbolExists)
include(CMakePushCheckState)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

option(WITH_JPEG "Compile with JPEG support" ON)
option(WITH_TIFF "Compile with TIFF support" ON)
option(WITH_ZLIB "Compile with zlib support" ON)
option(WITH_BZ2LIB "Compile with bz2lib support" ON)
option(WITH_WEBP "Compile with webp support" ON)
option(WITH_OPENJPEG "Compile with OpenJPEG support" ON)
option(WITH_OPENGL "Compile with OpenGL support" ON)
option(WITH_PERFORMANCE_LOGGING "Compile with Performance Logging" OFF)
option(WITH_CUPS "Compile with Cups support" ON)

# GNUInstallDirs provides: CMAKE_INSTALL_BINDIR, CMAKE_INSTALL_LIBDIR, CMAKE_INSTALL_INCLUDEDIR, etc.
# Automatically handles lib vs lib64, multiarch paths (/usr/lib/x86_64-linux-gnu), etc.
set(CMAKE_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/fox-${FOX_VERSION_MAJOR}.${FOX_VERSION_MINOR}" CACHE STRING "Installation directory for CMake files")


set(FOX_HEADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include")

# Include source and header file lists
include(headers.cmake)
include(sources.cmake)

# Rename FOX_SOURCES to SOURCES for compatibility with rest of file
set(SOURCES ${FOX_SOURCES})


set(ICONS
arrownext.gif
arrowprev.gif
bigapp.gif
bigcdrom.gif
bigcomputer.gif
bigdesktop.gif
bigdoc.gif
bigfloppy.gif
bigfloppy3.gif
bigfloppy5.gif
bigfolder.gif
bigfolderopen.gif
bigharddisk.gif
bignetdrive.gif
bignethood.gif
bookclr_gif.gif
bookdel_gif.gif
bookmrk_gif.gif
bookset_gif.gif
clearentry.gif
cmymode.gif
dialmode.gif
dirup_gif.gif
docktop.gif
dockbottom.gif
dockleft.gif
dockright.gif
dockfree.gif
dockflip.gif
entericon.gif
erroricon.gif
eyedrop.gif
filecancel.gif
fileaccept.gif
filecopy.gif
filemove.gif
filehidden.gif
filelink.gif
filerename.gif
filedelete.gif
fileshown.gif
foldernew.gif
hammer_gif.gif
gotohome.gif
gotowork.gif
hsvmode.gif
infoicon.gif
landscape.gif
listmode.gif
miniapp.gif
minicdrom.gif
minicomputer.gif
minidesktop.gif
minidoc.gif
minifloppy.gif
minifloppy3.gif
minifloppy5.gif
minifolder.gif
minifolderopen.gif
miniharddisk.gif
mininetdrive.gif
mininethood.gif
minizipdrive.gif
newfolder.bmp
portrait.gif
questionicon.gif
redo_gif.gif
rgbmode.gif
searchicon.gif
showbigicons.gif
showdetails.gif
showsmallicons.gif
sorting.bmp
undo_gif.gif
warningicon.gif
winclose.gif
winmaximize.gif
winminimize.gif
winrestore.gif)


# Private Dependencies interface library
add_library(FOX_DEPS INTERFACE)

# X11 and platform-specific interface library
# Generally, all HAVE_* definitions exposed through xincs.h should be added to FOX_XINCS.
# Programs that use this header should add this interface library to receive all the correct defines
add_library(FOX_XINCS INTERFACE)

# Main FOX library - type determined by BUILD_SHARED_LIBS
add_library(FOX ${SOURCES})

# Set common properties
set_target_properties(FOX PROPERTIES
  VERSION ${FOX_VERSION}
  SOVERSION ${FOX_VERSION_MAJOR}
  OUTPUT_NAME "FOX-${FOX_VERSION_MAJOR}.${FOX_VERSION_MINOR}"
  EXPORT_NAME FOX
)

# Shared library specific properties
if(BUILD_SHARED_LIBS)
  target_compile_definitions(FOX PUBLIC FOXDLL)
  set_target_properties(FOX PROPERTIES DEFINE_SYMBOL "FOXDLL_EXPORTS")
endif()

# Link dependencies
# FOX_DEPS must be PUBLIC for both static and shared builds:
# - Static: .a file contains unresolved symbols, consumers need dependencies on link line
# - Shared: Consumers may directly use dependency symbols (e.g., bitmapviewer uses TIFF)
# FOX_XINCS is PRIVATE so apps must explicitly opt-in via FX::FOX_XINCS
target_link_libraries(FOX PUBLIC FOX_DEPS PRIVATE FOX_XINCS)

# Add headers to the target
target_sources(FOX PUBLIC
              FILE_SET HEADERS
              BASE_DIRS ../include
              FILES ${FOX_HEADERS})

# Create namespaced aliases for use in build tree
add_library(FX::FOX ALIAS FOX)
add_library(FX::FOX_XINCS ALIAS FOX_XINCS)
add_library(FX::FOX_DEPS ALIAS FOX_DEPS)


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  include(windows.cmake)
else()
  include(unix.cmake)
endif()




if(WITH_PERFORMANCE_LOGGING)
  target_compile_definitions(FOX PUBLIC PERFORMANCE_LOGGING=1)
endif()


set(FOX_INCLUDE_DIR  "${CMAKE_INSTALL_INCLUDEDIR}/fox-${FOX_VERSION_MAJOR}.${FOX_VERSION_MINOR}")


# target_include_directories(FOX INTERFACE $<BUILD_INTERFACE:${FOX_HEADERS_DIR}> $<INSTALL_INTERFACE:${FOX_INCLUDE_DIR}>)
# target_link_libraries(FOX PRIVATE FOX_XINCS ${X11_LIBRARIES} ${LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp ${CMAKE_CURRENT_BINARY_DIR}/icons.h
  COMMAND reswrap -nc -h -n FX -o ${CMAKE_CURRENT_BINARY_DIR}/icons.h ${ICONS}
  COMMAND reswrap -nc -i ${CMAKE_CURRENT_BINARY_DIR}/icons.h -n FX -o ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp ${ICONS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS reswrap ${ICONS}
  VERBATIM
  )

# Version specific include directory



# Install targets
# Note: FOX_DEPS and FOX_XINCS must be exported because FOX links to them as PUBLIC/PRIVATE
#       fox-config.cmake finds all dependencies before loading targets to ensure they exist
install(TARGETS FOX FOX_DEPS FOX_XINCS
        EXPORT FOX_TARGETS
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Applications
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
        FILE_SET HEADERS DESTINATION ${FOX_INCLUDE_DIR} COMPONENT Development)


install(EXPORT FOX_TARGETS
        FILE fox-${FOX_VERSION_MAJOR}.${FOX_VERSION_MINOR}-targets.cmake
        NAMESPACE FX::
        DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
        COMPONENT Development)

# Export for use from build tree (allows using FOX without installing)
export(EXPORT FOX_TARGETS
       FILE "${CMAKE_CURRENT_BINARY_DIR}/fox-${FOX_VERSION_MAJOR}.${FOX_VERSION_MINOR}-targets.cmake"
       NAMESPACE FX::
)

# Register package in user's package registry (optional, allows find_package to find build tree)
export(PACKAGE FOX)


set(FOX_VERSION_FILENAME "${CMAKE_CURRENT_BINARY_DIR}/fox-config-version.cmake")
set(FOX_CONFIG_FILENAME  "${CMAKE_CURRENT_BINARY_DIR}/fox-config.cmake")


configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/fox-config.cmake.in" # Path to your template
    "${FOX_CONFIG_FILENAME}" # Output path in build dir
    INSTALL_DESTINATION "${CMAKE_INSTALL_CMAKEDIR}"   # Destination: lib/cmake/fox-1.7
    PATH_VARS FOX_INCLUDE_DIR # Makes FOX_INCLUDE_DIR relocatable
)

write_basic_package_version_file(
  ${FOX_VERSION_FILENAME}
  VERSION ${FOX_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  ${FOX_VERSION_FILENAME} DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
  COMPONENT Development
)

install(FILES
  ${FOX_CONFIG_FILENAME} DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
  COMPONENT Development
)


# install(FILES ${HEADERS} DESTINATION include/fox-${FOX_VERSION_MAJOR}.${FOX_VERSION_MINOR})
