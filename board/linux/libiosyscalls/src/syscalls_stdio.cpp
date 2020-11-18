// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <dlfcn.h>
#include <stdio.h>
#include "ff_stdio.h"
#include <errno.h>
#include <mutex>
#include <unordered_map>
#include <handle_mapper.hpp>
#include "internal.hpp"
#include "debug.hpp"


//NOTE: It will be removed in later stage
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

namespace
{
    int (*real_fprintf)(FILE *__restrict __stream, const char *__restrict __format, ...);
    size_t (*real_fwrite)(const void *__restrict __ptr, size_t __size, size_t __n, FILE *__restrict __s);
    int (*real_fputs)(const char *__restrict __s, FILE *__restrict __stream);
    int (*real_fputc)(int __c, FILE *__stream);
    int (*real_fileno)(FILE *__stream) __THROW __wur;
    int (*real_vfprintf)(FILE *__restrict __s, const char *__restrict __format, __gnuc_va_list __arg);

    std::unordered_map<FF_FILE*,size_t> g_fd_map;
    std::recursive_mutex g_mutex;
    internal::handle_mapper<FF_FILE*> g_handles;

    void __attribute__((constructor)) _lib_stdio_initialize()
    {
        real_fprintf = reinterpret_cast<decltype(real_fprintf)>(dlsym(RTLD_NEXT, "fprintf"));
        real_fwrite = reinterpret_cast<decltype(real_fwrite)>(dlsym(RTLD_NEXT, "fwrite"));
        real_fputs = reinterpret_cast<decltype(real_fputs)>(dlsym(RTLD_NEXT, "fputs"));
        real_fputc = reinterpret_cast<decltype(real_fputc)>(dlsym(RTLD_NEXT, "fputc"));
        real_fileno = reinterpret_cast<decltype(real_fileno)>(dlsym(RTLD_NEXT, "fileno"));
        real_vfprintf = reinterpret_cast<decltype(real_vfprintf)>(dlsym(RTLD_NEXT, "vfprintf"));
        if(!real_fprintf || !real_fwrite || !real_fputs || !real_fputc
            || !real_fileno || !real_vfprintf) {
            abort();
        }
    }
}

namespace vfsn::linux::internal {
    namespace {
        constexpr auto FIRST_FILEDESC = 64'566'756;
    }
    // Convert ff_file to standard fd
    int  ff_file_to_fd( FF_FILE* fil )
    {
        std::lock_guard<std::recursive_mutex> _lck(g_mutex);
        const auto it = g_fd_map.find(fil);
        return ( it != g_fd_map.end() )?(it->second):(-1);
    }
    // Convert standard fd for ff file
    FF_FILE* fd_to_ff_file( int fd )
    {
        std::lock_guard<std::recursive_mutex> _lck(g_mutex);
        if (fd < FIRST_FILEDESC) {
            return nullptr;
        }
        if( !g_handles.exists(fd-FIRST_FILEDESC) ) {
            return nullptr;
        }
        return g_handles[fd-FIRST_FILEDESC];
    }
    // Check if it is our internal FILE* object
    bool is_ff_handle(FILE* descriptor) {
        std::lock_guard<std::recursive_mutex> _lck(g_mutex);
        const auto it = g_fd_map.find(reinterpret_cast<FF_FILE*>(descriptor));
        return it != g_fd_map.end();
    }

}

extern "C"
{
    namespace vfs = vfsn::linux::internal;
    FILE *fopen(const char *pathname, const char *mode)
    {
        TRACE_SYSCALL();
        char pathbuf[ffconfigMAX_FILENAME];
        auto ret = ff_fopen(vfs::relative_to_root(pathbuf,sizeof pathbuf,pathname), mode);
        if(ret) {
            std::lock_guard<std::recursive_mutex> _lck(g_mutex);
            const auto fd = g_handles.insert(ret) + vfs::FIRST_FILEDESC;
            g_fd_map.insert(std::make_pair(ret,fd));
        }
        errno    = stdioGET_ERRNO();
        return reinterpret_cast<FILE*>(ret);
    }
    __asm__(".symver fopen,fopen@GLIBC_2.2.5");

    FILE *fopen64(const char *pathname, const char *mode)
    {
        TRACE_SYSCALL();
        return fopen(pathname,mode);
    }
    __asm__(".symver fopen64,fopen64@GLIBC_2.2.5");

    int fclose(FILE *__stream)
    {
        TRACE_SYSCALL();
        if(!vfs::is_ff_handle(__stream)) {
            real_fprintf(stderr,"ERROR: It is not a ff file handle\n");
            errno = EINVAL;
            return -1;
        }
        auto ret = ff_fclose(reinterpret_cast<FF_FILE *>(__stream));
        {
            std::lock_guard<std::recursive_mutex> _lck(g_mutex);
            const auto it = g_fd_map.find(reinterpret_cast<FF_FILE *>(__stream));
            if( it != g_fd_map.end() ) {
                g_handles.remove(it->second - vfs::FIRST_FILEDESC);
                g_fd_map.erase(it);
            }
        }
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fclose,fclose@GLIBC_2.2.5");

    FILE *fdopen(int __fd, const char *__modes) __THROW __wur
    {
        TRACE_SYSCALL();
        real_fprintf(stderr, "unimplemented call %s\n", __PRETTY_FUNCTION__  );
        errno = ENOTSUP;
        return nullptr;
    }
    __asm__(".symver fdopen,fdopen@GLIBC_2.2.5");

    int feof(FILE *__stream) __THROW __wur
    {
        TRACE_SYSCALL();
        if(!vfs::is_ff_handle(__stream)) {
            real_fprintf(stderr,"ERROR: It is not a ff file handle\n");
            errno = EINVAL;
            return -1;
        }
        auto ret = ff_feof(reinterpret_cast<FF_FILE *>(__stream));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver feof,feof@GLIBC_2.2.5");

    int ferror(FILE *) __THROW __wur
    {
        TRACE_SYSCALL();
        return stdioGET_ERRNO();
    }
    __asm__(".symver ferror,ferror@GLIBC_2.2.5");

    int fflush(FILE *__stream)
    {
        TRACE_SYSCALL();
        if(!vfs::is_ff_handle(__stream)) {
            real_fprintf(stderr,"ERROR: It is not a ff file handle\n");
            errno = EINVAL;
            return -1;
        }
        auto ret = ff_fflush(reinterpret_cast<FF_FILE *>(__stream));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fflush,fflush@GLIBC_2.2.5");

    int fgetc(FILE *__stream)
    {
        TRACE_SYSCALL();
        if(!vfs::is_ff_handle(__stream)) {
            real_fprintf(stderr,"ERROR: It is not a ff file handle\n");
            errno = EINVAL;
            return -1;
        }
        auto ret = ff_fgetc(reinterpret_cast<FF_FILE *>(__stream));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fgetc,fgetc@GLIBC_2.2.5");

    int fgetpos(FILE *__restrict __stream, fpos_t *__restrict __pos)
    {
        TRACE_SYSCALL();
        if(!vfs::is_ff_handle(__stream)) {
            real_fprintf(stderr,"ERROR: It is not a ff file handle\n");
            errno = EINVAL;
            return -1;
        }
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
        TRACE_SYSCALL();
        if(!vfs::is_ff_handle(__stream)) {
            real_fprintf(stderr,"ERROR: It is not a ff file handle\n");
            errno = EINVAL;
            return -1;
        }
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
        TRACE_SYSCALL();
        if(!vfs::is_ff_handle(__stream)) {
            real_fprintf(stderr,"ERROR: It is not a ff file handle\n");
            errno = EINVAL;
            return nullptr;
        }
        auto ret = ff_fgets(__s, __n, reinterpret_cast<FF_FILE *>(__stream));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fgets,fgets@GLIBC_2.2.5");

    int fileno(FILE *__stream) __THROW __wur
    {
        auto ret = vfsn::linux::internal::ff_file_to_fd(reinterpret_cast<FF_FILE*>(__stream));
        if(ret<0) {
            return real_fileno(__stream);
        } else {
            TRACE_SYSCALL();
            return ret;
        }
    }
    __asm__(".symver fileno,fileno@GLIBC_2.2.5");

    int fprintf(FILE *__restrict __stream, const char *__restrict __format, ...)
    {
        int iCount;
        size_t xResult;
        char *pcBuffer;
        va_list xArgs;

        if(!vfs::is_ff_handle(__stream)) {
            va_list arglist;
            va_start( arglist, __format );
            auto ret = real_vfprintf( __stream, __format, arglist );
            va_end( arglist );
            return ret;
        }
        TRACE_SYSCALL();
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
        if(!vfs::is_ff_handle(__stream)) {
            return real_fputc( __c, __stream );
        }
        TRACE_SYSCALL();
        auto ret = ff_fputc(__c, reinterpret_cast<FF_FILE *>(__stream));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fputc,fputc@GLIBC_2.2.5");

    int fputs(const char *__restrict __s, FILE *__restrict __stream)
    {
        if(!vfs::is_ff_handle(__stream)) {
            return real_fputs( __s, __stream );
        }
        TRACE_SYSCALL();
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
        TRACE_SYSCALL();
        if(!vfs::is_ff_handle(__stream)) {
            real_fprintf(stderr,"ERROR: It is not a ff file handle\n");
            errno = EINVAL;
            return 0;
        }
        auto ret = ff_fread(__ptr, __size, __n, reinterpret_cast<FF_FILE *>(__stream));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fread,fread@GLIBC_2.2.5");


    FILE *freopen (const char *__restrict __filename,
                      const char *__restrict __modes,
                      FILE *__restrict __stream) __wur
    {
        TRACE_SYSCALL();
        if(!vfs::is_ff_handle(__stream)) {
            real_fprintf(stderr,"ERROR: It is not a ff file handle\n");
            errno = EINVAL;
            return nullptr;
        }
        if( fclose(__stream) < 0) {
            return nullptr;
        }
        return fopen(__filename, __modes );
    }
    __asm__(".symver freopen,freopen@GLIBC_2.2.5");

    int fseek (FILE *__stream, long int __off, int __whence)
    {
        TRACE_SYSCALL();
        if(!vfs::is_ff_handle(__stream)) {
            real_fprintf(stderr,"ERROR: It is not a ff file handle\n");
            errno = EINVAL;
            return -1;
        }
        auto ret = ff_fseek(reinterpret_cast<FF_FILE*>(__stream), __off, __whence );
        errno = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fseek,fseek@GLIBC_2.2.5");

    int fsetpos (FILE *__stream, const fpos_t *__pos)
    {
        TRACE_SYSCALL();
        if(!vfs::is_ff_handle(__stream)) {
            real_fprintf(stderr,"ERROR: It is not a ff file handle\n");
            errno = EINVAL;
            return -1;
        }
        auto ret = ff_fseek( reinterpret_cast<FF_FILE*>(__stream), __pos->__pos, SEEK_SET );
        errno = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fsetpos,fsetpos@GLIBC_2.2.5");

    int fsetpos64 (FILE *__stream, const fpos64_t *__pos)
    {
        TRACE_SYSCALL();
        if(!vfs::is_ff_handle(__stream)) {
            real_fprintf(stderr,"ERROR: It is not a ff file handle\n");
            errno = EINVAL;
            return -1;
        }
        auto ret = ff_fseek( reinterpret_cast<FF_FILE*>(__stream), __pos->__pos, SEEK_SET );
        errno = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fsetpos64,fsetpos64@GLIBC_2.2.5");


    long int ftell (FILE *__stream) __wur
    {
        TRACE_SYSCALL();
        if(!vfs::is_ff_handle(__stream)) {
            real_fprintf(stderr,"ERROR: It is not a ff file handle\n");
            errno = EINVAL;
            return -1;
        }
        auto ret = ff_ftell(reinterpret_cast<FF_FILE*>(__stream));
        errno = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver ftell,ftell@GLIBC_2.2.5");

    size_t fwrite (const void *__restrict __ptr, size_t __size,
                          size_t __n, FILE *__restrict __s)
    {
        if(!vfs::is_ff_handle(__s)) {
            return real_fwrite( __ptr, __size, __n, __s );
        }
        TRACE_SYSCALL();
        auto ret = ff_fwrite(__ptr, __size, __n, reinterpret_cast<FF_FILE*>(__s));
        errno = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver fwrite,fwrite@GLIBC_2.2.5");

    int getc(FILE *__stream)
    {
        TRACE_SYSCALL();
        if(!vfs::is_ff_handle(__stream)) {
            real_fprintf(stderr,"ERROR: It is not a ff file handle\n");
            errno = EINVAL;
            return -1;
        }
        auto ret = ff_fgetc(reinterpret_cast<FF_FILE *>(__stream));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver getc,getc@GLIBC_2.2.5");

    int putc(int __c, FILE *__stream)
    {
        TRACE_SYSCALL();
        if(!vfs::is_ff_handle(__stream)) {
            real_fprintf(stderr,"ERROR: It is not a ff file handle\n");
            errno = EINVAL;
            return -1;
        }
        auto ret = ff_fputc(__c, reinterpret_cast<FF_FILE *>(__stream));
        errno    = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver putc,putc@GLIBC_2.2.5");

    int remove (const char *__filename) __THROW
    {
        TRACE_SYSCALL();
        char namebuf[ffconfigMAX_FILENAME];
        auto ret = ff_remove(vfs::relative_to_root(namebuf,sizeof namebuf, __filename));
        if (ret && stdioGET_ERRNO() == EISDIR)
            ret = ff_deltree(vfs::relative_to_root(namebuf,sizeof namebuf,__filename), nullptr, nullptr);
        errno = stdioGET_ERRNO();
        return ret;
    }
    __asm__(".symver remove,remove@GLIBC_2.2.5");

    void rewind (FILE *__stream)
    {
        TRACE_SYSCALL();
        if(!vfs::is_ff_handle(__stream)) {
            real_fprintf(stderr,"ERROR: It is not a ff file handle\n");
            errno = EINVAL;
            return;
        }
        ff_fseek(reinterpret_cast<FF_FILE*>(__stream), 0L, SEEK_SET);
    }
    __asm__(".symver rewind,rewind@GLIBC_2.2.5");


    void setbuf (FILE *__restrict , char *__restrict ) __THROW
    {
        TRACE_SYSCALL();
        real_fprintf(stderr, "unimplemented call %s\n", __PRETTY_FUNCTION__ );
        errno = ENOTSUP;
    }
    __asm__(".symver setbuf,setbuf@GLIBC_2.2.5");

    int setvbuf (FILE *__restrict __stream, char *__restrict __buf,
                        int __modes, size_t __n) __THROW
    {
        TRACE_SYSCALL();
        real_fprintf(stderr, "unimplemented call %s\n", __PRETTY_FUNCTION__ );
        errno = ENOTSUP;
        return 0;
    }
    __asm__(".symver setvbuf,setbuf@GLIBC_2.2.5");


    void setbuffer (FILE *__restrict __stream, char *__restrict __buf,
                           size_t __size) __THROW
    {
        TRACE_SYSCALL();
        real_fprintf(stderr, "unimplemented call %s\n", __PRETTY_FUNCTION__ );
        errno = ENOTSUP;
    }
    __asm__(".symver setbuffer,setbuffer@GLIBC_2.2.5");

    /* Make STREAM line-buffered.  */
    void setlinebuf (FILE *__stream) __THROW
    {
        TRACE_SYSCALL();
        errno = ENOTSUP;
        real_fprintf(stderr, "unimplemented call %s\n", __PRETTY_FUNCTION__ );
    }
    __asm__(".symver setlinebuf,setlinebuf@GLIBC_2.2.5");

}
#pragma GCC diagnostic pop
