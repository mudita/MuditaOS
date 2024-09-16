// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Platform.hpp>

#include <memory>

namespace purephone
{
    class PlatformFactory
    {
      public:
        constexpr static auto imageName = "PurePhone.img";

        auto makePlatform() const -> std::unique_ptr<platform::Platform>;
    };
}; // namespace purephone
