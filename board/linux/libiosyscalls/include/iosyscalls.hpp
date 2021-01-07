// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <stddef.h>
#include <stdio.h>
#include <purefs/vfs_subsystem.hpp>

namespace vfsn::linux::internal
{
    bool redirect_to_image();
    bool is_image_handle(const FILE* fil);
    int native_fd_to_image_fd(int fd);
    int image_fd_to_native_fd(int fd);
    bool is_image_fd(int fd);
    bool redirect_to_image(const char* inpath);
    const char* npath_translate(const char* inpath, char *buffer);

    struct FILEX {
        int fd {0};
        int error {0};
        int ungetchar {-1};
    };
    FILEX* allocate_filex(int fd);
    bool is_filex(const void* fd);
    void remove_filex(FILEX *fil);

    template <class Base, typename T, typename... Args>
        auto invoke_fs( T Base::*lfs_fun, Args&& ... args)
        -> decltype((static_cast<Base *>(nullptr)->*lfs_fun)(std::forward<Args>(args)...))
    {
        auto vfs = purefs::subsystem::vfs_core();
        if(!vfs) {
            errno = EIO;
            return -1;
        }
        auto ret = (vfs.get()->*lfs_fun)(std::forward<Args>(args)...);
        if(ret < 0) {
            errno = -ret;
            ret = -1;
        }
        return ret;
    }
}

