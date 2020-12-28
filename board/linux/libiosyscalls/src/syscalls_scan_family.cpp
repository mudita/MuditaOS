// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>
#include "debug.hpp"
#include <iosyscalls.hpp>
#include <dlfcn.h>

namespace
{
    int (*real_fprintf)(FILE *__restrict __stream, const char *__restrict __format, ...);
    int (*real_vfscanf) (FILE *__restrict __s, const char *__restrict __format, __gnuc_va_list __arg) __wur;
    int (*real_ungetc) (int __c, FILE *__stream);

    void __attribute__((constructor)) _syscalls_scan_family()
    {
        real_fprintf = reinterpret_cast<decltype(real_fprintf)>(dlsym(RTLD_NEXT, "fprintf"));
        real_ungetc = reinterpret_cast<decltype(real_ungetc)>(dlsym(RTLD_NEXT, "ungetc"));
        real_vfscanf = reinterpret_cast<decltype(real_vfscanf)>(dlsym(RTLD_NEXT, "vfscanf"));
        if(!real_fprintf || !real_ungetc || !real_vfscanf) {
            abort();
        }
    }
}

namespace
{

    using namespace vfsn::linux::internal;
    int ic(FILEX *fp)
    {
        char ch;
        if(fp->ungetchar>0)
        {
            ch = fp->ungetchar;
            fp->ungetchar = -1;
            return 0;
        }
        else
        {
            auto ret = invoke_fs(&purefs::fs::filesystem::read,fp->fd,&ch,1);
            fp->error = errno;
            return ret==1?0:ret;
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
        //ungetc(c, fp);
        return d == dst;
    }
    /* t is 1 for char, 2 for short, 4 for int, and 8 for long */
    int iint(FILEX *fp, void *dst, int t, int wid)
    {
        long n = 0;
        int c;
        int neg = 0;
        c = ic(fp);
        if (c == '-') {
            neg = 1;
        }
        if ((c == '-' || c == '+') && wid-- > 0) {
            c = ic(fp);
        }
        if (!isdigit(c) || wid <= 0) {
            ungetc(c, reinterpret_cast<FILE*>(fp));
            return 1;
        }
        do {
            n = n * 10 + c - '0';
        } while (isdigit(c = ic(fp)) && --wid > 0);
        ungetc(c, reinterpret_cast<FILE*>(fp));
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
}
extern "C"
{

    using namespace vfsn::linux::internal;
    int ungetc (int __c, FILE *__stream)
    {
        TRACE_SYSCALL();
        if(!is_filex(__stream))
        {
            return real_ungetc(__c,__stream);
        }
        else
        {
            auto fx = reinterpret_cast<FILEX*>(__stream);
            fx->ungetchar = __c;
            return 0;
        }
    }
    __asm__(".symver ungetc,vfscanf@GLIBC_2.2.5");

    int vfscanf (FILE *__restrict fp, const char *__restrict fmt,
                    __gnuc_va_list ap)
    {
        if(!is_filex(fp))
        {
            return real_vfscanf(fp,fmt,ap);
        }
        TRACE_SYSCALL();
        int ret = 0;
        int t, c;
        int wid = 1 << 20;
        auto fx = reinterpret_cast<FILEX*>(fp);
        while (*fmt) {
            while (isspace(static_cast<unsigned char>(*fmt))) {
                fmt++;
            }
            while (isspace(c = ic(fx)))
                ;
            ungetc(c, fp);
            while (*fmt && *fmt != '%' && !isspace((unsigned char) *fmt))
                if (*fmt++ != ic(fx))
                    return ret;
            if (*fmt != '%')
                continue;
            fmt++;
            if (isdigit((unsigned char) *fmt)) {
                wid = 0;
                while (isdigit((unsigned char) *fmt))
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
    __asm__(".symver vfscanf,vfscanf@GLIBC_2.2.5");

    int fscanf (FILE *__restrict fp,
                   const char *__restrict fmt, ...)
    {
        TRACE_SYSCALL();
        va_list ap;
        int ret;
        va_start(ap, fmt);
        ret = vfscanf(fp, fmt, ap);
        va_end(ap);
        return ret;
    }
    __asm__(".symver fscanf,fscanf@GLIBC_2.2.5");
}
