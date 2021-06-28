// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "iosyscalls-internal.hpp"

extern "C"
{
    namespace vfs = vfsn::linux::internal;

    void iosyscalls_set_sysroot(const char *newSysroot)
    {
        vfs::set_sysroot(newSysroot);
    }

    void iosyscalls_set_image_name(const char *newImageName)
    {
        vfs::set_image_path(newImageName);
    }
}
