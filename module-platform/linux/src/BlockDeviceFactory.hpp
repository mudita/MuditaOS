// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/blkdev/DeviceFactory.hpp>

#include <memory>

namespace platform::linux
{
    class BlockDeviceFactory : public purefs::DeviceFactory
    {
      public:
        explicit BlockDeviceFactory(std::string imageName);

        std::unique_ptr<purefs::blkdev::disk> makeDefaultBlockDevice() override final;
        std::unique_ptr<purefs::blkdev::disk> makeDefaultNvmDevice() override final;

      private:
        static constexpr auto eeprom_image_name   = "eeprom.img";
        static constexpr auto eeprom_sector_size  = 64;
        static constexpr auto eeprom_num_sysparts = 1;

        std::string imageName;
    };
}; // namespace platform::linux
