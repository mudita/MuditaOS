// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <dlfcn.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <mutex>
#include <unordered_map>
#include <iosyscalls.hpp>
#include <fcntl.h>
#include <stdarg.h>
#include <limits.h>

#include "debug.hpp"

namespace
{
    int (*real_fprintf)(FILE *__restrict __stream, const char *__restrict __format, ...);
    size_t (*real_fwrite)(const void *__restrict __ptr, size_t __size, size_t __n, FILE *__restrict __s);
    int (*real_fputs)(const char *__restrict __s, FILE *__restrict __stream);
    int (*real_fputc)(int __c, FILE *__stream);
    int (*real_putc) (int __c, FILE *__stream);
    int (*real_fileno)(FILE *__stream) __THROW __wur;
    int (*real_vfprintf)(FILE *__restrict __s, const char *__restrict __format, __gnuc_va_list __arg);
    FILE *(*real_fopen) (const char *__restrict __filename, const char *__restrict __modes) __wur;
    int (*real_fclose) (FILE *__stream);
    int (*real_getc) (FILE *__stream);
    int (*real_feof) (FILE *__stream) __THROW __wur;
    char *(*real_fgets) (char *__restrict __s, int __n, FILE *__restrict __stream) __wur;
    int (*real_ferror) (FILE *__stream) __THROW __wur;
    int (*real_fflush) (FILE *__stream);
    int (*real_fgetc) (FILE *__stream);
    int (*real_fgetpos) (FILE *__restrict __stream, fpos_t *__restrict __pos);
    int (*real_fgetpos64) (FILE *__restrict __stream, fpos64_t *__restrict __pos);
    size_t (*real_fread) (void *__restrict __ptr, size_t __size, size_t __n, FILE *__restrict __stream) __wur;
    FILE *(*real_freopen) (const char *__restrict __filename, const char *__restrict __modes, FILE *__restrict __stream) __wur;
    int (*real_fseek) (FILE *__stream, long int __off, int __whence);
    int (*real_fsetpos) (FILE *__stream, const fpos_t *__pos);
    int (*real_fsetpos64) (FILE *__stream, const fpos64_t *__pos);
    long int (*real_ftell) (FILE *__stream) __wur;
    void (*real_rewind) (FILE *__stream);


    void __attribute__((constructor)) _lib_stdio_initialize()
    {
        real_fprintf = reinterpret_cast<decltype(real_fprintf)>(dlsym(RTLD_NEXT, "fprintf"));
        real_fwrite = reinterpret_cast<decltype(real_fwrite)>(dlsym(RTLD_NEXT, "fwrite"));
        real_fputs = reinterpret_cast<decltype(real_fputs)>(dlsym(RTLD_NEXT, "fputs"));
        real_fputc = reinterpret_cast<decltype(real_fputc)>(dlsym(RTLD_NEXT, "fputc"));
        real_putc = reinterpret_cast<decltype(real_putc)>(dlsym(RTLD_NEXT, "putc"));
        real_fileno = reinterpret_cast<decltype(real_fileno)>(dlsym(RTLD_NEXT, "fileno"));
        real_vfprintf = reinterpret_cast<decltype(real_vfprintf)>(dlsym(RTLD_NEXT, "vfprintf"));
        real_fopen = reinterpret_cast<decltype(real_fopen)>(dlsym(RTLD_NEXT, "fopen"));
        real_fclose = reinterpret_cast<decltype(real_fclose)>(dlsym(RTLD_NEXT, "fclose"));
        real_getc = reinterpret_cast<decltype(real_getc)>(dlsym(RTLD_NEXT, "getc"));
        real_feof = reinterpret_cast<decltype(real_feof)>(dlsym(RTLD_NEXT, "feof"));
        real_fgets = reinterpret_cast<decltype(real_fgets)>(dlsym(RTLD_NEXT, "fgets"));
        real_ferror = reinterpret_cast<decltype(real_ferror)>(dlsym(RTLD_NEXT, "ferror"));
        real_fgetc = reinterpret_cast<decltype(real_fgetc)>(dlsym(RTLD_NEXT, "fgetc"));
        real_fgetpos = reinterpret_cast<decltype(real_fgetpos)>(dlsym(RTLD_NEXT, "fgetpos"));
        real_fgetpos64 = reinterpret_cast<decltype(real_fgetpos64)>(dlsym(RTLD_NEXT, "fgetpos64"));
        real_fread = reinterpret_cast<decltype(real_fread)>(dlsym(RTLD_NEXT, "fread"));
        real_freopen = reinterpret_cast<decltype(real_freopen)>(dlsym(RTLD_NEXT, "freopen"));
        real_fseek = reinterpret_cast<decltype(real_fseek)>(dlsym(RTLD_NEXT, "fseek"));
        real_fsetpos = reinterpret_cast<decltype(real_fsetpos)>(dlsym(RTLD_NEXT, "fsetpos"));
        real_fsetpos64 = reinterpret_cast<decltype(real_fsetpos64)>(dlsym(RTLD_NEXT, "fsetpos64"));
        real_ftell = reinterpret_cast<decltype(real_ftell)>(dlsym(RTLD_NEXT, "ftell"));
        real_rewind = reinterpret_cast<decltype(real_rewind)>(dlsym(RTLD_NEXT, "rewind"));
        real_fflush = reinterpret_cast<decltype(real_fflush)>(dlsym(RTLD_NEXT, "fflush"));
        if(!real_fprintf || !real_fwrite || !real_fputs || !real_fputc
            || !real_fileno || !real_vfprintf || !real_putc || !real_fopen
            || !real_fclose || !real_getc || !real_feof || !real_fgets
            || !real_ferror || !real_fflush || ! real_fgetc || !real_fgetpos
            || !real_fgetpos64 || !real_fread || !real_freopen || !real_fseek
            || !real_fsetpos || !real_fsetpos64 || !real_ftell || !real_rewind )
        {
            abort();
        }
    }
    auto fopen_to_open_flags(std::string_view flags)
    {
        int ret = 0;
        if(!flags.compare("r")) {
            ret = O_RDONLY;
        }
        else if(!flags.compare("w")) {
            ret = O_WRONLY | O_CREAT | O_TRUNC;
        }
        else if(!flags.compare("a"))
        {
            ret = O_WRONLY | O_CREAT | O_APPEND;
        }
        else if(!flags.compare("r+"))
        {
            ret = O_RDWR;
        }
        else if(!flags.compare("w+"))
        {
            ret = O_RDWR | O_CREAT | O_TRUNC;
        }
        else if(!flags.compare("a+"))
        {
            ret = O_RDWR | O_CREAT | O_APPEND;
        }
        return ret;
    }
}


extern "C"
{
    using namespace vfsn::linux::internal;

    FILE *_iosys_fopen(const char *pathname, const char *mode)
    {
        FILE* ret {};
        if(redirect_to_image(pathname))
        {
            TRACE_SYSCALLN("(%s,%s) -> VFS", pathname, mode);
            const auto fd = invoke_fs(&purefs::fs::filesystem::open, pathname, fopen_to_open_flags(mode),0644);
            if(fd >= 0) {
                ret = reinterpret_cast<FILE*>(allocate_filex(fd));
            }
        }
        else
        {
            TRACE_SYSCALLN("(%s,%s) -> linux fs", pathname, mode);
            char tmp[PATH_MAX];
            const auto path = npath_translate(pathname,tmp);
            ret = real_fopen(path,mode);
        }
        TRACE_SYSCALLN("(%s,%s)=%p", pathname, mode, ret);
        return ret;
    }
    __asm__(".symver _iosys_fopen,fopen@GLIBC_2.2.5");

    FILE *_iosys_fopen64(const char *pathname, const char *mode)
    {
        TRACE_SYSCALLN("(%s,%s)", pathname, mode);
        return fopen(pathname,mode);
    }
    __asm__(".symver _iosys_fopen64,fopen64@GLIBC_2.2.5");

    int _iosys_fclose(FILE *__stream)
    {
        if(is_filex(__stream))
        {
            TRACE_SYSCALLN("(%p) -> VFS", __stream);
            auto fx = reinterpret_cast<FILEX*>(__stream);
            auto ret = invoke_fs(&purefs::fs::filesystem::close, fx->fd);
            if(!ret)
            {
                remove_filex(fx);
            }
            else
            {
                fx->error = errno;
            }
            return ret;
        }
        else
        {
            TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            return real_fclose(__stream);
        }
    }
    __asm__(".symver _iosys_fclose,fclose@GLIBC_2.2.5");

    FILE *_iosys_fdopen(int __fd, const char *__modes) __THROW
    {
        TRACE_SYSCALL();
        std::cerr << "Unimplemented syscall " <<  __PRETTY_FUNCTION__ << std::endl;
        errno = ENOTSUP;
        return nullptr;
    }
    __asm__(".symver _iosys_fdopen,fdopen@GLIBC_2.2.5");

    int _iosys_feof(FILE *__stream) __THROW
    {
        int ret {};
        if(is_filex(__stream))
        {
            auto fx = reinterpret_cast<FILEX*>(__stream);
            do {
                const auto curr = invoke_fs(&purefs::fs::filesystem::seek,fx->fd,0,SEEK_CUR);
                if(curr<0) {
                    ret = curr;
                    break;
                }
                const auto ends = invoke_fs(&purefs::fs::filesystem::seek,fx->fd,0,SEEK_END);
                if(ends<0) {
                    ret = ends;
                    break;
                }
                const auto restored = invoke_fs(&purefs::fs::filesystem::seek,fx->fd,curr,SEEK_SET);
                if(restored<0) {
                    ret = restored;
                    break;
                }
                ret = curr >= ends;
            } while(0);
        }
        else
        {
            ret = real_feof(__stream);
        }
        TRACE_SYSCALLN("(%p)=%i",__stream,ret);
        return ret;
    }
    __asm__(".symver _iosys_feof,feof@GLIBC_2.2.5");

    int _iosys_ferror(FILE * stream) __THROW
    {
        TRACE_SYSCALL();
        if(is_filex(stream))
        {
            auto fx = reinterpret_cast<FILEX*>(stream);
            return fx->error;
        }
        else
        {
            return real_ferror(stream);
        }
    }
    __asm__(".symver _iosys_ferror,ferror@GLIBC_2.2.5");

    int _iosys_fflush(FILE *__stream)
    {
        int ret {};
        if(is_filex(__stream))
        {
            auto fx = reinterpret_cast<FILEX*>(__stream);
            ret = invoke_fs(&purefs::fs::filesystem::fsync, fx->fd);
            fx->error = errno;
        }
        else
        {
            ret = real_fflush(__stream);
        }
        TRACE_SYSCALLN("(%p)=%i",__stream,ret);
        return ret;
    }
    __asm__(".symver _iosys_fflush,fflush@GLIBC_2.2.5");

    int _iosys_fgetc(FILE *__stream)
    {
        TRACE_SYSCALL();
        if(is_filex(__stream))
        {
            auto fx = reinterpret_cast<FILEX*>(__stream);
            char ch;
            auto ret = invoke_fs(&purefs::fs::filesystem::read,fx->fd,&ch,1);
            fx->error = errno;
            return ret;
        }
        else
        {
            return real_fgetc(__stream);
        }
    }
    __asm__(".symver _iosys_fgetc,fgetc@GLIBC_2.2.5");

    int _iosys_fgetpos(FILE *__restrict __stream, fpos_t *__restrict __pos)
    {
        TRACE_SYSCALL();
        if(is_filex(__stream))
        {
            auto fx = reinterpret_cast<FILEX*>(__stream);
            auto ret = invoke_fs(&purefs::fs::filesystem::seek,fx->fd,0,SEEK_CUR);
            fx->error = errno;
            if(__pos) {
                __pos->__pos =  ret;
            }
            return (ret>=0)?(0):(-1);
        }
        else
        {
            return real_fgetpos(__stream, __pos);
        }
    }
    __asm__(".symver _iosys_fgetpos,fgetpos@GLIBC_2.2.5");

    int _iosys_fgetpos64(FILE *__restrict __stream, fpos64_t *__restrict __pos)
    {
        TRACE_SYSCALL();
        if(is_filex(__stream))
        {
            auto fx = reinterpret_cast<FILEX*>(__stream);
            auto ret = invoke_fs(&purefs::fs::filesystem::seek,fx->fd,0,SEEK_CUR);
            fx->error = errno;
            if(__pos) {
                __pos->__pos =  ret;
            }
            return (ret>=0)?(0):(-1);
        }
        else
        {
            return real_fgetpos64(__stream, __pos);
        }
    }
    __asm__(".symver _iosys_fgetpos64,fgetpos64@GLIBC_2.2.5");

    char *_iosys_fgets(char *__restrict __s, int __n, FILE *__restrict __stream)
    {
        TRACE_SYSCALL();
        if(is_filex(__s))
        {
            auto fx = reinterpret_cast<FILEX*>(__stream);
            char ch;
            size_t pos = 0;
            do {
                auto ret = invoke_fs(&purefs::fs::filesystem::read,fx->fd,&ch,1);
                if(ret == 0)
                {
                    fx->error = 0;
                    return nullptr;
                }
                else if(ret<0)
                {
                    fx->error = errno;
                    return nullptr;
                } else {
                    __s[pos++] = ch;
                }
            } while(ch == '\n');
            return __s;
        }
        else
        {
            return real_fgets(__s,__n,__stream);
        }
    }
    __asm__(".symver _iosys_fgets,fgets@GLIBC_2.2.5");

    int _iosys_fileno(FILE *__stream) __THROW
    {
        int ret {};
        if(is_filex(__stream))
        {
            auto fx = reinterpret_cast<FILEX*>(__stream);
            ret =  native_fd_to_image_fd(fx->fd);
        }
        else
        {
            ret = real_fileno(__stream);
        }
        TRACE_SYSCALLN("(%p)=%i",__stream,ret);
        return ret;
    }
    __asm__(".symver _iosys_fileno,fileno@GLIBC_2.2.5");

    int _iosys_fprintf(FILE *__restrict __stream, const char *__restrict __format, ...)
    {
        constexpr auto buf_len = 4096;
        int iCount;
        size_t xResult;
        char *pcBuffer;
        va_list xArgs;

        if(!is_filex(__stream))
        {
            va_list arglist;
            va_start( arglist, __format );
            auto ret = real_vfprintf( __stream, __format, arglist );
            va_end( arglist );
            return ret;
        }
        TRACE_SYSCALL();
        pcBuffer = new char[buf_len];
        auto fx = reinterpret_cast<FILEX*>(__stream);
        if (pcBuffer == NULL) {
            /* Store the errno to thread local storage. */
            fx->error = ENOMEM;
            iCount = -1;
        }
        else {
            va_start(xArgs, __format);
            iCount = vsnprintf(pcBuffer, buf_len, __format, xArgs);
            va_end(xArgs);

            /* ff_fwrite() will set ff_errno. */
            if (iCount > 0) {
                xResult = invoke_fs(&purefs::fs::filesystem::write, fx->fd, pcBuffer, iCount);
                if (xResult < (size_t)iCount) {
                    iCount = -1;
                }
            }
            delete [] pcBuffer;
        }
        fx->error = errno;
        return iCount;
    }
    __asm__(".symver _iosys_fprintf,fprintf@GLIBC_2.2.5");

    int _iosys_fputc(int __c, FILE *__stream)
    {
        if(!is_filex(__stream))
        {
            return real_fputc( __c, __stream );
        }
        else
        {
            TRACE_SYSCALL();
            auto fx = reinterpret_cast<FILEX*>(__stream);
            char ch = __c;
            int ret = invoke_fs(&purefs::fs::filesystem::write,fx->fd, &ch, sizeof ch);
            fx->error = errno;
            return ret==1?0:ret;
        }
    }
    __asm__(".symver _iosys_fputc,fputc@GLIBC_2.2.5");

    int _iosys_fputs(const char *__restrict __s, FILE *__restrict __stream)
    {
        int ret {};
        if(is_filex(__stream))
        {
            auto fx = reinterpret_cast<FILEX*>(__stream);
            const auto len = strlen(__s);
            ret = invoke_fs(&purefs::fs::filesystem::write,fx->fd, __s, len);
            fx->error = errno;
            ret = ret==int(len)?0:-1;
        }
        else
        {
            ret = real_fputs( __s, __stream );
        }
        TRACE_SYSCALLN("(%s, %p)=%i",__s, __stream, ret);
        return ret;
    }
    __asm__(".symver _iosys_fputs,fputs@GLIBC_2.2.5");

    size_t _iosys_fread(void *__restrict __ptr, size_t __size, size_t __n, FILE *__restrict __stream)
    {
        size_t ret {};
        if(__size!=0 && __n!=0)
        {
            if(is_filex(__stream))
            {
                auto fx = reinterpret_cast<FILEX*>(__stream);
                char* p = reinterpret_cast<char*>(__ptr);
                do {
                    auto res = invoke_fs(&purefs::fs::filesystem::read, fx->fd, p, __size);
                    const auto eof = res>0 && size_t(res)<__size;
                    fx->error = errno;
                    if(res<0 || eof) break;
                    p += __size;
                    --__n;
                    ++ret;
                } while(__n > 0 );
            }
            else
            {
                ret = real_fread(__ptr,__size,__n,__stream);
            }
        }
        TRACE_SYSCALLN("(%p, %lu, %lu, %p)=%i",__ptr,__size,__n,__stream, ret);
        return ret;
    }
    __asm__(".symver _iosys_fread,fread@GLIBC_2.2.5");


    FILE *_iosys_freopen (const char *__restrict __filename,
                      const char *__restrict __modes,
                      FILE *__restrict __stream)
    {
        TRACE_SYSCALL();
        if(!is_filex(__filename))
        {
            return real_freopen(__filename,__modes,__stream);
        }
        else {
            if( fclose(__stream) < 0) {
                return nullptr;
            }
            return fopen(__filename, __modes );
        }
    }
    __asm__(".symver _iosys_freopen,freopen@GLIBC_2.2.5");

    int _iosys_fseek (FILE *__stream, long int __off, int __whence)
    {
        int ret {};
        if(is_filex(__stream))
        {
            auto fx = reinterpret_cast<FILEX*>(__stream);
            ret = invoke_fs(&purefs::fs::filesystem::seek, fx->fd, __off, __whence);
            ret = ret>0?0:ret;
        }
        else
        {
            ret = real_fseek(__stream,__off,__whence);
        }
        TRACE_SYSCALLN("(%p, %li, %i)=%i",__stream,__off,__whence,ret);
        return ret;
    }
    __asm__(".symver _iosys_fseek,fseek@GLIBC_2.2.5");

    int _iosys_fsetpos (FILE *__stream, const fpos_t *__pos)
    {
        TRACE_SYSCALL();
        if(!is_filex(__stream))
        {
            return real_fsetpos(__stream,__pos);
        }
        else
        {
            auto fx = reinterpret_cast<FILEX*>(__stream);
            auto ret = invoke_fs(&purefs::fs::filesystem::seek, fx->fd, __pos->__pos, SEEK_SET);
            return ret>0?0:ret;
        }
    }
    __asm__(".symver _iosys_fsetpos,fsetpos@GLIBC_2.2.5");

    int _iosys_fsetpos64 (FILE *__stream, const fpos64_t *__pos)
    {
        TRACE_SYSCALL();
        if(!is_filex(__stream)) {
            return real_fsetpos64(__stream,__pos);
        }
        else
        {
            auto fx = reinterpret_cast<FILEX*>(__stream);
            auto ret = invoke_fs(&purefs::fs::filesystem::seek, fx->fd, __pos->__pos, SEEK_SET);
            return ret>0?0:ret;
        }
    }
    __asm__(".symver _iosys_fsetpos64,fsetpos64@GLIBC_2.2.5");


    long int _iosys_ftell (FILE *__stream)
    {
        long int ret {};
        if(is_filex(__stream))
        {
            auto fx = reinterpret_cast<FILEX*>(__stream);
            ret = invoke_fs(&purefs::fs::filesystem::seek,fx->fd,0,SEEK_CUR);
        }
        else
        {
            ret = real_ftell(__stream);
        }
        TRACE_SYSCALLN("(%p)=%i",__stream, ret);
        return ret;
    }
    __asm__(".symver _iosys_ftell,ftell@GLIBC_2.2.5");

    size_t _iosys_fwrite (const void *__restrict __ptr, size_t __size,
                          size_t __n, FILE *__restrict __s)
    {
        int ret {};
        if(__size != 0 && __n != 0)
        {
            if(is_filex(__s))
            {
                auto fx = reinterpret_cast<FILEX*>(__s);
                const char* p = reinterpret_cast<const char*>(__ptr);
                size_t items {};
                do {
                    auto ret = invoke_fs(&purefs::fs::filesystem::write, fx->fd, p, __size);
                    const auto eof = ret>=0 && size_t(ret)!=__size;
                    fx->error = errno;
                    if(ret<0 || eof) return ret;
                    p += __size;
                    --__n;
                    ++items;
                } while(__n > 0 );
                ret = ret<0?(-1):(items);
            }
            else
            {
                ret = real_fwrite( __ptr, __size, __n, __s );
            }
        }
        if (__s != stdout && __s != stderr)
            TRACE_SYSCALLN("(ptr: %p,size: %lu, n: %lu, fil: %p)=%i",__ptr,__size,__n,__s,ret);
        return ret;
    }
    __asm__(".symver _iosys_fwrite,fwrite@GLIBC_2.2.5");

    int _iosys_getc(FILE *__stream)
    {
        int ret {};
        if(is_filex(__stream))
        {
            auto fx = reinterpret_cast<FILEX*>(__stream);
            char ch;
            ret = invoke_fs(&purefs::fs::filesystem::read,fx->fd,&ch,1);
            fx->error = errno;
            ret = (ret==1)?(ch):(ret);
        }
        else
        {
            ret = real_getc(__stream);
        }
        TRACE_SYSCALLN("(%p)=%i",__stream, ret);
        return ret;
    }
    __asm__(".symver _iosys_getc,getc@GLIBC_2.2.5");

    int _iosys_putc(int __c, FILE *__stream)
    {
        int ret {};
        if(is_filex(__stream))
        {
            auto fx = reinterpret_cast<FILEX*>(__stream);
            char ch = __c;
            ret = invoke_fs(&purefs::fs::filesystem::write,fx->fd, &ch, sizeof ch);
            fx->error = errno;
            ret =  (ret==1)?(__c):(EOF);
        }
        else
        {
            ret = real_fputc( __c, __stream );
        }
        TRACE_SYSCALLN("(%i %p)=%i",__c,__stream, ret);
        return ret;
    }
    __asm__(".symver _iosys_putc,putc@GLIBC_2.2.5");

    int _iosys_remove (const char *__filename) __THROW
    {
        int ret {};
        if(redirect_to_image(__filename))
        {
            ret = invoke_fs(&purefs::fs::filesystem::unlink, __filename);
        }
        else
        {
            char tmp[PATH_MAX];
            const auto npath = npath_translate(__filename,tmp);
            auto r_remove = reinterpret_cast<int (*)(const char*)>(dlsym(RTLD_NEXT,"remove"));
            ret = r_remove(npath);
        }
        TRACE_SYSCALLN("(%s)=%i",__filename, ret);
        return ret;
    }
    __asm__(".symver _iosys_remove,remove@GLIBC_2.2.5");

    void _iosys_rewind (FILE *__stream)
    {
        TRACE_SYSCALLN("(%p)",__stream);
        if(is_filex(__stream))
        {
            auto fx = reinterpret_cast<FILEX*>(__stream);
            invoke_fs(&purefs::fs::filesystem::seek,fx->fd,0,SEEK_SET);
            fx->error = errno;
        }
        else
        {
            real_rewind(__stream);
        }
    }
    __asm__(".symver _iosys_rewind,rewind@GLIBC_2.2.5");


    void _iosys_setbuf (FILE *__restrict , char *__restrict ) __THROW
    {
        TRACE_SYSCALL();
        std::cerr << "Unimplemented syscall " <<  __PRETTY_FUNCTION__ << std::endl;
        errno = ENOTSUP;
    }
    __asm__(".symver _iosys_setbuf,setbuf@GLIBC_2.2.5");

    int _iosys_setvbuf (FILE *__restrict __stream, char *__restrict __buf,
                        int __modes, size_t __n) __THROW
    {
        TRACE_SYSCALL();
        std::cerr << "Unimplemented syscall " <<  __PRETTY_FUNCTION__ << std::endl;
        errno = ENOTSUP;
        return 0;
    }
    __asm__(".symver _iosys_setvbuf,setbuf@GLIBC_2.2.5");


    void _iosys_setbuffer (FILE *__restrict __stream, char *__restrict __buf,
                           size_t __size) __THROW
    {
        TRACE_SYSCALL();
        std::cerr << "Unimplemented syscall " <<  __PRETTY_FUNCTION__ << std::endl;
        errno = ENOTSUP;
    }
    __asm__(".symver _iosys_setbuffer,setbuffer@GLIBC_2.2.5");

    /* Make STREAM line-buffered.  */
    void _iosys_setlinebuf (FILE *__stream) __THROW
    {
        TRACE_SYSCALL();
        errno = ENOTSUP;
        std::cerr << "Unimplemented syscall " <<  __PRETTY_FUNCTION__ << std::endl;
    }
    __asm__(".symver _iosys_setlinebuf,setlinebuf@GLIBC_2.2.5");

}
