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

namespace {
    constexpr auto ENV_NAME = "IOSYSCALLS_REDIRECT_TO_IMAGE";
    constexpr auto FIRST_FILEDESC = 64'566'756;
    bool g_readed = false ;
    bool g_redirect = false;
    constexpr const char * ALLOWED_PATHS[] = {
        "/dev/",
        "/etc/",
        "/sys/",
        "PurePhone.img",
        nullptr,
    };
    constexpr const char * EXCLUDED_PATHS[] = {
        "/sys/user",
        "/sys/factory-test",
        "/sys/current/assets",
        "/sys/updates",
        nullptr,
    };

    constexpr const char * TRANSLATION_PATHS[] =
    {
        "sys/user",
        "sys/factory-test",
        "assets",
        "sys/updates",
        nullptr
    };

    pthread_mutex_t g_lock = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    phmap::flat_hash_set<vfsn::linux::internal::FILEX*> g_fdlist;
}

namespace vfsn::linux::internal
{
    bool redirect_to_image()
    {
        if(!g_readed) {
            const auto env = std::getenv(ENV_NAME);
            g_redirect = env && !std::strcmp(env,"1");
            g_readed = true;
        }
        return g_redirect;
    }

    bool redirect_to_image(const char* inpath)
    {
        auto native_redirect = redirect_to_image();
        auto redirect = native_redirect;
        if(native_redirect) {
            for(auto path=ALLOWED_PATHS; *path; ++path) {
                if( std::strstr(inpath,*path)==inpath) {
                    redirect = false;
                    break;
                }
            }
        }
        if(native_redirect && !redirect)
        {
            for(auto path=EXCLUDED_PATHS; *path; ++path) {
                if( std::strstr(inpath,*path)==inpath) {
                    redirect = true;
                    break;
                }
            }
        }
        return redirect;
    }

    const char* npath_translate(const char* inpath, char *buffer)
    {
        for(size_t trans_pos=0; EXCLUDED_PATHS[trans_pos]; ++trans_pos) {
            if( std::strstr(inpath,EXCLUDED_PATHS[trans_pos])==inpath) {
                std::strncpy(buffer,TRANSLATION_PATHS[trans_pos],PATH_MAX);
                std::strncat(buffer,inpath+std::strlen(EXCLUDED_PATHS[trans_pos]), PATH_MAX);
                return buffer;
            }
        }
        return inpath;
    }

    bool is_image_handle(const FILE* fil)
    {
        return false;
    }

    int native_fd_to_image_fd(int fd)
    {
        return FIRST_FILEDESC + fd;
    }

    int image_fd_from_native_fd(int fd)
    {
        if(fd < FIRST_FILEDESC ) {
            return -1;
        } else {
            return fd - FIRST_FILEDESC;
        }
    }

    bool is_image_fd(int fd)
    {
        return fd >= FIRST_FILEDESC;
    }

    FILEX* allocate_filex(int fd)
    {
        auto ret = new FILEX;
        ret->fd = fd;
        pthread_mutex_lock(&g_lock);
        g_fdlist.emplace( ret );
        pthread_mutex_unlock(&g_lock);
        return ret;
    }

    bool is_filex(const void* fd)
    {
        if(fd==nullptr) {
            return false;
        }
        pthread_mutex_lock(&g_lock);
        auto fres = g_fdlist.find(reinterpret_cast<const FILEX*>(fd));
        auto isfilex = ( fres != g_fdlist.end() );
        pthread_mutex_unlock(&g_lock);
        return isfilex;
    }

    void remove_filex(FILEX *fil)
    {
        pthread_mutex_lock(&g_lock);
        auto fres = g_fdlist.find(fil);
        if(fres != g_fdlist.end())
        {
            g_fdlist.erase(fres);
            delete fil;
        }
        pthread_mutex_unlock(&g_lock);
    }

    void debug_trace_syscall(const char* fn, const char* format, ...)
    {
        auto rvprint = reinterpret_cast<decltype(&vfprintf)>(dlsym(RTLD_NEXT, "vfprintf"));
        auto rprint = reinterpret_cast<decltype(&fprintf)>(dlsym(RTLD_NEXT, "fprintf"));
        rprint(stderr,">>>>>>> IOFUNC: [%s] ", fn);
        va_list args;
        va_start(args, format);
        rvprint(stderr, format, args);
        va_end(args);
        rprint(stderr," >>>>>>>>>>>\n");
    }
}
