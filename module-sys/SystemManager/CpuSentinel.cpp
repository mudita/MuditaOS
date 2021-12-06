// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <SystemManager/CpuSentinel.hpp>
#include "system/messages/RequestCpuFrequencyMessage.hpp"
#include "system/Constants.hpp"
#include <memory>

namespace sys
{

    CpuSentinel::CpuSentinel(std::string name,
                             sys::Service *service,
                             std::function<void(bsp::CpuFrequencyMHz)> callback)
        : name(name), owner(service), callback(callback)
    {}

    [[nodiscard]] auto CpuSentinel::GetName() const noexcept -> std::string
    {
        return name;
    }

    void CpuSentinel::HoldMinimumFrequency(bsp::CpuFrequencyMHz frequencyToHold)
    {
        if (currentFrequencyToHold != frequencyToHold) {
            auto msg = std::make_shared<sys::HoldCpuFrequencyMessage>(GetName(), frequencyToHold);
            owner->bus.sendUnicast(std::move(msg), service::name::system_manager);
            currentFrequencyToHold = frequencyToHold;
        }
    }

    void CpuSentinel::ReleaseMinimumFrequency()
    {
        if (currentFrequencyToHold != bsp::CpuFrequencyMHz::Level_0) {
            auto msg = std::make_shared<sys::ReleaseCpuFrequencyMessage>(GetName());
            owner->bus.sendUnicast(std::move(msg), service::name::system_manager);
            currentFrequencyToHold = bsp::CpuFrequencyMHz::Level_0;
        }
    }

    void CpuSentinel::HoldFrequencyPermanently(bsp::CpuFrequencyMHz frequencyToHold)
    {
        permanentFrequencyToHold.isActive        = true;
        permanentFrequencyToHold.frequencyToHold = frequencyToHold;
        auto msg = std::make_shared<sys::HoldCpuFrequencyPermanentlyMessage>(GetName(), frequencyToHold);
        owner->bus.sendUnicast(std::move(msg), service::name::system_manager);
    }

    [[nodiscard]] auto CpuSentinel::GetFrequency() const noexcept -> bsp::CpuFrequencyMHz
    {
        if (permanentFrequencyToHold.isActive) {
            return permanentFrequencyToHold.frequencyToHold;
        }
        else {
            return currentFrequency;
        }
    }

    void CpuSentinel::ReleasePermanentFrequency()
    {
        if (permanentFrequencyToHold.isActive) {
            auto msg = std::make_shared<sys::ReleaseCpuPermanentFrequencyMessage>(GetName());
            owner->bus.sendUnicast(std::move(msg), service::name::system_manager);
            permanentFrequencyToHold.isActive        = false;
            permanentFrequencyToHold.frequencyToHold = bsp::CpuFrequencyMHz::Level_0;
        }
    }

    bool CpuSentinel::isPermanentFrequencyActive()
    {
        return permanentFrequencyToHold.isActive;
    }

    void CpuSentinel::CpuFrequencyHasChanged(bsp::CpuFrequencyMHz newFrequency)
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

    bool CpuSentinel::HoldMinimumFrequencyAndWait(bsp::CpuFrequencyMHz frequencyToHold,
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

    void CpuSentinel::ReadRegistrationData(bsp::CpuFrequencyMHz frequencyHz, bool permanentFrequency)
    {
        currentFrequency                  = frequencyHz;
        permanentFrequencyToHold.isActive = permanentFrequency;

        if (permanentFrequencyToHold.isActive) {
            permanentFrequencyToHold.frequencyToHold = currentFrequency;
        }
    }
} // namespace sys
