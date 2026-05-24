include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)
include(CheckCXXSymbolExists)
include(CMakePushCheckState)

# Unix/X11-specific option
option(WITH_XIM "Compile with X Input Method support" ON)

find_package(PkgConfig)
find_package(Threads)
find_package(X11)
find_package(Freetype)
pkg_check_modules(FONTCONFIG fontconfig)





if(FREETYPE_FOUND)
  target_include_directories(FOX_XINCS INTERFACE ${FREETYPE_INCLUDE_DIRS})
  # include_directories(${FREETYPE_INCLUDE_DIRS})
endif()

if(X11_XShm_FOUND)
   target_compile_definitions(FOX_XINCS INTERFACE HAVE_XSHM_H)
endif()

if(X11_Xcursor_FOUND)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_XCURSOR_H)
  target_link_libraries(FOX_DEPS INTERFACE ${X11_Xcursor_LIB})
endif()

if(X11_Xft_FOUND AND FREETYPE_FOUND AND FONTCONFIG_FOUND)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_XFT_H)
  target_include_directories(FOX_XINCS INTERFACE ${X11_Xft_INCLUDE_PATH})
  target_link_libraries(FOX_DEPS INTERFACE ${X11_Xft_LIB} ${FONTCONFIG_LIBRARIES} ${FREETYPE_LIBRARIES})
endif()

if(X11_Xshape_FOUND)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_XSHAPE_H)
  target_link_libraries(FOX_DEPS INTERFACE ${X11_Xshape_LIB})
endif()

if(X11_Xrandr_FOUND)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_XRANDR_H)
  target_link_libraries(FOX_DEPS INTERFACE ${X11_Xrandr_LIB})
endif()

if(X11_Xfixes_FOUND)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_XFIXES_H)
  target_link_libraries(FOX_DEPS INTERFACE ${X11_Xfixes_LIB})
endif()

if(X11_Xrender_FOUND)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_XRENDER_H)
  target_link_libraries(FOX_DEPS INTERFACE ${X11_Xrender_LIB})
endif()

# check for xinput2
find_path(X11_Xi2_INCLUDE_PATH X11/extensions/XInput2.h ${X11_INC_SEARCH_PATH})
if (X11_Xi2_INCLUDE_PATH AND X11_Xi_LIB)
  set(X11_INCLUDE_DIR ${X11_INCLUDE_DIR} ${X11_Xi2_INCLUDE_PATH})
  list(REMOVE_DUPLICATES X11_INCLUDE_DIR)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_XINPUT2_H)
  target_link_libraries(FOX_DEPS INTERFACE ${X11_Xi_LIB})
endif ()


# X11
target_link_libraries(FOX_DEPS INTERFACE ${X11_LIBRARIES})



check_cxx_source_compiles( "#include <immintrin.h>
int main(int argc,char *argv) { return 0; }" HAVE_IMMINTRIN_H)
if(HAVE_IMMINTRIN_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_IMMINTRIN_H)
endif()


check_cxx_symbol_exists(localtime_r "time.h" HAVE_LOCALTIME_R)
if(HAVE_LOCALTIME_R)
  target_compile_definitions(FOX PRIVATE HAVE_LOCALTIME_R)
endif()

check_cxx_symbol_exists(clock_gettime "time.h" HAVE_CLOCK_GETTIME_NORT)
if(NOT HAVE_CLOCK_GETTIME_NORT)
  cmake_push_check_state()
  set(CMAKE_REQUIRED_LIBRARIES -lrt)
  check_cxx_symbol_exists(clock_gettime "time.h" HAVE_CLOCK_GETTIME)
  if(HAVE_CLOCK_GETTIME)
    list(APPEND LIBRARIES -lrt)
  endif()
  cmake_pop_check_state()
endif()

cmake_push_check_state()
  check_include_file_cxx(dlfcn.h HAVE_DLFCN_H)
  if(HAVE_DLFCN_H)
    target_compile_definitions(FOX_XINCS INTERFACE HAVE_DLFCN_H)
  endif()
  set(CMAKE_REQUIRED_LIBRARIES -ldl)
  check_cxx_symbol_exists(dlopen "dlfcn.h" HAVE_DLOPEN)
  if(HAVE_DLOPEN)
    list(APPEND LIBRARIES -ldl)
  endif()
cmake_pop_check_state()


cmake_push_check_state()
set(CMAKE_REQUIRED_LIBRARIES -ldld)
check_cxx_symbol_exists(shl_load "" HAVE_SHL_LOAD)
if(HAVE_DLOPEN_NODL)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_SHL_LOAD)
  list(APPEND LIBRARIES -ldld)
endif()
cmake_pop_check_state()

check_cxx_symbol_exists(gmtime_r "time.h" HAVE_GMTIME_R)
if(HAVE_GMTIME_R)
  target_compile_definitions(FOX PRIVATE HAVE_GMTIME_R)
endif()

# Not used anymore
# check_cxx_symbol_exists(readdir_r "dirent.h" HAVE_READDIR_R)
# if(HAVE_READDIR_R)
#   add_definitions(-DHAVE_READDIR_R)
# endif()

check_cxx_symbol_exists(getpwuid_r "sys/types.h;pwd.h" HAVE_GETPWUID_R)
if(HAVE_GETPWUID_R)
  target_compile_definitions(FOX PRIVATE HAVE_GETPWUID_R)
endif()

check_cxx_symbol_exists(getpwnam_r "sys/types.h;pwd.h" HAVE_GETPWNAM_R)
if(HAVE_GETPWNAM_R)
  target_compile_definitions(FOX PRIVATE HAVE_GETPWNAM_R)
endif()

check_cxx_symbol_exists(getgrgid_r "sys/types.h;grp.h" HAVE_GETGRGID_R)
if(HAVE_GETGRGID_R)
  target_compile_definitions(FOX PRIVATE HAVE_GETGRGID_R)
endif()

check_cxx_symbol_exists(uname "sys/utsname.h" HAVE_SYS_UTSNAME_H)
if(HAVE_SYS_UTSNAME_H)
  target_compile_definitions(FOX PRIVATE HAVE_SYS_UTSNAME_H)
endif()


cmake_push_check_state()
  set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
  set(CMAKE_REQUIRED_LIBRARIES -pthread)
  set(CMAKE_REQUIRED_FLAGS -pthread)
  check_cxx_symbol_exists(pthread_setaffinity_np "pthread.h" HAVE_PTHREAD_SETAFFINITY_NP)
  if(HAVE_PTHREAD_SETAFFINITY_NP)
    target_compile_definitions(FOX_XINCS INTERFACE HAVE_PTHREAD_SETAFFINITY_NP)
  endif()
cmake_pop_check_state()

cmake_push_check_state()
  set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
  check_cxx_symbol_exists(pipe2 "fcntl.h;unistd.h" HAVE_PIPE2)
  if(HAVE_PIPE2)
    target_compile_definitions(FOX PRIVATE HAVE_PIPE2)
  endif()
cmake_pop_check_state()

check_cxx_symbol_exists(sched_getcpu "sched.h" HAVE_SCHED_GETCPU)
if(HAVE_SCHED_GETCPU)
  target_compile_definitions(FOX PRIVATE HAVE_SCHED_GETCPU)
endif()

check_cxx_symbol_exists(epoll_create1 "sys/epoll.h" HAVE_EPOLL_CREATE1)
if(HAVE_EPOLL_CREATE1)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_EPOLL_CREATE1)
endif()

# Definition not used but header is in xincs
check_cxx_symbol_exists(timerfd_create "sys/timerfd.h" HAVE_TIMERFD_CREATE)
if(HAVE_TIMERFD_CREATE)
  target_compile_definitions(FOX PRIVATE HAVE_TIMERFD_CREATE)
endif()

check_cxx_symbol_exists(inotify_init1 "sys/inotify.h" HAVE_INOTIFY_INIT1)
if(HAVE_INOTIFY_INIT1)
  target_compile_definitions(FOX PRIVATE HAVE_INOTIFY_INIT1)
endif()

check_cxx_symbol_exists(statvfs "sys/statvfs.h" HAVE_STATVFS)
if(HAVE_STATVFS)
  target_compile_definitions(FOX PRIVATE HAVE_STATVFS)
endif()



#
# AC_HEADER_DIRENT
# This macro is obsolescent, as all current systems with directory libraries have <dirent.h>.
# New programs need not use this macro.
#
check_cxx_source_compiles("#include <dirent.h>\nint main(int argc,char *argv[]) { DIR * d; return 0; }" HAVE_DIRENT_H)
if(HAVE_DIRENT_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_DIRENT_H)
endif()

#
# AC_HEADER_TIME
#
# This macro is obsolescent, as current systems can include both files when they exist.
# New programs need not use this macro.
#
check_include_file_cxx(sys/time.h HAVE_SYS_TIME_H)
if(HAVE_SYS_TIME_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_SYS_TIME_H)
  check_cxx_source_compiles("#include <sys/time.h>\n#include <time.h>\nint main(int arc,char * argv[]) { return 0; }" TIME_WITH_SYS_TIME)
  if(TIME_WITH_SYS_TIME)
    target_compile_definitions(FOX_XINCS INTERFACE TIME_WITH_SYS_TIME)
  endif()
endif()




#
# AC_HEADER_SYS_WAIT
#
# If sys/wait.h is not Posix compatible, then instead of including it, define the Posix macros with their usual interpretations.
# This macro is obsolescent, as current systems are compatible with Posix. New programs need not use this macro.
# New programs need not use this macro.
#
# cmake: only check for header file since FOX doesn't define the alternative macros
check_include_file_cxx(sys/wait.h HAVE_SYS_WAIT_H)
if(HAVE_SYS_WAIT_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_SYS_WAIT_H)
endif()

check_include_file_cxx(semaphore.h HAVE_SEMAPHORE_H)
if(HAVE_SEMAPHORE_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_SEMAPHORE_H)
endif()

check_include_file_cxx(unistd.h HAVE_UNISTD_H)
if(HAVE_UNISTD_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_UNISTD_H)
endif()

check_include_file_cxx(sys/dir.h HAVE_SYS_DIR_H)
if(HAVE_SYS_DIR_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_SYS_DIR_H)
endif()

check_include_file_cxx(sys/filio.h HAVE_SYS_FILIO_H)
if(HAVE_SYS_FILIO_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_SYS_FILIO_H)
endif()

check_include_file_cxx(sys/mman.h HAVE_SYS_MMAN_H)
if(HAVE_SYS_MMAN_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_SYS_MMAN_H)
endif()

check_include_file_cxx(sys/mount.h HAVE_SYS_MOUNT_H)
if(HAVE_SYS_MOUNT_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_SYS_MOUNT_H)
endif()

check_include_file_cxx(sys/param.h HAVE_SYS_PARAM_H)
if(HAVE_SYS_PARAM_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_SYS_PARAM_H)
endif()

check_include_file_cxx(sys/select.h HAVE_SYS_SELECT_H)
if(HAVE_SYS_SELECT_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_SYS_SELECT_H)
endif()

check_include_file_cxx(sys/epoll.h HAVE_SYS_EPOLL_H)
if(HAVE_SYS_EPOLL_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_SYS_EPOLL_H)
endif()

check_include_file_cxx(sys/shm.h HAVE_SYS_SHM_H)
if(HAVE_SYS_SHM_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_SYS_SHM_H)
endif()

check_include_file_cxx(sys/statvfs.h HAVE_SYS_STATVFS_H)
if(HAVE_SYS_STATVFS_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_SYS_STATVFS_H)
endif()

check_include_file_cxx(sys/pstat.h HAVE_SYS_PSTAT_H)
if(HAVE_SYS_PSTAT_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_SYS_PSTAT_H)
endif()

check_include_file_cxx(sys/inotify.h HAVE_SYS_INOTIFY_H)
if(HAVE_SYS_INOTIFY_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_SYS_INOTIFY_H)
endif()

# For applications that want to use resource monitoring (getrusage, getrlimit, etc.)
check_include_file_cxx(sys/resource.h HAVE_SYS_RESOURCE_H)
if(HAVE_SYS_RESOURCE_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_SYS_RESOURCE_H)
endif()

# Used by FOX for CPU detection on BSD/macOS (FXThread::processors)
check_include_file_cxx(sys/sysctl.h HAVE_SYS_SYSCTL_H)
if(HAVE_SYS_SYSCTL_H)
  target_compile_definitions(FOX_XINCS INTERFACE HAVE_SYS_SYSCTL_H)
endif()

# For timerfd API (Linux timer file descriptors) - not used by FOX yet, just TODO comments
# check_include_file_cxx(sys/timerfd.h HAVE_SYS_TIMERFD_H)
# if(HAVE_SYS_TIMERFD_H)
#   target_compile_definitions(FOX_XINCS INTERFACE HAVE_SYS_TIMERFD_H)
# endif()

# Afaik not needed as XSHM already includes it
# check_include_file_cxx(sys/ipc.h HAVE_SYS_IPC_H)
# if(HAVE_SYS_IPC_H)
#   target_compile_definitions(FOX_XINCS INTERFACE HAVE_SYS_IPC_H)
# endif()

if(WITH_CUPS)
  find_package(Cups)
  if(Cups_FOUND)
    target_link_libraries(FOX_DEPS INTERFACE Cups::Cups)
    target_compile_definitions(FOX PUBLIC HAVE_CUPS_H)
  endif()
endif()

if(WITH_JPEG)
  find_package(JPEG)
  if(JPEG_FOUND)
    target_link_libraries(FOX_DEPS INTERFACE ${JPEG_LIBRARIES})
    target_compile_definitions(FOX PUBLIC HAVE_JPEG_H)
  endif()
endif()

if(WITH_ZLIB)
  find_package(ZLIB)
  if(TARGET ZLIB::ZLIB)
    target_link_libraries(FOX_DEPS INTERFACE ZLIB::ZLIB)
    target_compile_definitions(FOX PUBLIC HAVE_ZLIB_H)
  endif()
endif()

if(WITH_BZ2LIB)
  find_package(BZip2)
  if(TARGET BZip2::BZip2)
    target_link_libraries(FOX_DEPS INTERFACE BZip2::BZip2)
    target_compile_definitions(FOX PUBLIC HAVE_BZ2LIB_H)
  endif()
endif()

if(WITH_TIFF)
  find_package(TIFF)
  if(TARGET TIFF::TIFF)
    target_link_libraries(FOX_DEPS INTERFACE TIFF::TIFF)
    target_compile_definitions(FOX PUBLIC HAVE_TIFF_H)
  endif()
endif()

if(WITH_WEBP)
  pkg_check_modules(WEBP libwebp)
  if(WEBP_FOUND)
    target_link_libraries(FOX_DEPS INTERFACE ${WEBP_LIBRARIES})
    target_compile_definitions(FOX PUBLIC HAVE_WEBP_H)
  endif()
endif()

if(WITH_OPENJPEG)
  pkg_check_modules(OPENJPEG libopenjp2)
  if(OPENJPEG_FOUND)
    target_link_libraries(FOX_DEPS INTERFACE ${OPENJPEG_LIBRARIES})
    target_compile_definitions(FOX PUBLIC HAVE_JP2_H HAVE_J2K_H)
    target_include_directories(FOX PRIVATE ${OPENJPEG_INCLUDE_DIRS})
  else()
    pkg_check_modules(OPENJPEG libopenjpeg1)
    if(OPENJPEG_FOUND)
      target_link_libraries(FOX_DEPS INTERFACE ${OPENJPEG_LIBRARIES})
      target_compile_definitions(FOX PUBLIC HAVE_JP2_H HAVE_J2K_H)
      target_include_directories(FOX PRIVATE ${OPENJPEG_INCLUDE_DIRS})
    else()
      pkg_check_modules(OPENJPEG libopenjpeg)
      if(OPENJPEG_FOUND)
        target_link_libraries(FOX_DEPS INTERFACE ${OPENJPEG_LIBRARIES})
        target_compile_definitions(FOX PUBLIC HAVE_JP2_H HAVE_J2K_H)
        target_include_directories(FOX PRIVATE ${OPENJPEG_INCLUDE_DIRS})
    endif()
    endif()
  endif()
endif()


if(WITH_OPENGL)
  find_package(OpenGL REQUIRED COMPONENTS OpenGL GLX)
  if (NOT TARGET OpenGL::GLU)
    message(FATAL_ERROR "OpenGL Utility Library not found")
  endif()
  target_link_libraries(FOX_DEPS INTERFACE OpenGL::GL OpenGL::GLU OpenGL::GLX)
  target_compile_definitions(FOX PUBLIC HAVE_GL_H HAVE_GLU_H HAVE_GLX_H)
endif()

# Track dependencies that were actually linked (for generating fox-config.cmake)
# This list is used to automatically generate find_dependency() calls
# Only needed for static library builds - shared libraries already have dependencies linked in
set(FOX_LINKED_DEPENDENCIES "" CACHE INTERNAL "List of dependencies linked to FOX_DEPS")

# Always required
list(APPEND FOX_LINKED_DEPENDENCIES "Threads" "X11" "Freetype")

# Optional - only add if actually linked
if(TARGET ZLIB::ZLIB)
  list(APPEND FOX_LINKED_DEPENDENCIES "ZLIB")
endif()
if(TARGET BZip2::BZip2)
  list(APPEND FOX_LINKED_DEPENDENCIES "BZip2")
endif()
if(TARGET TIFF::TIFF)
  list(APPEND FOX_LINKED_DEPENDENCIES "TIFF")
endif()
if(JPEG_FOUND)
  list(APPEND FOX_LINKED_DEPENDENCIES "JPEG")
endif()
if(WITH_OPENGL)
  # Consumer just needs to find OpenGL - the targets file knows which components to use
  list(APPEND FOX_LINKED_DEPENDENCIES "OpenGL")
endif()
if(TARGET WebP::webp)
  list(APPEND FOX_LINKED_DEPENDENCIES "WebP")
endif()
if(TARGET openjp2)
  list(APPEND FOX_LINKED_DEPENDENCIES "OpenJPEG")
endif()
if(TARGET Cups::Cups)
  list(APPEND FOX_LINKED_DEPENDENCIES "Cups")
endif()

# X Input Method support (X11-specific)
if(NOT WITH_XIM)
  target_compile_definitions(FOX_XINCS INTERFACE NO_XIM)
endif()
