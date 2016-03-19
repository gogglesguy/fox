/********************************************************************************
*                                                                               *
*              F O X   P r i v a t e   I n c l u d e   F i l e s                *
*                                                                               *
*********************************************************************************
* Copyright (C) 1997,2010 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or modify          *
* it under the terms of the GNU Lesser General Public License as published by   *
* the Free Software Foundation; either version 3 of the License, or             *
* (at your option) any later version.                                           *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 *
* GNU Lesser General Public License for more details.                           *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public License      *
* along with this program.  If not, see <http://www.gnu.org/licenses/>          *
********************************************************************************/
#ifndef XINCS_H
#define XINCS_H


////////////////////  DO NOT INCLUDE THIS PRIVATE HEADER FILE  //////////////////

// Thread safe
#ifndef _POSIX_PTHREAD_SEMANTICS
#define _POSIX_PTHREAD_SEMANTICS
#endif

// GNU extras if we can get them
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// Use 64-bit files
#ifndef WIN32
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#endif

// Basic includes
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <locale.h>
#include <fcntl.h>
#include <sys/stat.h>

// Platform includes
#if defined(WIN32)  /////////////// Windows /////////////////////////////////////

// Common headers
#if _WIN32_WINNT < 0x0400
#define _WIN32_WINNT 0x0400
#endif
#ifndef STRICT
#define STRICT
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>            // Core Windows stuff
#include <winspool.h>           // Printer stuff
#ifndef __CYGWIN__
#include <winsock2.h>
#endif
#include <commctrl.h>           // For _TrackMouseEvent
#include <shellapi.h>
#include <imm.h>                // IME
#ifdef UNICODE
#include <wchar.h>              // Wide character support
#endif

// OpenGL includes
#ifdef HAVE_GL_H
#include <GL/gl.h>
#endif
#ifndef GLAPIENTRY
#define GLAPIENTRY
#endif
#ifndef GLAPI
#define GLAPI
#endif
#ifdef HAVE_GLU_H
#include <GL/glu.h>
#endif

#else ////////////////////////////// Unix ///////////////////////////////////////

// Common headers
#include <grp.h>
#include <pwd.h>
#include <sys/ioctl.h>
#ifdef HAVE_UNISTD_H
#include <sys/types.h>
#include <unistd.h>
#endif
#ifdef HAVE_SYS_FILIO_H         // Get FIONREAD on Solaris
#include <sys/filio.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#define NAMLEN(dirent) strlen((dirent)->d_name)
#else
#define dirent direct
#define NAMLEN(dirent) (dirent)->d_namlen
#ifdef HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#endif
#ifdef HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif
#ifdef HAVE_NDIR_H
#include <ndir.h>
#endif
#endif
#ifdef TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_SYS_IPC_H
#include <sys/ipc.h>
#endif
#ifdef HAVE_SYS_SHM_H
#include <sys/shm.h>
#endif
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#ifdef HAVE_SYS_SYSCTL_H
#include <sys/sysctl.h>
#endif
#ifdef HAVE_SYS_PSTAT_H
#include <sys/pstat.h>
#endif
#if defined(__APPLE__)
#include <libkern/OSAtomic.h>
#endif
#include <pthread.h>
#include <semaphore.h>

// X11 includes
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xcms.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#ifdef HAVE_XSHM_H
#include <X11/extensions/XShm.h>
#endif
#ifdef HAVE_XCURSOR_H
#include <X11/Xcursor/Xcursor.h>
#endif
#ifdef HAVE_XFT_H
#include <X11/Xft/Xft.h>
#endif
#ifdef HAVE_XSHAPE_H
#include <X11/extensions/shape.h>
#endif
#ifdef HAVE_XRANDR_H
#include <X11/extensions/Xrandr.h>
#endif
#ifdef HAVE_XFIXES_H
#include <X11/extensions/Xfixes.h>
#endif
#ifdef HAVE_XRENDER_H
#include <X11/extensions/Xrender.h>
#endif
#ifdef HAVE_XINPUT_H
#include <X11/extensions/XI.h>
#include <X11/extensions/XInput.h>
#endif
#ifndef NO_XIM
#ifndef XlibSpecificationRelease        // Not defined until X11R5
#define NO_XIM
#elif XlibSpecificationRelease < 6      // Need at least Xlib X11R6
#define NO_XIM
#endif
#endif

// OpenGL includes
#ifdef HAVE_GL_H
#ifndef SUN_OGL_NO_VERTEX_MACROS
#define SUN_OGL_NO_VERTEX_MACROS
#endif
#ifndef HPOGL_SUPPRESS_FAST_API
#define HPOGL_SUPPRESS_FAST_API
#endif
#include <GL/gl.h>
#ifdef HAVE_GLX_H
#include <GL/glx.h>
#endif
#endif
#ifdef HAVE_GLU_H
#include <GL/glu.h>
#endif

#endif //////////////////////////////////////////////////////////////////////////

// Maximum path length
#ifndef MAXPATHLEN
#if defined(PATH_MAX)
#define MAXPATHLEN   PATH_MAX
#elif defined(_MAX_PATH)
#define MAXPATHLEN   _MAX_PATH
#elif defined(MAX_PATH)
#define MAXPATHLEN   MAX_PATH
#else
#define MAXPATHLEN   2048
#endif
#endif

// Maximum host name length
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif

// Some systems don't have it
#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif
#ifndef SEEK_END
#define SEEK_END 2
#endif

#endif
