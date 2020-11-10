// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
//
#include "vfs.hpp"
#include "vfs_paths.hpp"
#include "ff_image_user_disk.hpp"

namespace
{
    constexpr auto image_name = "PurePhone.img";
}

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
    emmcFFDisk = freertos_fat::internals::diskInit(purefs::dir::eMMC_disk, image_name);

    /* Print out information on the disk. */
    freertos_fat::internals::diskShowPartition(emmcFFDisk);

    bootConfig.os_root_path = purefs::dir::eMMC_disk;

    if (loadBootConfig(getCurrentBootJSON())) {
        LOG_INFO("vfs::Init osType %s root:%s", bootConfig.os_type.c_str(), bootConfig.os_root_path.c_str());
        if (ff_chdir(bootConfig.os_root_path.c_str()) != 0) {
            LOG_ERROR("vfs::Init can't chdir to %s", bootConfig.os_root_path.c_str());
        }
    }
    else {
        LOG_ERROR("vfs::Init unable to determine OS type, fallback to %s", bootConfig.os_root_path.c_str());
    }

    LOG_INFO("vfs::Init running on ARM osRootPath: %s", bootConfig.os_root_path.c_str());

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
