/**
 ******************************************************************************
 * @file	clib.h
 * @brief	
 *		配置C库

 	其配置作用于clib/ubootlibc.c clib/string.c两个文件
 * @ see ubootlibc.c string.c
 *
*/
/*--------------------------------------------------
 * version    |    author    |    date    |    content
 * V1.0 		uboot		2014-12-9	
 ******************************************************************************
*/


#ifndef _CLIB_
#define _CLIB_





// #define __HAVE_ARCH_STRCPY
// #define __HAVE_ARCH_STRNCPY
#define __HAVE_ARCH_STRCAT
#define __HAVE_ARCH_STRNCAT
// #define __HAVE_ARCH_STRCMP
#define __HAVE_ARCH_STRNCMP
#define __HAVE_ARCH_STRCHR
#define __HAVE_ARCH_STRRCHR
// #define __HAVE_ARCH_STRLEN
#define __HAVE_ARCH_STRNLEN
#define __HAVE_ARCH_STRDUP
// #define __HAVE_ARCH_STRSPN
// #define __HAVE_ARCH_STRPBRK
// #define __HAVE_ARCH_STRTOK
#define __HAVE_ARCH_STRSEP
#define __HAVE_ARCH_STRSWAB
#define __HAVE_ARCH_MEMSET
#define __HAVE_ARCH_BCOPY
// #define __HAVE_ARCH_MEMCPY
#define __HAVE_ARCH_MEMMOVE
// #define __HAVE_ARCH_MEMCMP
#define __HAVE_ARCH_MEMSCAN
#define __HAVE_ARCH_STRSTR
#define __HAVE_ARCH_MEMCHR




#endif

