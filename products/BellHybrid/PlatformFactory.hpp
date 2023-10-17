// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Platform.hpp>

#include <memory>

namespace bellhybrid
{
    class PlatformFactory
    {
      public:
        constexpr static auto imageName = "BellHybrid.img";

        auto makePlatform() const -> std::unique_ptr<platform::Platform>;
    };
}; // namespace bellhybrid
