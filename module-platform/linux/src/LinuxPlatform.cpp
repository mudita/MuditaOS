// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <platform/linux/LinuxPlatform.hpp>

#include "BlockDeviceFactory.hpp"

#include <bsp/bsp.hpp>
#include <purefs/vfs_subsystem.hpp>
#include <filesystem>
#include <exception>

using platform::linux::BlockDeviceFactory;
using platform::linux::LinuxPlatform;

LinuxPlatform::LinuxPlatform(std::string imageName) : imageName(std::move(imageName))
{
    bsp::board_init();
}

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

    auto blockDeviceFactory = std::make_unique<BlockDeviceFactory>(imageName);
    vfs                     = purefs::subsystem::initialize(std::move(blockDeviceFactory));

    if (int err = purefs::subsystem::mount_defaults(); err != 0) {
        throw std::runtime_error("Failed to initiate filesystem: " + std::to_string(err));
    }

    usesFilesystem = true;
}

void LinuxPlatform::deinit()
{
    if (usesFilesystem) {
        if (int err = purefs::subsystem::unmount_all(); err != 0) {
            throw std::runtime_error("Failed to unmount all: " + std::to_string(err));
        }
        usesFilesystem = false;
    }
}
