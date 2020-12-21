// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
//
#include "vfs.hpp"
#include <purefs/filesystem_paths.hpp>
#include "ff_image_user_disk.hpp"
#include <cstring>

namespace
{
    constexpr auto image_name = "PurePhone.img";
}

// NOTE: Ugly hack relative to root we don't want to use C++ in linking
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

//! Ugly hack for preload will be removed later
namespace vfsn::linux::internal
{
    const char *relative_to_root(char *out_path, size_t out_path_len, const char *inpath)
    {
        return std::strncpy(out_path, vfs.relativeToRoot(inpath).c_str(), out_path_len);
    }
    bool vfs_is_initialized()
    {
        return vfs.isInitialized();
    }
} // namespace vfsn::linux::internal

vfs::vfs()
{}

vfs::~vfs()
{
    freertos_fat::internals::diskDelete(emmcFFDisk);
}

void vfs::Init()
{
    if (emmcFFDisk) {
        LOG_WARN("Disk manager already initialized");
        return;
    }
    emmcFFDisk = freertos_fat::internals::diskInit(purefs::dir::getRootDiskPath().c_str(), image_name);

    /* Print out information on the disk. */
    freertos_fat::internals::diskShowPartition(emmcFFDisk);

    initDone = true;
    int err  = bootConfig.load();
    if (!err) {
        if (ff_chdir(bootConfig.os_root_path().c_str()) != 0) {
            LOG_ERROR("vfs::Init can't chdir to %s", bootConfig.os_root_path().c_str());
        }
        LOG_INFO("vfs::Init osType %s root:%s", bootConfig.os_type().c_str(), bootConfig.os_root_path().c_str());
        if (ff_chdir(bootConfig.os_root_path().c_str()) != 0) {
            LOG_ERROR("vfs::Init can't chdir to %s", bootConfig.os_root_path().c_str());
        }
    }
    else {
        LOG_ERROR("vfs::Init unable to determine OS type, fallback to %s", bootConfig.os_root_path().c_str());
    }

    LOG_INFO("vfs::Init running on ARM osRootPath: %s", bootConfig.os_root_path().c_str());

    // this should already exist and have user data in it
    // if it does not create an exmpty directory so that sqlite3 does not fault
    if (const auto userDiskPath = purefs::dir::getUserDiskPath(); isDir(userDiskPath.c_str()) == false) {
        LOG_ERROR("vfs::Init looks like %s does not exist, try to create it", userDiskPath.c_str());
        if (ff_mkdir(userDiskPath.c_str()) != 0) {
            LOG_ERROR("vfs::Init can't create %s directory", userDiskPath.c_str());
        }
    }
    else {
        LOG_INFO("vfs::Init looks like %s exists", userDiskPath.c_str());
    }
    chnNotifier.onFileSystemInitialized();
}
#pragma GCC diagnostic pop
