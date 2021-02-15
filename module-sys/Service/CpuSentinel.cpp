// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CpuSentinel.hpp"
#include "SystemManager/messages/RequestCpuFrequencyMessage.hpp"
#include "SystemManager/Constants.hpp"
#include <memory>

namespace sys
{

    CpuSentinel::CpuSentinel(std::string name, sys::Service *service, std::function<void(bsp::CpuFrequencyHz)> callback)
        : name(name), owner(service), callback(callback)
    {}

    [[nodiscard]] auto CpuSentinel::GetName() const noexcept -> std::string
    {
        return name;
    }

    void CpuSentinel::HoldMinimumFrequency(bsp::CpuFrequencyHz frequencyToHold)
    {
        auto msg = std::make_shared<sys::RequestCpuFrequencyMessage>(GetName(), frequencyToHold);
        owner->bus.sendUnicast(msg, service::name::system_manager);
    }

    void CpuSentinel::CpuFrequencyHasChanged(bsp::CpuFrequencyHz newFrequency)
    {
        if (callback) {
            callback(newFrequency);
        }
    }

} // namespace sys
