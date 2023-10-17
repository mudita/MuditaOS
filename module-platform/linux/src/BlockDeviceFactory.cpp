// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BlockDeviceFactory.hpp"

#include <platform/linux/DiskImage.hpp>

using platform::linux::BlockDeviceFactory;

BlockDeviceFactory::BlockDeviceFactory(std::string imageName) : imageName(std::move(imageName))
{}

std::unique_ptr<purefs::blkdev::disk> BlockDeviceFactory::makeDefaultBlockDevice()
{
    return std::make_unique<purefs::blkdev::disk_image>(imageName);
}

std::unique_ptr<purefs::blkdev::disk> BlockDeviceFactory::makeDefaultNvmDevice()
{
    return std::make_unique<purefs::blkdev::disk_image>(eeprom_image_name, eeprom_sector_size, eeprom_num_sysparts);
}
