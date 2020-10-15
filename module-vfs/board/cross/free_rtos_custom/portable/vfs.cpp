//
// Created by lucck on 14.10.2020.
//
#include "vfs.hpp"
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

    emmcFFDisk = FF_eMMC_user_DiskInit(purefs::dir::eMMC_disk.c_str(), &emmc);

    /* Print out information on the disk. */
    FF_eMMC_user_DiskShowPartition(emmcFFDisk);

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
    if (isDir(purefs::dir::user_disk.c_str()) == false) {
        LOG_ERROR("vfs::Init looks like %s does not exist, try to create it", purefs::dir::user_disk.c_str());
        if (ff_mkdir(purefs::dir::user_disk.c_str()) != 0) {
            LOG_ERROR("vfs::Init can't create %s directory", purefs::dir::user_disk.c_str());
        }
    }
    else {
        LOG_INFO("vfs::Init looks like %s exists", purefs::dir::user_disk.c_str());
    }
}
