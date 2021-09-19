// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <platform/rt1051/RT1051Platform.hpp>

#include "BlockDeviceFactory.hpp"

#include <bsp/bsp.hpp>
#include <purefs/vfs_subsystem.h>

#include <exception>

using platform::rt1051::BlockDeviceFactory;
using platform::rt1051::RT1051Platform;

RT1051Platform::RT1051Platform()
{
    bsp::BoardInit();
}

RT1051Platform::~RT1051Platform()
{
    if (usesFilesystem) {
        purefs::subsystem::unmount_all();
    }
}

void RT1051Platform::init()
{
    initFilesystem();
}

void RT1051Platform::initFilesystem()
{
    if (usesFilesystem) {
        throw std::runtime_error("Filesystem already initialized");
    }

    auto blockDeviceFactory = std::make_unique<BlockDeviceFactory>();
    vfs                     = purefs::subsystem::initialize(std::move(blockDeviceFactory));

    if (int err = purefs::subsystem::mount_defaults(); err != 0) {
        throw std::runtime_error("Failed to initiate filesystem: " + std::to_string(err));
    }

    usesFilesystem = true;
}
