// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <BlockDeviceFactory.hpp>
#include <disk_emmc.hpp>

namespace
{
    constexpr auto NO_DEVICE = nullptr;
}
using platform::rt1051::BlockDeviceFactory;

std::unique_ptr<purefs::blkdev::disk> BlockDeviceFactory::makeDefaultBlockDevice()
{
    return std::make_unique<purefs::blkdev::disk_emmc>();
}

std::unique_ptr<purefs::blkdev::disk> BlockDeviceFactory::makeDefaultNvmDevice()
{
    return NO_DEVICE;
}
