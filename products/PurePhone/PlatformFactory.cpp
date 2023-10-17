// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PlatformFactory.hpp"

#if defined(PLATFORM_linux)
#include <platform/linux/LinuxPlatform.hpp>
#endif

#if defined(PLATFORM_rt1051)
#include <platform/rt1051/RT1051Platform.hpp>
#endif

#include <memory>
#include <utility>

using purephone::PlatformFactory;

auto PlatformFactory::makePlatform() const -> std::unique_ptr<platform::Platform>
{
#if defined(PLATFORM_linux)
    return std::make_unique<platform::linux::LinuxPlatform>(imageName);
#elif defined(PLATFORM_rt1051)
    return std::make_unique<platform::rt1051::RT1051Platform>();
#else
#error "Target platform not defined."
#endif
    return nullptr;
}
