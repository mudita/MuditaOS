// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <iosyscalls.hpp>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <parallel_hashmap/phmap.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <debug.hpp>
#include <dirent.h>

namespace
{
    constexpr auto ENV_NAME       = "IOSYSCALLS_REDIRECT_TO_IMAGE";
    constexpr auto FIRST_FILEDESC = 64'566'756;
    bool g_evaluated              = false;
    bool g_redirect               = false;

    constexpr const char *LINUX_PATHS[]{"/dev/",
                                        "/etc/",
                                        "/tmp/",
                                        "/sys/.boot.json",
                                        "/usr/share",
                                        "/run/user",
                                        "/home",
                                        "/proc",
                                        "/dev/shm",
                                        "PurePhone.img",
                                        "MuditaOS.log",
                                        nullptr};

    constexpr const char *IMAGE_PATHS[]{"/sys", nullptr};

    pthread_mutex_t g_lock = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    phmap::flat_hash_set<vfsn::linux::internal::FILEX *> g_fdlist;
    phmap::flat_hash_set<DIR *> g_dirlist;
} // namespace

namespace vfsn::linux::internal
{
    bool redirect_to_image()
    {
        if (!g_evaluated) {
            const auto env = std::getenv(ENV_NAME);
            g_redirect     = env && !std::strcmp(env, "1");
            g_evaluated    = true;
        }
        return g_redirect;
    }

    bool redirect_to_image(const char *inpath)
    {
        if (!redirect_to_image())
            return false;

        for (auto path = LINUX_PATHS; *path; ++path)
            if (std::strstr(inpath, *path) == inpath)
                return false;

        return true;
    }

    const char *npath_translate(const char *inpath, char *buffer)
    {
        for (auto path = IMAGE_PATHS; *path; ++path)
            if (std::strstr(inpath, *path) == inpath) {
                std::strncpy(buffer, inpath + 1, PATH_MAX);
                return buffer;
            }
        return inpath;
    }

    bool is_image_handle(const FILE *fil)
    {
        return false;
    }

    int to_native_fd(int fd)
    {
        return FIRST_FILEDESC + fd;
    }

    int to_image_fd(int fd)
    {
        return fd - FIRST_FILEDESC;
    }

    bool is_image_fd(int fd)
    {
        return fd >= FIRST_FILEDESC;
    }

    FILEX *allocate_filex(int fd)
    {
        auto ret = new FILEX;
        ret->fd  = fd;
        pthread_mutex_lock(&g_lock);
        g_fdlist.emplace(ret);
        pthread_mutex_unlock(&g_lock);
        return ret;
    }

    void add_DIR_to_image_list(DIR *indir)
    {
        pthread_mutex_lock(&g_lock);
        g_dirlist.emplace(indir);
        pthread_mutex_unlock(&g_lock);
    }

    void remove_DIR_from_image_list(DIR *indir)
    {
        pthread_mutex_lock(&g_lock);
        auto fres = g_dirlist.find(indir);
        if (fres != g_dirlist.end()) {
            g_dirlist.erase(fres);
        }
        pthread_mutex_unlock(&g_lock);
    }

    bool is_image_DIR(DIR *indir)
    {
        if (indir == nullptr) {
            return false;
        }
        pthread_mutex_lock(&g_lock);
        auto fres    = g_dirlist.find(indir);
        auto isntdir = (fres != g_dirlist.end());
        pthread_mutex_unlock(&g_lock);
        return isntdir;
    }

    bool is_filex(const void *fd)
    {
        if (fd == nullptr) {
            return false;
        }
        pthread_mutex_lock(&g_lock);
        auto fres    = g_fdlist.find(reinterpret_cast<const FILEX *>(fd));
        auto isfilex = (fres != g_fdlist.end());
        pthread_mutex_unlock(&g_lock);
        return isfilex;
    }

    void remove_filex(FILEX *fil)
    {
        pthread_mutex_lock(&g_lock);
        auto fres = g_fdlist.find(fil);
        if (fres != g_fdlist.end()) {
            g_fdlist.erase(fres);
            delete fil;
        }
        pthread_mutex_unlock(&g_lock);
    }

    void debug_trace_syscall(const char *fn, const char *format, ...)
    {
        auto rvprint = reinterpret_cast<decltype(&vfprintf)>(dlsym(RTLD_NEXT, "vfprintf"));
        auto rprint  = reinterpret_cast<decltype(&fprintf)>(dlsym(RTLD_NEXT, "fprintf"));
        rprint(stderr, ">>>>>>> IOFUNC: [%s] ", fn);
        va_list args;
        va_start(args, format);
        rvprint(stderr, format, args);
        va_end(args);
        rprint(stderr, " >>>>>>>>>>>\n");
    }
} // namespace vfsn::linux::internal
