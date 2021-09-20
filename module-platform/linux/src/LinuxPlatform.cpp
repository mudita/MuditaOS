// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <platform/linux/LinuxPlatform.hpp>

#include "BlockDeviceFactory.hpp"

#include <iosyscalls.hpp>
#include <purefs/vfs_subsystem.hpp>

#include <exception>

using platform::linux::BlockDeviceFactory;
using platform::linux::LinuxPlatform;

LinuxPlatform::LinuxPlatform(std::string imageName) : imageName(std::move(imageName))
{}

LinuxPlatform::~LinuxPlatform()
{
    if (usesFilesystem) {
        purefs::subsystem::unmount_all();
    }
}

void LinuxPlatform::init()
{
    initFilesystem();
}

void LinuxPlatform::initFilesystem()
{
    if (usesFilesystem) {
        throw std::runtime_error("Filesystem already initialized");
    }

    iosyscalls_set_image_name(imageName.c_str());

    auto blockDeviceFactory = std::make_unique<BlockDeviceFactory>(imageName);
    vfs                     = purefs::subsystem::initialize(std::move(blockDeviceFactory));

    if (int err = purefs::subsystem::mount_defaults(); err != 0) {
        throw std::runtime_error("Failed to initiate filesystem: " + std::to_string(err));
    }

    usesFilesystem = true;
}
