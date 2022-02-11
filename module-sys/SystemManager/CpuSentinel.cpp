// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <SystemManager/CpuSentinel.hpp>
#include "system/messages/RequestCpuFrequencyMessage.hpp"
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
            auto msg = std::make_shared<sys::HoldCpuFrequencyMessage>(GetName(), frequencyToHold);
            owner->bus.sendUnicast(std::move(msg), service::name::system_manager);
            currentFrequencyToHold = frequencyToHold;
            currentReason          = std::string("up: ") + owner->getCurrentProcessing() + std::string(" req: ") +
                            std::to_string(int(frequencyToHold));
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
        if (taskWaitingForFrequency != nullptr && newFrequency >= currentFrequencyToHold) {
            xTaskNotifyGive(taskWaitingForFrequency);
            taskWaitingForFrequency = nullptr;
        }
    }

    bool CpuSentinel::HoldMinimumFrequencyAndWait(bsp::CpuFrequencyMHz frequencyToHold,
                                                  TaskHandle_t taskToNotify,
                                                  uint32_t timeout)
    {
        currentReason = std::string("h+w: ") + owner->getCurrentProcessing();
        HoldMinimumFrequency(frequencyToHold);

        if (currentFrequencyToHold < frequencyToHold) {
            taskWaitingForFrequency = taskToNotify;
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
