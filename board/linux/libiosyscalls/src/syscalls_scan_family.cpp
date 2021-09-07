// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "iosyscalls-internal.hpp"

#include "debug.hpp"
#include "syscalls_real.hpp"

#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>

namespace
{
    namespace real
    {
        __REAL_DECL(ungetc);
        __REAL_DECL(vfscanf);
    } // namespace real

    void __attribute__((constructor)) _syscalls_scan_family()
    {
        __REAL_DLSYM(ungetc);
        __REAL_DLSYM(vfscanf);

        if (!(real::ungetc && real::vfscanf)) {
            abort();
        }
    }
} // namespace

namespace
{
    namespace vfs = vfsn::linux::internal;
    using FILEX   = vfs::FILEX;
    using fs      = purefs::fs::filesystem;

    int ic(FILEX *fp)
    {
        char ch;
        if (fp->ungetchar > 0) {
            ch            = fp->ungetchar;
            fp->ungetchar = -1;
            return 0;
        }
        else {
            auto ret  = vfs::invoke_fs(&fs::read, fp->fd, &ch, 1);
            fp->error = errno;
            return ret == 1 ? 0 : ret;
        }
    }

    int istr(FILEX *fp, char *dst, int wid)
    {
        char *d = dst;
        int c;
        while ((c = ic(fp)) != EOF && wid-- > 0 && !isspace(c)) {
            *d++ = c;
        }
        *d = '\0';
        // ungetc(c, fp);
        return d == dst;
    }
    /* t is 1 for char, 2 for short, 4 for int, and 8 for long */
    int iint(FILEX *fp, void *dst, int t, int wid)
    {
        long n = 0;
        int c;
        int neg = 0;
        c       = ic(fp);
        if (c == '-') {
            neg = 1;
        }
        if ((c == '-' || c == '+') && wid-- > 0) {
            c = ic(fp);
        }
        if (!isdigit(c) || wid <= 0) {
            ungetc(c, reinterpret_cast<FILE *>(fp));
            return 1;
        }
        do {
            n = n * 10 + c - '0';
        } while (isdigit(c = ic(fp)) && --wid > 0);
        ungetc(c, reinterpret_cast<FILE *>(fp));
        if (t == 8) {
            *reinterpret_cast<long *>(dst) = neg ? -n : n;
        }
        else if (t == 4) {
            *reinterpret_cast<int *>(dst) = neg ? -n : n;
        }
        else if (t == 2) {
            *reinterpret_cast<short *>(dst) = neg ? -n : n;
        }
        else {
            *reinterpret_cast<char *>(dst) = neg ? -n : n;
        }
        return 0;
    }
} // namespace
extern "C"
{
    namespace vfs = vfsn::linux::internal;
    using FILEX   = vfs::FILEX;

    int ungetc(int __c, FILE *__stream)
    {
        if (vfs::is_filex(__stream)) {
            TRACE_SYSCALLN("(%p) -> VFS", __stream);
            auto fx       = reinterpret_cast<FILEX *>(__stream);
            fx->ungetchar = __c;
            return 0;
        }
        else {
            TRACE_SYSCALLN("(%p) -> linux fs", __stream);
            return real::ungetc(__c, __stream);
        }
    }
    /* WARNING:
     *   this implementation of ungetc() is work-in-progress
     *   and should remain local until FILEX buffering is implemented!
     */
    //    __asm__(".symver _iosys_ungetc,ungetc@GLIBC_2.2.5");

    int _iosys_vfscanf(FILE *__restrict fp, const char *__restrict fmt, __gnuc_va_list ap)
    {
        if (!vfs::is_filex(fp)) {
            TRACE_SYSCALLN("(%p) -> linux fs", fp);
            return real::vfscanf(fp, fmt, ap);
        }
        TRACE_SYSCALLN("(%p) -> VFS", fp);
        int ret = 0;
        int t, c;
        int wid = 1 << 20;
        auto fx = reinterpret_cast<FILEX *>(fp);
        while (*fmt) {
            while (isspace(static_cast<unsigned char>(*fmt))) {
                fmt++;
            }
            while (isspace(c = ic(fx)))
                ;
            ungetc(c, fp);
            while (*fmt && *fmt != '%' && !isspace((unsigned char)*fmt))
                if (*fmt++ != ic(fx))
                    return ret;
            if (*fmt != '%')
                continue;
            fmt++;
            if (isdigit((unsigned char)*fmt)) {
                wid = 0;
                while (isdigit((unsigned char)*fmt))
                    wid = wid * 10 + *fmt++ - '0';
            }
            t = sizeof(int);
            while (*fmt == 'l') {
                t = sizeof(long);
                fmt++;
            }
            while (*fmt == 'h') {
                t = unsigned(t) < sizeof(int) ? sizeof(char) : sizeof(short);
                fmt++;
            }
            switch (*fmt++) {
            case 'u':
            case 'd':
                if (iint((fx), va_arg(ap, long *), t, wid))
                    return ret;
                ret++;
                break;
            case 's':
                if (istr((fx), va_arg(ap, char *), wid))
                    return ret;
                ret++;
                break;
            }
        }
        return ret;
    }
    __asm__(".symver _iosys_vfscanf,vfscanf@GLIBC_2.2.5");

    int _iosys_fscanf(FILE *__restrict fp, const char *__restrict fmt, ...)
    {
        TRACE_SYSCALLN("(%p) -> vfscanf()", fp);
        va_list ap;
        int ret;
        va_start(ap, fmt);
        ret = vfscanf(fp, fmt, ap);
        va_end(ap);
        return ret;
    }
    __asm__(".symver _iosys_fscanf,fscanf@GLIBC_2.2.5");
}
