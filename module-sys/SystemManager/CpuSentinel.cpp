// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <SystemManager/CpuSentinel.hpp>
#include "system/messages/RequestCpuFrequencyMessage.hpp"
#include "system/Constants.hpp"
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
        if (currentFrequencyToHold != frequencyToHold) {
            auto msg = std::make_shared<sys::HoldCpuFrequencyMessage>(GetName(), frequencyToHold);
            owner->bus.sendUnicast(std::move(msg), service::name::system_manager);
            currentFrequencyToHold = frequencyToHold;
        }
    }

    void CpuSentinel::ReleaseMinimumFrequency()
    {
        if (currentFrequencyToHold != bsp::CpuFrequencyHz::Level_0) {
            auto msg = std::make_shared<sys::ReleaseCpuFrequencyMessage>(GetName());
            owner->bus.sendUnicast(std::move(msg), service::name::system_manager);
            currentFrequencyToHold = bsp::CpuFrequencyHz::Level_0;
        }
    }

    void CpuSentinel::CpuFrequencyHasChanged(bsp::CpuFrequencyHz newFrequency)
    {
        currentFrequency = newFrequency;
        if (callback) {
            callback(newFrequency);
        }
        if (taskHandle != nullptr && newFrequency >= currentFrequencyToHold) {
            xTaskNotifyGive(taskHandle);
            taskHandle = nullptr;
        }
    }

    bool CpuSentinel::HoldMinimumFrequencyAndWait(bsp::CpuFrequencyHz frequencyToHold,
                                                  TaskHandle_t taskToNotify,
                                                  uint32_t timeout)
    {
        HoldMinimumFrequency(frequencyToHold);

        if (currentFrequencyToHold < frequencyToHold) {
            taskHandle = taskToNotify;
            return ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(timeout)) == 0;
        }

        return true;
    }
} // namespace sys
