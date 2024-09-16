// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <purefs/DeviceFactory.hpp>

#include <memory>

namespace platform::linux::purefs
{
    class DeviceFactory : public purefs::DeviceFactory
    {
      public:
        std::unique_ptr<blkdev::disk> makeDefaultBlockDevice() override final;
        std::unique_ptr<blkdev::disk> makeDefaultNvmDevice() override final;
    };
}; // namespace platform::linux::purefs
