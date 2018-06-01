#ifndef __LIB_CONFIG_IOS_H
#define __LIB_CONFIG_IOS_H

/* =================================================================== */
/*   lib/config-iOS.h - Hand crafted config file for iPhone/iPad          */
/* =================================================================== */

#define OS "iOS"

/* ---------------------------------------------------------------- */
/*                          HEADER FILES                            */
/* ---------------------------------------------------------------- */

#define HAVE_SYS_UN_H           1
#define HAVE_POLL_FINE          1 
#define HAVE_SYS_PARAM_H		1
#define HAVE_SYS_IOCTL_H		1
#define HAVE_NETINET_IN_H       1
#define HAVE_SYS_SOCKET_H       1          // socklen_t
#define HAVE_SYS_SELECT_H       1
#define HAVE_NETDB_H            1          // struct hostent
#define HAVE_ARPA_INET_H        1
#define HAVE_UNISTD_H           1
#define HAVE_NET_IF_H           1
#define HAVE_SYS_TYPES_H        1
// #define HAVE_GETTIMEOFDAY       1
#define HAVE_FCNTL_H            1         // O_WRONLY/O_CREAT/O_TRUNC/O_RDONLY   open
#define HAVE_SYS_STAT_H         1         // stat  fstat
#define HAVE_ALLOCA_H           1
#define HAVE_TIME_H             1
#define HAVE_STDLIB_H           1
#define HAVE_UTIME_H            1
#define HAVE_SYS_TIME_H         1
#define STDC_HEADERS             1
#define USE_THREADS_POSIX        1
#define HAVE_PTHREAD_H           1
// 
#define TIME_WITH_SYS_TIME      1
// 
// #define HAVE_ALARM              1
// #define HAVE_STRDUP             1
// #define HAVE_UTIME              1
// #define HAVE_SETVBUF            1
// #define HAVE_STRFTIME           1
// #define HAVE_INET_ADDR          1
// #define HAVE_MEMCPY             1
#define HAVE_SELECT             1
#define HAVE_SOCKET             1
#define HAVE_STRUCT_TIMEVAL     1
#define HAVE_BOOL_T             1
#define HAVE_FCNTL_O_NONBLOCK   1

// 
// //#define HAVE_STRICMP          1
// #define HAVE_SIGACTION          1
// #define HAVE_SIGNAL_H           1
// #define HAVE_SIG_ATOMIC_T       1
// 
// #ifdef MACOS_SSL_SUPPORT
// #       define USE_SSLEAY       1
// #       define USE_OPENSSL      1
// #endif
// 
// #define CURL_DISABLE_LDAP       1
// 
// #define HAVE_RAND_STATUS        1
// #define HAVE_RAND_EGD           1
// 
// #define HAVE_IOCTL              1
#define HAVE_IOCTL_FIONBIO      1
// 
// #define RETSIGTYPE void
// 
#define SIZEOF_INT              4
#define SIZEOF_SHORT            2
// 
// #define HAVE_GETNAMEINFO 1
// #define GETNAMEINFO_QUAL_ARG1 const
// #define GETNAMEINFO_TYPE_ARG1 struct sockaddr *
// #define GETNAMEINFO_TYPE_ARG2 socklen_t
// #define GETNAMEINFO_TYPE_ARG46 size_t
// #define GETNAMEINFO_TYPE_ARG7 int

#define HAVE_RECV 1
#define RECV_TYPE_ARG1 int
#define RECV_TYPE_ARG2 void *
#define RECV_TYPE_ARG3 size_t
#define RECV_TYPE_ARG4 int
#define RECV_TYPE_RETV ssize_t

#define HAVE_RECVFROM 1
#define RECVFROM_TYPE_ARG1 int
#define RECVFROM_TYPE_ARG2 void
#define RECVFROM_TYPE_ARG3 size_t
#define RECVFROM_TYPE_ARG4 int
#define RECVFROM_TYPE_ARG5 struct sockaddr
#define RECVFROM_TYPE_ARG6 int
#define RECVFROM_TYPE_RETV ssize_t
#define RECVFROM_TYPE_ARG2_IS_VOID 1

#define HAVE_SEND 1
#define SEND_TYPE_ARG1 int
#define SEND_QUAL_ARG2 const
#define SEND_TYPE_ARG2 void *
#define SEND_TYPE_ARG3 size_t
#define SEND_TYPE_ARG4 int
#define SEND_TYPE_RETV ssize_t

// #define HAVE_EXTRA_STRICMP_H 1
// #define HAVE_EXTRA_STRDUP_H  1


/* to disable FTP */
#define CURL_DISABLE_FTP 1

/* to disable TELNET */
#define CURL_DISABLE_TELNET 1

/* to disable LDAP */
#define CURL_DISABLE_LDAP 1

/* to disable TFTP */
#define CURL_DISABLE_TFTP 1

/* to disable POP3 */
#define CURL_DISABLE_POP3 1

/* to disable SMTP */
#define CURL_DISABLE_SMTP 1




#endif /* __LIB_CONFIG_IOS_H */
