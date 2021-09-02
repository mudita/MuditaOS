// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-os/CriticalSectionGuard.hpp>

#include <memory>

namespace hal::impl
{
    template <typename TDerived, typename TInterface, typename... Args>
    inline std::shared_ptr<TInterface> factory(Args... args)
    {
        const auto criticalSection = cpp_freertos::CriticalSectionGuard{};
        static std::weak_ptr<TDerived> singleton;
        auto inst = singleton.lock();
        if (not inst) {
            inst      = std::make_shared<TDerived>(args...);
            singleton = inst;
        }
        return inst;
    }
} // namespace hal::impl
