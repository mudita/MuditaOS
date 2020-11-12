// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

//#include <stdio.h>
#include <ff_stdio.h>
#include <errno.h>
#include <ctype.h>
#include "debug.hpp"
namespace {


    int ic(FF_FILE *fp)
    {
        char ch;
        return ff_fread( &ch, 1, 1, fp);
    }


    int istr(FF_FILE *fp, char *dst, int wid)
    {
        char *d = dst;
        int c;
        while ((c = ic(fp)) != EOF && wid-- > 0 && !isspace(c)) {
            *d++ = c;
        }
        *d = '\0';
        ungetc(c, reinterpret_cast<FILE*>(fp));
        return d == dst;
    }
    /* t is 1 for char, 2 for short, 4 for int, and 8 for long */
    int iint(FF_FILE *fp, void *dst, int t, int wid)
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
extern "C" {

    int ungetc (int __c, FILE *__stream)
    {
        TRACE_SYSCALL();
        int ret = ff_fseek(reinterpret_cast<FF_FILE*>(__stream), -1, SEEK_CUR );
        if( ret ) {
            errno = stdioGET_ERRNO();
            return ret;
        }
        char ch = __c;
        ret = ff_fwrite(&ch, 1, 1, reinterpret_cast<FF_FILE*>(__stream) );
        if(ret!=1) {
            errno = stdioGET_ERRNO();
            return -1;
        }
        errno = stdioGET_ERRNO();
        return 0;
    }
    __asm__(".symver ungetc,vfscanf@GLIBC_2.2.5");




    int vfscanf (FILE *__restrict fp, const char *__restrict fmt,
                    __gnuc_va_list ap)
    {
        TRACE_SYSCALL();
        int ret = 0;
        int t, c;
        int wid = 1 << 20;
        while (*fmt) {
            while (isspace(static_cast<unsigned char>(*fmt))) {
                fmt++;
            }
            while (isspace(c = ic(reinterpret_cast<FF_FILE*>(fp))))
                ;
            ungetc(c, fp);
            while (*fmt && *fmt != '%' && !isspace((unsigned char) *fmt))
                if (*fmt++ != ic(reinterpret_cast<FF_FILE*>(fp)))
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
                if (iint(reinterpret_cast<FF_FILE*>(fp), va_arg(ap, long *), t, wid))
                    return ret;
                ret++;
                break;
            case 's':
                if (istr(reinterpret_cast<FF_FILE*>(fp), va_arg(ap, char *), wid))
                    return ret;
                ret++;
                break;
            }
        }
        return ret;
    }
    __asm__(".symver vfscanf,vfscanf@GLIBC_2.2.5");

    int fscanf (FILE *__restrict fp,
                   const char *__restrict fmt, ...) __wur
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
