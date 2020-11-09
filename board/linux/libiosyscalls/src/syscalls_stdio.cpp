// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <dlfcn.h>
#include <stdio.h>
#include "ff_stdio.h"
#include <errno.h>
#include <mutex>
#include <set>


namespace
{
    int (*real_fprintf)(FILE *__restrict __stream, const char *__restrict __format, ...);
    size_t (*real_fwrite)(const void *__restrict __ptr, size_t __size, size_t __n, FILE *__restrict __s);
    std::set<FF_FILE*> g_fd_map;
    std::recursive_mutex g_mutex;

    void __attribute__((constructor)) _lib_stdio_initialize()
    {
        real_fprintf = reinterpret_cast<decltype(real_fprintf)>(dlsym(RTLD_NEXT, "fprintf"));
        real_fwrite = reinterpret_cast<decltype(real_fwrite)>(dlsym(RTLD_NEXT, "fwrite"));
        if(!real_fprintf || !real_fwrite) {
            abort();
        }
    }
}

extern "C"
{
    FILE *fopen(const char *pathname, const char *mode)
    {
        std::lock_guard<std::recursive_mutex> _lck(g_mutex);
        auto ret = ff_fopen(pathname, mode);
        if(ret) g_fd_map.insert(ret);
        errno    = stdioGET_ERRNO();
        return reinterpret_cast<FILE*>(ret);
    }
    __asm__(".symver fopen,fopen@GLIBC_2.2.5");

    FILE *fopen64(const char *pathname, const char *mode)
    {
        auto ret = reinterpret_cast<FILE *>(ff_fopen(pathname, mode));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fopen64,fopen64@GLIBC_2.2.5");

    int fclose(FILE *__stream)
    {
        std::lock_guard<std::recursive_mutex> _lck(g_mutex);
        auto ret = ff_fclose(reinterpret_cast<FF_FILE *>(__stream));
        g_fd_map.erase(reinterpret_cast<FF_FILE*>(__stream));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fclose,fclose@GLIBC_2.2.5");

    FILE *fdopen(int __fd, const char *__modes) __THROW __wur
    {
        real_fprintf(stderr, "unimplemented call %s\n", __PRETTY_FUNCTION__  );
        errno = ENOTSUP;
        return nullptr;
    }
    __asm__(".symver fdopen,fdopen@GLIBC_2.2.5");

    int feof(FILE *__stream) __THROW __wur
    {
        auto ret = ff_feof(reinterpret_cast<FF_FILE *>(__stream));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver feof,feof@GLIBC_2.2.5");

    int ferror(FILE *) __THROW __wur
    {
        return stdioGET_ERRNO();
    }
    __asm__(".symver ferror,ferror@GLIBC_2.2.5");

    int fflush(FILE *__stream)
    {
        auto ret = ff_fflush(reinterpret_cast<FF_FILE *>(__stream));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fflush,fflush@GLIBC_2.2.5");

    int fgetc(FILE *__stream)
    {
        auto ret = ff_fgetc(reinterpret_cast<FF_FILE *>(__stream));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fgetc,fgetc@GLIBC_2.2.5");

    int fgetpos(FILE *__restrict __stream, fpos_t *__restrict __pos)
    {
        auto ret = ff_ftell(reinterpret_cast<FF_FILE *>(__stream));
        if (ret > 0 && __pos) {
            __pos->__pos = ret;
            return 0;
        }
        else
            return ret;
    }
    __asm__(".symver fgetpos,fgetpos@GLIBC_2.2.5");

    int fgetpos64(FILE *__restrict __stream, fpos64_t *__restrict __pos)
    {
        auto ret = ff_ftell(reinterpret_cast<FF_FILE *>(__stream));
        if (ret > 0 && __pos) {
            __pos->__pos = ret;
            return 0;
        }
        else
            return ret;
    }
    __asm__(".symver fgetpos64,fgetpos64@GLIBC_2.2.5");

    char *fgets(char *__restrict __s, int __n, FILE *__restrict __stream) __wur
    {
        auto ret = ff_fgets(__s, __n, reinterpret_cast<FF_FILE *>(__stream));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fgets,fgets@GLIBC_2.2.5");

    int fileno(FILE *__stream) __THROW __wur
    {
        if(__stream==stdin) return STDIN_FILENO;
        else if(__stream==stdout) return STDOUT_FILENO;
        else if(__stream==stderr) return STDERR_FILENO;
        return reinterpret_cast<uintptr_t>(__stream);
    }
    __asm__(".symver fileno,fileno@GLIBC_2.2.5");

    int fprintf(FILE *__restrict __stream, const char *__restrict __format, ...)
    {
        int iCount;
        size_t xResult;
        char *pcBuffer;
        va_list xArgs;

        pcBuffer = (char *)ffconfigMALLOC(ffconfigFPRINTF_BUFFER_LENGTH);
        if (pcBuffer == NULL) {
            /* Store the errno to thread local storage. */
            stdioSET_ERRNO(pdFREERTOS_ERRNO_ENOMEM);
            iCount = -1;
        }
        else {
            va_start(xArgs, __format);
            iCount = vsnprintf(pcBuffer, ffconfigFPRINTF_BUFFER_LENGTH, __format, xArgs);
            va_end(xArgs);

            /* ff_fwrite() will set ff_errno. */
            if (iCount > 0) {
                xResult = ff_fwrite(pcBuffer, (size_t)1, (size_t)iCount, reinterpret_cast<FF_FILE *>(__stream));
                if (xResult < (size_t)iCount) {
                    iCount = -1;
                }
            }
            ffconfigFREE(pcBuffer);
        }
        errno = stdioGET_ERRNO();
        return iCount;
    }
    __asm__(".symver fprintf,fprintf@GLIBC_2.2.5");

    int fputc(int __c, FILE *__stream)
    {
        auto ret = ff_fputc(__c, reinterpret_cast<FF_FILE *>(__stream));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fputc,fputc@GLIBC_2.2.5");

    int fputs(const char *__restrict __s, FILE *__restrict __stream)
    {
        int ret;
        while (*__s) {
            ret = ff_fputc(*__s++, reinterpret_cast<FF_FILE *>(__stream));
            if (ret < 0)
                break;
        }
        errno = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fputs,fputs@GLIBC_2.2.5");

    size_t fread(void *__restrict __ptr, size_t __size, size_t __n, FILE *__restrict __stream) __wur
    {
        auto ret = ff_fread(__ptr, __size, __n, reinterpret_cast<FF_FILE *>(__stream));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fread,fread@GLIBC_2.2.5");


    FILE *freopen (const char *__restrict __filename,
                      const char *__restrict __modes,
                      FILE *__restrict __stream) __wur
    {
        if( ff_fclose(reinterpret_cast<FF_FILE*>(__stream)) < 0 ) {
            errno = stdioGET_ERRNO();
            return nullptr;
        }
        auto ret = ff_fopen(__filename, __modes);
        errno = stdioGET_ERRNO();
        return reinterpret_cast<FILE*>(ret);
    }
    __asm__(".symver freopen,freopen@GLIBC_2.2.5");

    int fscanf (FILE *__restrict __stream,
                   const char *__restrict __format, ...) __wur
    {
        real_fprintf(stderr, "Unimplemented call\n");
        errno = ENOTSUP;
        return -1;
    }
    __asm__(".symver fscanf,fscanf@GLIBC_2.2.5");

    int fseek (FILE *__stream, long int __off, int __whence)
    {
        auto ret = ff_fseek(reinterpret_cast<FF_FILE*>(__stream), __off, __whence );
        errno = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fseek,fseek@GLIBC_2.2.5");

    int fsetpos (FILE *__stream, const fpos_t *__pos)
    {
        auto ret = ff_fseek( reinterpret_cast<FF_FILE*>(__stream), __pos->__pos, SEEK_SET );
        errno = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fsetpos,fsetpos@GLIBC_2.2.5");

    int fsetpos64 (FILE *__stream, const fpos64_t *__pos)
    {
        auto ret = ff_fseek( reinterpret_cast<FF_FILE*>(__stream), __pos->__pos, SEEK_SET );
        errno = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fsetpos64,fsetpos64@GLIBC_2.2.5");


    long int ftell (FILE *__stream) __wur
    {
        auto ret = ff_ftell(reinterpret_cast<FF_FILE*>(__stream));
        errno = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver ftell,ftell@GLIBC_2.2.5");

    size_t fwrite (const void *__restrict __ptr, size_t __size,
                          size_t __n, FILE *__restrict __s)
    {
        std::lock_guard<std::recursive_mutex> _lck(g_mutex);
        if( g_fd_map.find(reinterpret_cast<FF_FILE*>(__s)) == g_fd_map.end() ) {
            return real_fwrite( __ptr, __size, __n, __s );
        }
        auto ret = ff_fwrite(__ptr, __size, __n, reinterpret_cast<FF_FILE*>(__s));
        errno = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fwrite,fwrite@GLIBC_2.2.5");

    int getc(FILE *__stream)
    {
        auto ret = ff_fgetc(reinterpret_cast<FF_FILE *>(__stream));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver getc,getc@GLIBC_2.2.5");

    int putc(int __c, FILE *__stream)
    {
        auto ret = ff_fputc(__c, reinterpret_cast<FF_FILE *>(__stream));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver putc,putc@GLIBC_2.2.5");

    int remove (const char *__filename) __THROW
    {
        auto ret = ff_remove(__filename);
        errno = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver remove,remove@GLIBC_2.2.5");

    void rewind (FILE *__stream)
    {
        ff_fseek(reinterpret_cast<FF_FILE*>(__stream), 0L, SEEK_SET);
    }
    __asm__(".symver rewind,rewind@GLIBC_2.2.5");


    void setbuf (FILE *__restrict , char *__restrict ) __THROW
    {
        real_fprintf(stderr, "unimplemented call %s\n", __PRETTY_FUNCTION__ );
        errno = ENOTSUP;
    }
    __asm__(".symver setbuf,setbuf@GLIBC_2.2.5");

    int setvbuf (FILE *__restrict __stream, char *__restrict __buf,
                        int __modes, size_t __n) __THROW
    {
        real_fprintf(stderr, "unimplemented call %s\n", __PRETTY_FUNCTION__ );
        errno = ENOTSUP;
        return 0;
    }
    __asm__(".symver setvbuf,setbuf@GLIBC_2.2.5");


    void setbuffer (FILE *__restrict __stream, char *__restrict __buf,
                           size_t __size) __THROW
    {
        errno = ENOTSUP;
    }
    __asm__(".symver setbuffer,setbuffer@GLIBC_2.2.5");
/* Make STREAM line-buffered.  */
    void setlinebuf (FILE *__stream) __THROW
    {
        errno = ENOTSUP;
        real_fprintf(stderr, "unimplemented call %s\n", __PRETTY_FUNCTION__ );
    }
    __asm__(".symver setlinebuf,setlinebuf@GLIBC_2.2.5");

}
