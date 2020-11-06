// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/types.h>
#include <dirent.h>


/* Function pointers to hold the value of the glibc functions */
static  ssize_t (*real_write)(int fd, const void *buf, size_t count) = NULL;
static int (*real_open) (const char *__file, int __oflag, int flags );
static FILE * (*real_fopen)(const char *pathname, const char *mode);
static FILE * (*real_fopen64)(const char *pathname, const char *mode);
static size_t (*real_fwrite)(const void *__restrict __ptr, size_t __size,
               size_t __n, FILE *__restrict __s);
static size_t (*real_fwrite64)(const void *__restrict __ptr, size_t __size,
                             size_t __n, FILE *__restrict __s);
static DIR *(*real_opendir) (const char *__name);
static struct dirent *(*real_readdir)(DIR *__dirp);


FILE * fopen(const char *pathname, const char *mode)
{
    printf("fopen:file#:%s\n", pathname);
    real_fopen64 = dlsym(RTLD_NEXT, "fopen64");
    return real_fopen64(pathname, mode);
}

__asm__(".symver fopen,fopen@GLIBC_2.2.5");

FILE * fopen64(const char *pathname, const char *mode)
{
    printf("fopen64:file#:%s\n", pathname);
    real_fopen = dlsym(RTLD_NEXT, "fopen");
    return real_fopen(pathname, mode);
}

__asm__(".symver fopen64,fopen64@GLIBC_2.2.5");

int open(const char *__file, int __oflag, ...)
{
    va_list valist;
    printf("open:file#:%s\n", __file);
    real_open = dlsym(RTLD_NEXT, "open");
    int mode;
    va_start( valist, __oflag);
        mode = va_arg(valist,int);
    va_end(valist);
    return real_open(__file, __oflag, mode);
}
__asm__(".symver open,open@GLIBC_2.2.5");

/* wrapping write function call */
ssize_t write(int fd, const void *buf, size_t count)
{

    /* printing out the number of characters */
    printf("write:chars#:%lu\n", count);
    /* reslove the real write function from glibc
     * and pass the arguments.
     */
    real_write = dlsym(RTLD_NEXT, "write");
    return real_write(fd, buf, count);
}


size_t fwrite (const void *__restrict __ptr, size_t __size,
                      size_t __n, FILE *__restrict __s)
{
    printf("fwrite:file#:%p\n", __s);
    real_fwrite = dlsym(RTLD_NEXT, "fwrite");
    return real_fwrite(__ptr, __size, __n, __s);
}

__asm__(".symver fwrite,fwrite@GLIBC_2.2.5");

size_t fwrite64 (const void *__restrict __ptr, size_t __size,
               size_t __n, FILE *__restrict __s)
{
    printf("fwrite64:file#:%p\n", __s);
    real_fwrite64 = dlsym(RTLD_NEXT, "fwrite");
    return real_fwrite(__ptr, __size, __n, __s);
}

__asm__(".symver fwrite64,fwrite64@GLIBC_2.2.5");

DIR *opendir(const char *name)
{
    printf("opendir:file#%s", name );
    real_opendir = dlsym(RTLD_NEXT, "opendir");
    return real_opendir(name);
}

__asm__(".symver opendir,opendir@GLIBC_2.2.5");


extern void *ff_fopen( const char *pcFile, const char *pcMode );

struct dirent *readdir (DIR *__dirp) {

    void * dupa = ff_fopen( ".", "r");
    printf("ff_fopen:DIR#%p", dupa );

    printf("readdir:DIR#%p", __dirp );
    real_readdir = dlsym(RTLD_NEXT, "readdir");
    return real_readdir(__dirp);
}
__asm__(".symver readdir,readdir@GLIBC_2.2.5");
