#ifndef HEADER_HTTP_LIB_SETUP_H
#define HEADER_HTTP_LIB_SETUP_H

/*
 * Define WIN32 when build target is Win32 API
 */

#if (defined(_WIN32) || defined(__WIN32__)) && !defined(WIN32)
#define WIN32
#endif

/*
 * Include configuration script results or hand-crafted
 * configuration file for platforms which lack config tool.
 */

#ifdef WIN32
#include "config-win32.h"
#endif

#ifdef __VXWORKS__
#  include "config-vxworks.h"
#endif

#ifdef __LINUX__
#  include "config-linux.h"
#endif

#ifdef __ARM_D6446__
#  include "config-arm.h"
#endif

#ifdef _NETRA600_
#  include "config-netra.h"
#endif

#ifdef _IOS_PLATFORM_
#include "config-iOS.h"
#endif




/* ================================================================ */
/* Definition of preprocessor macros/symbols which modify compiler  */
/* behavior or generated code characteristics must be done here,   */
/* as appropriate, before any system header file is included. It is */
/* also possible to have them defined in the config file included   */
/* before this point. As a result of all this we frown inclusion of */
/* system header files in our config files, avoid this at any cost. */
/* ================================================================ */

/*
 * AIX 4.3 and newer needs _THREAD_SAFE defined to build
 * proper reentrant code. Others may also need it.
 */

#ifdef NEED_THREAD_SAFE
#  ifndef _THREAD_SAFE
#    define _THREAD_SAFE
#  endif
#endif

/*
 * Tru64 needs _REENTRANT set for a few function prototypes and
 * things to appear in the system header files. Unixware needs it
 * to build proper reentrant code. Others may also need it.
 */

#ifdef NEED_REENTRANT
#  ifndef _REENTRANT
#    define _REENTRANT
#  endif
#endif

/*
 * Include header files for windows builds before redefining anything.
 * Use this preprocessor block only to include or exclude windows.h,
 * winsock2.h, ws2tcpip.h or winsock.h. Any other windows thing belongs
 * to any other further and independent block.  Under Cygwin things work
 * just as under linux (e.g. <sys/socket.h>) and the winsock headers should
 * never be included when __CYGWIN__ is defined.  configure script takes
 * care of this, not defining HAVE_WINDOWS_H, HAVE_WINSOCK_H, HAVE_WINSOCK2_H,
 * neither HAVE_WS2TCPIP_H when __CYGWIN__ is defined.
 */

#ifdef HAVE_WINDOWS_H
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#  ifdef HAVE_WINSOCK2_H
#    include <winsock2.h>
#    ifdef HAVE_WS2TCPIP_H
#       include <ws2tcpip.h>
#    endif
#  else
#    ifdef HAVE_WINSOCK_H
#      include <winsock.h>
#    endif
#  endif
#endif

/*
 * Define USE_WINSOCK to 2 if we have and use WINSOCK2 API, else
 * define USE_WINSOCK to 1 if we have and use WINSOCK  API, else
 * undefine USE_WINSOCK.
 */

#undef USE_WINSOCK

#ifdef HAVE_WINSOCK2_H
#  define USE_WINSOCK 2
#else
#  ifdef HAVE_WINSOCK_H
#    define USE_WINSOCK 1
#  endif
#endif


#ifdef __VXWORKS__
#  include <sockLib.h>    /* for generic BSD socket functions */
#  include <ioLib.h>      /* for basic I/O interface functions */
#endif

#include <stdio.h>
#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif
#include <errno.h>

#ifdef __TANDEM /* for nsr-tandem-nsk systems */
#include <floss.h>
#endif

#ifndef STDC_HEADERS /* no standard C headers! */
#include <curl/stdcheaders.h>
#endif

/*
 * Large file (>2Gb) support using WIN32 functions.
 */

#ifdef USE_WIN32_LARGE_FILES
#  include <io.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  define lseek(fdes,offset,whence)  _lseeki64(fdes, offset, whence)
#  define fstat(fdes,stp)            _fstati64(fdes, stp)
#  define stat(fname,stp)            _stati64(fname, stp)
#  define struct_stat                struct _stati64
#  define LSEEK_ERROR                (__int64)-1
#endif

/*
 * Small file (<2Gb) support using WIN32 functions.
 */

#ifdef USE_WIN32_SMALL_FILES
#  include <io.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  define lseek(fdes,offset,whence)  _lseek(fdes, (long)offset, whence)
#  define fstat(fdes,stp)            _fstat(fdes, stp)
#  define stat(fname,stp)            stat(fname, stp)
#  define struct_stat                struct stat
#  define LSEEK_ERROR                (long)-1
#endif

#ifndef struct_stat
#  define struct_stat struct stat
#endif

#ifndef LSEEK_ERROR
#  define LSEEK_ERROR (off_t)-1
#endif

/*
 * Default sizeof(off_t) in case it hasn't been defined in config file.
 */

#ifndef SIZEOF_OFF_T
#  if defined(__VMS) && !defined(__VAX)
#    if defined(_LARGEFILE)
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__OS400__) && defined(__ILEC400__)
#    if defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__MVS__) && defined(__IBMC__)
#    if defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__370__) && defined(__IBMC__)
#    if defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  endif
#  ifndef SIZEOF_OFF_T
#    define SIZEOF_OFF_T 4
#  endif
#endif

/*
 * Arg 2 type for gethostname in case it hasn't been defined in config file.
 */

#ifndef GETHOSTNAME_TYPE_ARG2
#  ifdef USE_WINSOCK
#    define GETHOSTNAME_TYPE_ARG2 int
#  else
#    define GETHOSTNAME_TYPE_ARG2 size_t
#  endif
#endif

/* Below we define some functions. They should

   4. set the SIGALRM signal timeout
   5. set dir/file naming defines
   */

#ifdef WIN32

#  define DIR_CHAR      "\\"
#  define DOT_CHAR      "_"

#else /* WIN32 */

#  ifdef MSDOS  /* Watt-32 */

#    include <sys/ioctl.h>
#    define select(n,r,w,x,t) select_s(n,r,w,x,t)
#    define ioctl(x,y,z) ioctlsocket(x,y,(char *)(z))
#    include <tcp.h>
#    ifdef word
#      undef word
#    endif
#    ifdef byte
#      undef byte
#    endif

#  endif /* MSDOS */

#  ifdef __minix
     /* Minix 3 versions up to at least 3.1.3 are missing these prototypes */
     extern char * strtok_r(char *s, const char *delim, char **last);
     extern struct tm * gmtime_r(const time_t * const timep, struct tm *tmp);
#  endif

#  define DIR_CHAR      "/"
#  ifndef DOT_CHAR
#    define DOT_CHAR      "."
#  endif

#  ifdef MSDOS
#    undef DOT_CHAR
#    define DOT_CHAR      "_"
#  endif

#  ifndef fileno /* sunos 4 have this as a macro! */
     int fileno( FILE *stream);
#  endif

#endif /* WIN32 */

/*
 * msvc 6.0 requires PSDK in order to have INET6_ADDRSTRLEN
 * defined in ws2tcpip.h as well as to provide IPv6 support.
 */

#if defined(_MSC_VER) && !defined(__POCC__)
#  if !defined(HAVE_WS2TCPIP_H) || \
     ((_MSC_VER < 1300) && !defined(INET6_ADDRSTRLEN))
#    undef HAVE_GETADDRINFO_THREADSAFE
#    undef HAVE_FREEADDRINFO
#    undef HAVE_GETADDRINFO
#    undef HAVE_GETNAMEINFO
#    undef ENABLE_IPV6
#  endif
#endif

/* ---------------------------------------------------------------- */
/*             resolver specialty compile-time defines              */
/*         CURLRES_* defines to use in the host*.c sources          */
/* ---------------------------------------------------------------- */

/*
 * lcc-win32 doesn't have _beginthreadex(), lacks threads support.
 */

#if defined(__LCC__) && defined(WIN32)
#  undef USE_THREADS_POSIX
#  undef USE_THREADS_WIN32
#endif

/*
 * MSVC threads support requires a multi-threaded runtime library.
 * _beginthreadex() is not available in single-threaded ones.
 */

#if defined(_MSC_VER) && !defined(__POCC__) && !defined(_MT)
#  undef USE_THREADS_POSIX
#  undef USE_THREADS_WIN32
#endif

/*
 * Mutually exclusive CURLRES_* definitions.
 */

#ifdef USE_ARES
#  define CURLRES_ASYNCH
#  define CURLRES_ARES
#elif defined(USE_THREADS_POSIX) || defined(USE_THREADS_WIN32)
#  define CURLRES_ASYNCH
#  define CURLRES_THREADED
#else
#  define CURLRES_SYNCH
#endif

#ifdef ENABLE_IPV6
#  define CURLRES_IPV6
#else
#  define CURLRES_IPV4
#endif

/* ---------------------------------------------------------------- */

/*
 * When using WINSOCK, TELNET protocol requires WINSOCK2 API.
 */

#if defined(USE_WINSOCK) && (USE_WINSOCK != 2)
#  define CURL_DISABLE_TELNET 1
#endif

/*
 * msvc 6.0 does not have struct sockaddr_storage and
 * does not define IPPROTO_ESP in winsock2.h. But both
 * are available if PSDK is properly installed.
 */

#if defined(_MSC_VER) && !defined(__POCC__)
#  if !defined(HAVE_WINSOCK2_H) || ((_MSC_VER < 1300) && !defined(IPPROTO_ESP))
#    undef HAVE_STRUCT_SOCKADDR_STORAGE
#  endif
#endif

/*
 * Intentionally fail to build when using msvc 6.0 without PSDK installed.
 * The brave of heart can circumvent this, defining ALLOW_MSVC6_WITHOUT_PSDK
 * in lib/config-win32.h although absolutely discouraged and unsupported.
 */

#if defined(_MSC_VER) && !defined(__POCC__)
#  if !defined(HAVE_WINDOWS_H) || ((_MSC_VER < 1300) && !defined(_FILETIME_))
#    if !defined(ALLOW_MSVC6_WITHOUT_PSDK)
#      error MSVC 6.0 requires "February 2003 Platform SDK" a.k.a. "Windows Server 2003 PSDK"
#    else
#      define CURL_DISABLE_LDAP 1
#    endif
#  endif
#endif


#ifndef SIZEOF_TIME_T
/* assume default size of time_t to be 32 bit */
#define SIZEOF_TIME_T 4
#endif

#define LIBIDN_REQUIRED_VERSION "0.4.1"

#if defined(USE_GNUTLS) || defined(USE_SSLEAY) || defined(USE_NSS) || defined(USE_QSOSSL) || defined(USE_POLARSSL)
#define USE_SSL    /* SSL support has been enabled */
#endif

#if !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_NTLM)
#if defined(USE_SSLEAY) || defined(USE_WINDOWS_SSPI) || defined(USE_GNUTLS) || defined(USE_NSS)
#define USE_NTLM
#endif
#endif

/* non-configure builds may define CURL_WANTS_CA_BUNDLE_ENV */
#if defined(CURL_WANTS_CA_BUNDLE_ENV) && !defined(CURL_CA_BUNDLE)
#define CURL_CA_BUNDLE getenv("CURL_CA_BUNDLE")
#endif

/*
 * Include macros and defines that should only be processed once.
 */

#ifndef __SETUP_ONCE_H
#include "setup_once.h"
#endif

#endif /* HEADER_CURL_LIB_SETUP_H */
