// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "disk.hpp"

#include <memory>

namespace purefs
{
    class DeviceFactory
    {
      public:
        virtual ~DeviceFactory()                                       = default;
        virtual std::unique_ptr<blkdev::disk> makeDefaultBlockDevice() = 0;
        virtual std::unique_ptr<blkdev::disk> makeDefaultNvmDevice()   = 0;
    };
} // namespace purefs
