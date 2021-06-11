// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/vfs_subsystem_internal.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/blkdev/disk_image.hpp>

namespace purefs::subsystem::internal
{
    namespace
    {
        constexpr auto disk_image_name            = "PurePhone.img";
        constexpr auto eeprom_image_name          = "eeprom.img";
        static constexpr auto eeprom_sector_size  = 64;
        static constexpr auto eeprom_num_sysparts = 1;
    } // namespace
    auto create_default_block_device() -> std::shared_ptr<blkdev::disk>
    {
        return std::make_shared<blkdev::disk_image>(disk_image_name);
    }
    auto create_default_nvm_device() -> std::shared_ptr<blkdev::disk>
    {
        return std::make_shared<blkdev::disk_image>(eeprom_image_name, eeprom_sector_size, eeprom_num_sysparts);
    }
} // namespace purefs::subsystem::internal
