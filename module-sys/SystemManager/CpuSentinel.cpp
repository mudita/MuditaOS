// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <SystemManager/CpuSentinel.hpp>
#include "system/messages/RequestCpuFrequencyMessage.hpp"
#include "system/messages/HoldCpuFrequency.hpp"
#include "system/Constants.hpp"
#include <Timers/TimerFactory.hpp>
#include <memory>

namespace sys
{
    namespace
    {
        constexpr std::chrono::seconds defaultHoldFrequencyTime{1};
    } // namespace

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
            auto msg =
                std::make_shared<sys::HoldCpuFrequencyMessage>(GetName(), frequencyToHold, xTaskGetCurrentTaskHandle());
            owner->bus.sendUnicast(std::move(msg), service::name::system_manager);
            currentFrequencyToHold = frequencyToHold;
            currentReason          = std::string("up: ") + owner->getCurrentProcessing() + std::string(" req: ") +
                            std::to_string(int(frequencyToHold));
            ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));
        }
    }

    void CpuSentinel::ReleaseMinimumFrequency()
    {
        if (currentFrequencyToHold != bsp::CpuFrequencyMHz::Level_0) {
            auto msg = std::make_shared<sys::ReleaseCpuFrequencyMessage>(GetName());
            owner->bus.sendUnicast(std::move(msg), service::name::system_manager);
            currentFrequencyToHold = bsp::CpuFrequencyMHz::Level_0;
            currentReason          = std::string("down: ") + owner->getCurrentProcessing();
        }
    }

    [[nodiscard]] auto CpuSentinel::GetFrequency() const noexcept -> bsp::CpuFrequencyMHz
    {
            return currentFrequency;
    }

    void CpuSentinel::CpuFrequencyHasChanged(bsp::CpuFrequencyMHz newFrequency)
    {
        currentFrequency = newFrequency;
        if (callback) {
            callback(newFrequency);
        }
    }

    void CpuSentinel::ReadRegistrationData(bsp::CpuFrequencyMHz frequencyHz)
    {
        currentFrequency = frequencyHz;
    }

    TimedCpuSentinel::TimedCpuSentinel(std::string name, sys::Service *service)
        : CpuSentinel(name, service), timerHandle{sys::TimerFactory::createSingleShotTimer(
                                          owner, "holdFrequencyTimer", defaultHoldFrequencyTime, [this](sys::Timer &) {
                                              ReleaseMinimumFrequency();
                                          })}
    {}

    TimedCpuSentinel::~TimedCpuSentinel()
    {
        if (timerHandle.isActive()) {
            timerHandle.stop();
        }
    }

    void TimedCpuSentinel::HoldMinimumFrequencyForTime(bsp::CpuFrequencyMHz frequencyToHold,
                                                       std::chrono::milliseconds timeout)
    {
        if (currentFrequencyToHold != frequencyToHold) {
            HoldMinimumFrequency(frequencyToHold);
            timerHandle.restart(timeout);
        }
    }

    TaskHandle_t CpuSentinel::getTask()
    {
        return owner->GetHandle();
    }

    std::string CpuSentinel::getReason()
    {
        return currentReason;
    }
} // namespace sys
