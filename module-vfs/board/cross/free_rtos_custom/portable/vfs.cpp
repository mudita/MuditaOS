// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "vfs.hpp"
#include <purefs/filesystem_paths.hpp>
#include "ff_eMMC_user_disk.hpp"

vfs::vfs() : emmc()
{}

vfs::~vfs()
{
    FF_eMMC_user_DiskDelete(emmcFFDisk);
    emmc.DeInit();
}

void vfs::Init()
{
    emmc.Init();

    emmcFFDisk = FF_eMMC_user_DiskInit(purefs::dir::getRootDiskPath().c_str(), &emmc);

    /* Print out information on the disk. */
    FF_eMMC_user_DiskShowPartition(emmcFFDisk);

    initDone = true;
    int err  = bootConfig.load();
    if (!err) {
        LOG_INFO("vfs::Init osType %s root:%s", bootConfig.os_type().c_str(), bootConfig.os_root_path().c_str());
        if (ff_chdir(bootConfig.os_root_path().c_str()) != 0) {
            LOG_ERROR("vfs::Init can't chdir to %s", bootConfig.os_root_path().c_str());
        }
    }
    else {
        LOG_ERROR("vfs::Init unable to determine OS type, fallback to %s", bootConfig.os_root_path().c_str());
    }

    chnNotifier.onFileSystemInitialized();
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
}
