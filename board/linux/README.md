
# board-linux

This directory contains the necessary files for the Linux simulator filesystem. To have the same filesystem interface for the Linux simulator
and RT1051 we have to translate paths. It is done by wrappers which translate path and then invoke real syscall from
`glibc`. The advantage of this solution is that we don't need to translate the path in the application layer, everything is done by low-level
wrappers. So we don't need to modify third-party libraries to translate path. However, we need to maintain `glibc`'s versions if we change the compiler or libraries. 


## How does it work

If `opendir` function is invoked 
``` c
DIR *dir;
if ((dir = opendir (".")) == NULL) {
    perror ("Cannot open .");
    exit (1);
}
//...
```
it is linked to our wrapper as `_iosys_opendir`
``` c
DIR *_iosys_opendir(const char *dirname)
{
    __dirstream *ret{};
    if (!dirname) {
        //...
        errno = EINVAL;
        return ret;
    }
    if (vfs::redirect_to_image(dirname)) {
        //...
    }
    else {
        char tmp[PATH_MAX];
        // Path translation
        const auto newpath = vfs::npath_translate(dirname, tmp);
        // Invoke opendir from glibc
        ret = real::opendir(newpath);
    }
    return ret;
}
__asm__(".symver _iosys_opendir,opendir@GLIBC_2.2.5");
```
As you see it is done by `.symver _iosys_opendir,opendir@GLIBC_2.2.5` which links it with a proper version of `glibc` function.

```
        +--------------+                      +--------------------------+                  +-----------------+
        |              |    invoke wrapper    | _iosys_opendir(...) {    |   invoke glibc   |                 |
        | opendir(...) |--------------------->| opendir(translated_path) |----------------->| real::opendir() |
        |              |                      | }                        |                  |                 |
        +--------------+                      +--------------------------+                  +-----------------+
```

## How to find correct version
Our library doesn't handle all syscalls because we don't know the exact syscall version. While changing compiler or some library
it will be necessary to update the version or add a new wrapper. 

Example:
Let's say we have a new compiler and there is `stdc++` implementation that uses a different `stat` syscall or different version (see Related links).
We can easily find the correct version by checking symbols.
```
$ readelf -Ws /usr/lib/x86_64-linux-gnu/libstdc++.so.6 | grep -w stat
   176: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND stat@GLIBC_2.33 (66)
```
## How to update syscalls wrappers
### Update version.txt
This file is used by linker to tell which internal names to replace. Before editing check `glibc` version and do not duplicate symbols.
```
GLIBC_2.33 {
    global:
        stat;   # updated stat version inserting to GLIBC_2.33
        lstat;
        fstat;
        stat64;
        lstat64;
        fstat64;
};
```
### Add or update wrapper
Updating the wrapper version is easy. You need to modify only `@GLIBC` string according to symbols found previously.
```
__asm__(".symver _iosys_stat,stat@GLIBC_<version>");
```
However if you need to add a new wrapper the best way is just to check other implementations. But finally it should look like:
```
_iosys_<function_name>(...)
{
    // VFS implementation if needed
    if (...) {
        //...
    }
    else {
        // Path translation if needed
        // Invoke function_name from glibc id needed
    }
}
__asm__(".symver _iosys_<function_name>,function_name@GLIBC_<version>");
```
### Add declarations (if needed)
If you added a wrapper then you must add helpers for intercepting library calls in `real` namespace. 
These functions take a handle of a dynamic loaded shared object and return the address where that
symbol is loaded into memory.
- `__REAL_DECL(function_name)` - just `decltype`
- `__REAL_DLSYM(function_name)` obtain address of a symbol in a shared object or
       executable

## Related

[GNU C library backward compatibility](https://developers.redhat.com/blog/2019/08/01/how-the-gnu-c-library-handles-backward-compatibility)

[GNU C library syscalls versions](https://fossies.org/linux/glibc/io/Versions)

[DLSYM Manual](https://man7.org/linux/man-pages/man3/dlsym.3.html)