// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "iosyscalls-internal.hpp"

#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h> // for va_*
#include <limits.h> // for PATH_MAX
#include <string.h> // for strlen
#include <cassert>

#include "syscalls_real.hpp"

#include "debug.hpp"

namespace
{
    namespace real
    {
        __REAL_DECL(fprintf);
        __REAL_DECL(fwrite);
        __REAL_DECL(fread);
        __REAL_DECL(fopen);
        __REAL_DECL(fopen64);
        __REAL_DECL(fclose);
        __REAL_DECL(fputc);
        __REAL_DECL(fputs);
        __REAL_DECL(putc);
        __REAL_DECL(fgetc);
        __REAL_DECL(fgets);
        __REAL_DECL(getc);
        __REAL_DECL(freopen);
        __REAL_DECL(fdopen);
        __REAL_DECL(fseek);
        __REAL_DECL(ftell);
        __REAL_DECL(fgetpos);
        __REAL_DECL(fgetpos64);
        __REAL_DECL(fsetpos);
        __REAL_DECL(fsetpos64);
        __REAL_DECL(feof);
        __REAL_DECL(rewind);
        __REAL_DECL(fileno);
        __REAL_DECL(ferror);
        __REAL_DECL(fflush);
        __REAL_DECL(remove);
        __REAL_DECL(rename);
    } // namespace real

    void __attribute__((constructor)) _lib_stdio_initialize()
    {
        __REAL_DLSYM(fprintf);
        __REAL_DLSYM(fwrite);
        __REAL_DLSYM(fread);
        __REAL_DLSYM(fopen);
        __REAL_DLSYM(fopen64);
        __REAL_DLSYM(fclose);
        __REAL_DLSYM(fputc);
        __REAL_DLSYM(fputs);
        __REAL_DLSYM(putc);
        __REAL_DLSYM(fgetc);
        __REAL_DLSYM(fgets);
        __REAL_DLSYM(getc);
        __REAL_DLSYM(freopen);
        __REAL_DLSYM(fdopen);
        __REAL_DLSYM(fseek);
        __REAL_DLSYM(ftell);
        __REAL_DLSYM(fgetpos);
        __REAL_DLSYM(fgetpos64);
        __REAL_DLSYM(fsetpos);
        __REAL_DLSYM(fsetpos64);
        __REAL_DLSYM(feof);
        __REAL_DLSYM(rewind);
        __REAL_DLSYM(fileno);
        __REAL_DLSYM(ferror);
        __REAL_DLSYM(fflush);
        __REAL_DLSYM(remove);
        __REAL_DLSYM(rename);

        if (!(real::fprintf && real::fwrite && real::fread && real::fopen && real::fopen64 && real::fclose &&
              real::fputc && real::fputs && real::putc && real::fgetc && real::fgets && real::getc && real::freopen &&
              real::fdopen && real::fseek && real::ftell && real::fgetpos && real::fgetpos64 && real::fsetpos &&
              real::fsetpos64 && real::feof && real::rewind && real::fileno && real::ferror && real::fflush &&
              real::remove && real::rename)) {
            abort();
        }
    }

    auto fopen_to_open_flags(std::string_view flags)
    {
        int ret = 0;
        if (!flags.compare("r")) {
            ret = O_RDONLY;
        }
        else if (!flags.compare("w")) {
            ret = O_WRONLY | O_CREAT | O_TRUNC;
        }
        else if (!flags.compare("a")) {
            ret = O_WRONLY | O_CREAT | O_APPEND;
        }
        else if (!flags.compare("r+")) {
            ret = O_RDWR;
        }
        else if (!flags.compare("w+")) {
            ret = O_RDWR | O_CREAT | O_TRUNC;
        }
        else if (!flags.compare("a+")) {
            ret = O_RDWR | O_CREAT | O_APPEND;
        }
        return ret;
    }
} // namespace

extern "C"
{
    namespace vfs = vfsn::linux::internal;
    using FILEX   = vfs::FILEX;
    using fs      = purefs::fs::filesystem;

    FILE *_iosys_fopen(const char *pathname, const char *mode)
    {
        FILE *ret{};
        if (vfs::redirect_to_image(pathname)) {
            TRACE_SYSCALLN("(%s,%s) -> VFS", pathname, mode);
            const auto fd = vfs::invoke_fs(&fs::open, pathname, fopen_to_open_flags(mode), 0644);
            if (fd >= 0) {
                ret = reinterpret_cast<FILE *>(vfs::allocate_filex(fd));
            }
        }
        else {
            char tmp[PATH_MAX];
            const auto path = vfs::npath_translate(pathname, tmp);
            TRACE_SYSCALLN("(%s,%s) -> (%s) linux fs", pathname, mode, path);
            ret = real::fopen(path, mode);
        }
        TRACE_SYSCALLN("(%s,%s)=%p", pathname, mode, ret);
        return ret;
    }
    __asm__(".symver _iosys_fopen,fopen@GLIBC_2.2.5");

    FILE *_iosys_fopen64(const char *pathname, const char *mode)
    {
        FILE *ret{};
        if (vfs::redirect_to_image(pathname)) {
            TRACE_SYSCALLN("(%s,%s) -> fopen()", pathname, mode);
            return fopen(pathname, mode);
        }
        else {
            char tmp[PATH_MAX];
            const auto path = vfs::npath_translate(pathname, tmp);
            TRACE_SYSCALLN("(%s,%s) -> (%s) linux fs", pathname, mode, path);
            ret = real::fopen64(path, mode);
        }
        TRACE_SYSCALLN("(%s,%s)=%p", pathname, mode, ret);
        return ret;
    }
    __asm__(".symver _iosys_fopen64,fopen64@GLIBC_2.2.5");

    int _iosys_fclose(FILE *__stream)
    {
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p) -> VFS", __stream);
            auto fx  = reinterpret_cast<FILEX *>(__stream);
            auto ret = vfs::invoke_fs(&fs::close, fx->fd);
            if (!ret) {
                vfs::remove_filex(fx);
            }
            else {
                fx->error = errno;
            }
            return ret;
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            return real::fclose(__stream);
        }
    }
    __asm__(".symver _iosys_fclose,fclose@GLIBC_2.2.5");

    FILE *_iosys_fdopen(int __fd, const char *__modes) __THROW
    {
        if (vfs::is_image_fd(__fd)) {
            TRACE_SYSCALLN("(%d) -> VFS", __fd);
            std::cerr << "Unimplemented syscall " << __PRETTY_FUNCTION__ << std::endl;
            errno = ENOTSUP;
            return nullptr;
        }
        else {
            TRACE_SYSCALLN("(%d) -> linux fs", __fd);
            return real::fdopen(__fd, __modes);
        }
    }
    __asm__(".symver _iosys_fdopen,fdopen@GLIBC_2.2.5");

    int _iosys_feof(FILE *__stream) __THROW
    {
        int ret{};
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p) -> VFS", __stream);
            auto fx = reinterpret_cast<FILEX *>(__stream);
            do {
                const auto curr = vfs::invoke_fs(&fs::seek, fx->fd, 0, SEEK_CUR);
                if (curr < 0) {
                    ret = curr;
                    break;
                }
                const auto ends = vfs::invoke_fs(&fs::seek, fx->fd, 0, SEEK_END);
                if (ends < 0) {
                    ret = ends;
                    break;
                }
                const auto restored = vfs::invoke_fs(&fs::seek, fx->fd, curr, SEEK_SET);
                if (restored < 0) {
                    ret = restored;
                    break;
                }
                ret = curr >= ends;
            } while (0);
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            ret = real::feof(__stream);
        }
        TRACE_SYSCALLN("(%p)=%i", __stream, ret);
        return ret;
    }
    __asm__(".symver _iosys_feof,feof@GLIBC_2.2.5");

    int _iosys_ferror(FILE *stream) __THROW
    {
        if (vfs::is_filex(stream)) {
            TRACE_SYSCALLN("(%p) -> VFS", stream);
            auto fx = reinterpret_cast<FILEX *>(stream);
            return fx->error;
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", stream);
            return real::ferror(stream);
        }
    }
    __asm__(".symver _iosys_ferror,ferror@GLIBC_2.2.5");

    int _iosys_fflush(FILE *__stream)
    {
        int ret{};
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p) -> VFS", __stream);
            auto fx   = reinterpret_cast<FILEX *>(__stream);
            ret       = vfs::invoke_fs(&fs::fsync, fx->fd);
            fx->error = errno;
        }
        else {
            if (__stream != stdout && __stream != stderr)
                TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            ret = real::fflush(__stream);
        }
        if (__stream != stdout && __stream != stderr)
            TRACE_SYSCALLN("(%p)=%i", __stream, ret);
        return ret;
    }
    __asm__(".symver _iosys_fflush,fflush@GLIBC_2.2.5");

    int _iosys_fgetc(FILE *__stream)
    {
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p) -> VFS", __stream);
            auto fx = reinterpret_cast<FILEX *>(__stream);
            char ch;
            auto ret  = vfs::invoke_fs(&fs::read, fx->fd, &ch, 1);
            fx->error = errno;
            return ret;
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            return real::fgetc(__stream);
        }
    }
    __asm__(".symver _iosys_fgetc,fgetc@GLIBC_2.2.5");

    int _iosys_fgetpos(FILE *__restrict __stream, fpos_t *__restrict __pos)
    {
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p) -> VFS", __stream);
            auto fx   = reinterpret_cast<FILEX *>(__stream);
            auto ret  = vfs::invoke_fs(&fs::seek, fx->fd, 0, SEEK_CUR);
            fx->error = errno;
            if (__pos) {
                __pos->__pos = ret;
            }
            return (ret >= 0) ? (0) : (-1);
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            return real::fgetpos(__stream, __pos);
        }
    }
    __asm__(".symver _iosys_fgetpos,fgetpos@GLIBC_2.2.5");

    int _iosys_fgetpos64(FILE *__restrict __stream, fpos64_t *__restrict __pos)
    {
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p) -> VFS", __stream);
            auto fx   = reinterpret_cast<FILEX *>(__stream);
            auto ret  = vfs::invoke_fs(&fs::seek, fx->fd, 0, SEEK_CUR);
            fx->error = errno;
            if (__pos) {
                __pos->__pos = ret;
            }
            return (ret >= 0) ? (0) : (-1);
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            return real::fgetpos64(__stream, __pos);
        }
    }
    __asm__(".symver _iosys_fgetpos64,fgetpos64@GLIBC_2.2.5");

    char *_iosys_fgets(char *__restrict __s, int __n, FILE *__restrict __stream)
    {
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p) -> VFS", __stream);
            auto fx = reinterpret_cast<FILEX *>(__stream);
            char ch;
            size_t pos = 0;
            do {
                auto ret = vfs::invoke_fs(&fs::read, fx->fd, &ch, 1);
                if (ret == 0) {
                    fx->error = 0;
                    return nullptr;
                }
                else if (ret < 0) {
                    fx->error = errno;
                    return nullptr;
                }
                else {
                    __s[pos++] = ch;
                }
            } while (ch == '\n');
            return __s;
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            return real::fgets(__s, __n, __stream);
        }
    }
    __asm__(".symver _iosys_fgets,fgets@GLIBC_2.2.5");

    int _iosys_fileno(FILE *__stream) __THROW
    {
        int ret{};
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p) -> VFS", __stream);
            ret = vfs::to_native_fd(reinterpret_cast<FILEX *>(__stream)->fd);
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            ret = real::fileno(__stream);
        }
        TRACE_SYSCALLN("(%p)=%i", __stream, ret);
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

        if (!vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            va_list arglist;
            va_start(arglist, __format);
            auto ret = vfprintf(__stream, __format, arglist);
            va_end(arglist);
            return ret;
        }

        TRACE_SYSCALLN("(%p) -> VFS", __stream);
        pcBuffer = new char[buf_len];
        auto fx  = reinterpret_cast<FILEX *>(__stream);
        if (pcBuffer == NULL) {
            /* Store the errno to thread local storage. */
            fx->error = ENOMEM;
            iCount    = -1;
        }
        else {
            va_start(xArgs, __format);
            iCount = vsnprintf(pcBuffer, buf_len, __format, xArgs);
            va_end(xArgs);

            /* ff_fwrite() will set ff_errno. */
            if (iCount > 0) {
                xResult = vfs::invoke_fs(&fs::write, fx->fd, pcBuffer, iCount);
                if (xResult < (size_t)iCount) {
                    iCount = -1;
                }
            }
            delete[] pcBuffer;
        }
        fx->error = errno;
        return iCount;
    }
    __asm__(".symver _iosys_fprintf,fprintf@GLIBC_2.2.5");

    int _iosys_fputc(int __c, FILE *__stream)
    {
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p,%d) -> VFS", __stream, __c);
            auto fx   = reinterpret_cast<FILEX *>(__stream);
            char ch   = __c;
            int ret   = vfs::invoke_fs(&fs::write, fx->fd, &ch, sizeof ch);
            fx->error = errno;
            return ret == 1 ? 0 : ret;
        }
        else {
            if (__stream != stdout && __stream != stderr)
                TRACE_SYSCALLN("(%p,%d) -> linux fs", __stream, __c);
            return real::fputc(__c, __stream);
        }
    }
    __asm__(".symver _iosys_fputc,fputc@GLIBC_2.2.5");

    int _iosys_fputs(const char *__restrict __s, FILE *__restrict __stream)
    {
        int ret{};
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p,%p) -> VFS", __s, __stream);
            auto fx        = reinterpret_cast<FILEX *>(__stream);
            const auto len = strlen(__s);
            ret            = vfs::invoke_fs(&fs::write, fx->fd, __s, len);
            fx->error      = errno;
            ret            = ret == int(len) ? 0 : -1;
        }
        else {
            if (__stream != stdout && __stream != stderr)
                TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            ret = real::fputs(__s, __stream);
        }
        if (__stream != stdout && __stream != stderr)
            TRACE_SYSCALLN("(%s, %p)=%i", __s, __stream, ret);
        return ret;
    }
    __asm__(".symver _iosys_fputs,fputs@GLIBC_2.2.5");

    size_t _iosys_fread(void *__restrict __ptr, size_t __size, size_t __n, FILE *__restrict __stream)
    {
        size_t ret{};
        if (__size != 0 && __n != 0) {
            if (vfs::is_filex(__stream)) {
                TRACE_SYSCALLN("(%p) -> VFS", __stream);
                auto fx   = reinterpret_cast<FILEX *>(__stream);
                char *p   = reinterpret_cast<char *>(__ptr);
                bool eof  = false;
                auto size = __size * __n;
                assert(__size == (size / __n));
                while (size && !eof) {
                    size_t readsize = (size > 8192) ? 8192 : size;
                    auto res        = vfs::invoke_fs(&fs::read, fx->fd, p, readsize);
                    fx->error       = errno;
                    if (res < 0)
                        break;
                    if (size_t(res) < readsize)
                        eof = true;
                    p += res;
                    ret += res;
                    size -= res;
                }
            }
            else {
                TRACE_SYSCALLN("(%p) -> linux fs", __stream);
                ret = real::fread(__ptr, __size, __n, __stream);
            }
        }
        TRACE_SYSCALLN("(%p, %lu, %lu, %p)=%i", __ptr, __size, __n, __stream, ret);
        return ret;
    }
    __asm__(".symver _iosys_fread,fread@GLIBC_2.2.5");

    FILE *_iosys_freopen(const char *__restrict __filename, const char *__restrict __modes, FILE *__restrict __stream)
    {
        if (vfs::is_filex(__filename)) {
            TRACE_SYSCALLN("(%s,%s) -> VFS", __filename, __modes);
            if (fclose(__stream) < 0) {
                return nullptr;
            }
            return fopen(__filename, __modes);
        }
        else {
            TRACE_SYSCALLN("(%s,%s) -> linux fs", __filename, __modes);
            return real::freopen(__filename, __modes, __stream);
        }
    }
    __asm__(".symver _iosys_freopen,freopen@GLIBC_2.2.5");

    int _iosys_fseek(FILE *__stream, long int __off, int __whence)
    {
        int ret{};
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p) -> VFS", __stream);
            auto fx = reinterpret_cast<FILEX *>(__stream);
            ret     = vfs::invoke_fs(&fs::seek, fx->fd, __off, __whence);
            ret     = (ret > 0) ? 0 : ret;
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            ret = real::fseek(__stream, __off, __whence);
        }
        TRACE_SYSCALLN("(%p, %li, %i)=%i", __stream, __off, __whence, ret);
        return ret;
    }
    __asm__(".symver _iosys_fseek,fseek@GLIBC_2.2.5");

    int _iosys_fsetpos(FILE *__stream, const fpos_t *__pos)
    {
        TRACE_SYSCALL();
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p) -> VFS", __stream);
            auto fx  = reinterpret_cast<FILEX *>(__stream);
            auto ret = vfs::invoke_fs(&fs::seek, fx->fd, __pos->__pos, SEEK_SET);
            return (ret > 0) ? 0 : ret;
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            return real::fsetpos(__stream, __pos);
        }
    }
    __asm__(".symver _iosys_fsetpos,fsetpos@GLIBC_2.2.5");

    int _iosys_fsetpos64(FILE *__stream, const fpos64_t *__pos)
    {
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p) -> VFS", __stream);
            auto fx  = reinterpret_cast<FILEX *>(__stream);
            auto ret = vfs::invoke_fs(&fs::seek, fx->fd, __pos->__pos, SEEK_SET);
            return (ret > 0) ? 0 : ret;
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            return real::fsetpos64(__stream, __pos);
        }
    }
    __asm__(".symver _iosys_fsetpos64,fsetpos64@GLIBC_2.2.5");

    long int _iosys_ftell(FILE *__stream)
    {
        long int ret{};
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p) -> VFS", __stream);
            auto fx = reinterpret_cast<FILEX *>(__stream);
            ret     = vfs::invoke_fs(&fs::seek, fx->fd, 0, SEEK_CUR);
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            ret = real::ftell(__stream);
        }
        TRACE_SYSCALLN("(%p)=%i", __stream, ret);
        return ret;
    }
    __asm__(".symver _iosys_ftell,ftell@GLIBC_2.2.5");

    size_t _iosys_fwrite(const void *__restrict __ptr, size_t __size, size_t __n, FILE *__restrict __s)
    {
        int ret{};
        if (__size != 0 && __n != 0) {
            if (vfs::is_filex(__s)) {
                TRACE_SYSCALLN("(%p) -> VFS", __s);
                auto fx       = reinterpret_cast<FILEX *>(__s);
                const char *p = reinterpret_cast<const char *>(__ptr);
                bool eos      = false;
                auto size     = __size * __n;
                assert(__size == (size / __n));
                while (size && !eos) {
                    size_t wrsize = (size > 8192) ? 8192 : size;
                    auto res      = vfs::invoke_fs(&fs::write, fx->fd, p, wrsize);
                    fx->error     = errno;
                    if (res < 0)
                        break;
                    if (size_t(res) < wrsize)
                        eos = true;
                    p += res;
                    ret += res;
                    size -= res;
                }
            }
            else {
                if (__s != stdout && __s != stderr)
                    TRACE_SYSCALLN("(%p) -> linux fs", __s);
                ret = real::fwrite(__ptr, __size, __n, __s);
            }
        }
        if (__s != stdout && __s != stderr)
            TRACE_SYSCALLN("(ptr: %p,size: %lu, n: %lu, fil: %p)=%i", __ptr, __size, __n, __s, ret);
        return ret;
    }
    __asm__(".symver _iosys_fwrite,fwrite@GLIBC_2.2.5");

    int _iosys_getc(FILE *__stream)
    {
        int ret{};
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p) -> VFS", __stream);
            auto fx = reinterpret_cast<FILEX *>(__stream);
            char ch;
            ret       = vfs::invoke_fs(&fs::read, fx->fd, &ch, 1);
            fx->error = errno;
            ret       = (ret == 1) ? (ch) : (ret);
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            ret = real::getc(__stream);
        }
        TRACE_SYSCALLN("(%p)=%i", __stream, ret);
        return ret;
    }
    __asm__(".symver _iosys_getc,getc@GLIBC_2.2.5");

    int _iosys_putc(int __c, FILE *__stream)
    {
        int ret{};
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p) -> VFS", __stream);
            auto fx   = reinterpret_cast<FILEX *>(__stream);
            char ch   = __c;
            ret       = vfs::invoke_fs(&fs::write, fx->fd, &ch, sizeof ch);
            fx->error = errno;
            ret       = (ret == 1) ? (__c) : (EOF);
        }
        else {
            if (__stream != stdout && __stream != stderr)
                TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            ret = real::putc(__c, __stream);
        }
        if (__stream != stdout && __stream != stderr)
            TRACE_SYSCALLN("(%i %p)=%i", __c, __stream, ret);
        return ret;
    }
    __asm__(".symver _iosys_putc,putc@GLIBC_2.2.5");

    int _iosys_remove(const char *__filename) __THROW
    {
        int ret{};
        if (vfs::redirect_to_image(__filename)) {
            TRACE_SYSCALLN("(%s) -> VFS", __filename);
            ret = vfs::invoke_fs(&fs::unlink, __filename);
        }
        else {
            char tmp[PATH_MAX];
            const auto npath = vfs::npath_translate(__filename, tmp);
            TRACE_SYSCALLN("(%s) -> (%s) linux fs", __filename, npath);
            ret = real::remove(npath);
        }
        TRACE_SYSCALLN("(%s)=%i", __filename, ret);
        return ret;
    }
    __asm__(".symver _iosys_remove,remove@GLIBC_2.2.5");

    int _iosys_rename(const char *oldpath, const char *newpath)
    {
        TRACE_SYSCALL();
        if (vfs::redirect_to_image(oldpath)) {
            TRACE_SYSCALLN("(%s,%s) -> VFS", oldpath, newpath);
            return vfs::invoke_fs(&fs::rename, oldpath, newpath);
        }
        else {
            char tmp[PATH_MAX], tmp2[PATH_MAX];
            const auto oldp = vfs::npath_translate(oldpath, tmp);
            const auto newp = vfs::npath_translate(newpath, tmp2);
            TRACE_SYSCALLN("(%s,%s) -> (%s,%s) linux fs", oldpath, newpath, oldp, newp);
            return real::rename(oldp, newp);
        }
    }
    __asm__(".symver _iosys_rename,rename@GLIBC_2.2.5");

    void _iosys_rewind(FILE *__stream)
    {
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p) -> VFS", __stream);
            auto fx = reinterpret_cast<FILEX *>(__stream);
            vfs::invoke_fs(&fs::seek, fx->fd, 0, SEEK_SET);
            fx->error = errno;
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            real::rewind(__stream);
        }
    }
    __asm__(".symver _iosys_rewind,rewind@GLIBC_2.2.5");

    void _iosys_setbuf(FILE *__restrict, char *__restrict) __THROW
    {
        TRACE_SYSCALL();
        errno = ENOTSUP;
    }
    __asm__(".symver _iosys_setbuf,setbuf@GLIBC_2.2.5");

    int _iosys_setvbuf(FILE *__restrict __stream, char *__restrict __buf, int __modes, size_t __n) __THROW
    {
        TRACE_SYSCALL();
        errno = ENOTSUP;
        return 0;
    }
    __asm__(".symver _iosys_setvbuf,setvbuf@GLIBC_2.2.5");

    void _iosys_setbuffer(FILE *__restrict __stream, char *__restrict __buf, size_t __size) __THROW
    {
        TRACE_SYSCALL();
        errno = ENOTSUP;
    }
    __asm__(".symver _iosys_setbuffer,setbuffer@GLIBC_2.2.5");

    /* Make STREAM line-buffered.  */
    void _iosys_setlinebuf(FILE *__stream) __THROW
    {
        TRACE_SYSCALL();
        errno = ENOTSUP;
    }
    __asm__(".symver _iosys_setlinebuf,setlinebuf@GLIBC_2.2.5");
}
