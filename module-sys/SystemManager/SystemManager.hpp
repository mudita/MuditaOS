// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * Service.hpp
 *
 *  Created on: Mar 7, 2019
 *      Author: mati
 */

#ifndef SYSTEMMANAGER_SYSTEMMANAGER_HPP_
#define SYSTEMMANAGER_SYSTEMMANAGER_HPP_

#include <functional>
#include <stdexcept>
#include "thread.hpp"
#include "condition_variable.hpp"
#include "mutex.hpp"
#include "Service/Mailbox.hpp"
#include "Service/Service.hpp"
#include "Service/ServiceCreator.hpp"
#include "Timers/TimerHandle.hpp"
#include "PowerManager.hpp"
#include "PhoneModes/Subject.hpp"
#include <common_data/RawKey.hpp>
#include "Constants.hpp"
#include "CpuStatistics.hpp"
#include "DeviceManager.hpp"
#include <chrono>
#include <vector>
#include <apps-common/Application.hpp>

namespace sys
{
    namespace constants
    {
        using namespace std::chrono_literals;
        inline constexpr std::chrono::milliseconds timerInitInterval{30s};
        inline constexpr std::chrono::milliseconds timerPeriodInterval{100ms};
        inline constexpr std::chrono::milliseconds powerManagerLogsTimerInterval{1h};
        inline constexpr auto restoreTimeout{5000};
    } // namespace constants

    class PhoneModeRequest;         // Forward declaration
    class TetheringStateRequest;    // Forward declaration
    class TetheringEnabledResponse; // Forward declaration

    enum class Code
    {
        CloseSystem,
        Update,
        Restore,
        Reboot,
        RebootToUpdate,
        None,
    };

    class SystemInitialisationError : public std::runtime_error
    {
      public:
        using std::runtime_error::runtime_error;
    };

    class SystemManagerCmd : public DataMessage
    {
      public:
        explicit SystemManagerCmd(Code type                 = Code::None,
                                  CloseReason closeReason   = CloseReason::RegularPowerDown,
                                  UpdateReason updateReason = UpdateReason::Update)
            : DataMessage(BusChannel::SystemManagerRequests), type(type), closeReason(closeReason),
              updateReason(updateReason)
        {}

        Code type;
        CloseReason closeReason;
        UpdateReason updateReason;
    };

    class SystemManager : public Service
    {
      private:
        UpdateReason updateReason{UpdateReason::Update};

      public:
        using InitFunction = std::function<bool()>;

        enum class State
        {
            Running,
            Suspend,
            Shutdown,
            ShutdownReady,
            Reboot,
            RebootToUpdate
        } state = State::Running;

        explicit SystemManager(std::vector<std::unique_ptr<BaseServiceCreator>> &&creators);
        ~SystemManager() override;

        void set(enum State state);

        void initialize();

        void StartSystem(InitFunction sysInit, InitFunction appSpaceInit);

        // Invoke system close procedure
        static bool CloseSystem(Service *s);

        static bool Restore(Service *s);

        static bool Reboot(Service *s);

        static bool RebootToUpdate(Service *s, UpdateReason updateReason);

        static bool SuspendService(const std::string &name, Service *caller);

        static bool ResumeService(const std::string &name, Service *caller);

        /// Runs a service
        static bool RunSystemService(std::shared_ptr<Service> service, Service *caller, TickType_t timeout = 5000);
        /// Runs an application
        static bool RunApplication(std::shared_ptr<app::Application> app, Service *caller, TickType_t timeout = 5000);

        /// Destroy existing service
        /// @note there is no fallback
        static bool DestroySystemService(const std::string &name, Service *caller);
        /// Destroy existing application
        static bool DestroyApplication(const std::string &name, Service *caller);

        /// Translates a slider state into a phone mode.
        /// \param key  Slider button state
        /// \return Phone mode.
        static phone_modes::PhoneMode translateSliderState(const RawKey &key);

        /// Kill service
        /// @note - this is final, it straight takes service, calls it's close callback and it's gone
        /// please mind that services & apps not registered in SystemManager cant be killed - these should be handled by
        /// parents (as above in Destroy) ApplicationManager somehow propagates this, but I would call how it's done
        /// `imperfect`
        ///
        /// @note there is no timeout on deinit handler, might be worth to consider blocking `message` DeinitHandler
        /// instead
        void kill(std::shared_ptr<Service> const &toKill);

      private:
        MessagePointer DataReceivedHandler(DataMessage *msg, ResponseMessage *resp) override;

        ReturnCodes InitHandler() override;

        ReturnCodes DeinitHandler() override
        {
            return ReturnCodes::Success;
        }

        ReturnCodes SwitchPowerModeHandler(const ServicePowerMode mode) final
        {
            return ReturnCodes::Success;
        }

        void Run() override;

        void StartSystemServices();

        static bool RunService(std::shared_ptr<Service> service, Service *caller, TickType_t timeout = 5000);
        static bool RequestServiceClose(const std::string &name, Service *caller, TickType_t timeout = 5000);

        template <typename T> void DestroyServices(const T &whitelist);
        /// Sysmgr stores list of all active services but some of them are under control of parent services.
        /// Parent services ought to manage lifetime of child services hence we are sending DestroyRequests only to
        /// parents.
        /// It closes all workers except EventManager -as it needs information from Eventmanager that it's safe to
        /// shutdown
        void CloseSystemHandler(CloseReason closeReason = CloseReason::RegularPowerDown);

        void CloseServices();

        void preCloseRoutine(CloseReason closeReason);

        void postStartRoutine();

        void readyToCloseHandler(Message *msg);

        void UpdateSystemHandler();

        void RestoreSystemHandler();

        void RebootHandler(State state, std::optional<UpdateReason> updateReason = std::nullopt);

        /// loop to handle prior to full system close
        /// for now for rt1051 we need to
        /// 1. check if we have power plug in
        /// 2. if power is in, if we didn't get request for poweron ( this triggers reboot)
        /// this is needed as we can't just shutdown phone with power in - it would end in phone unable to boot (as
        /// red key won't work to power it on. For more information follow up with schematics
        bool PreShutdownLoop();

        /// periodic update of cpu statistics
        void CpuStatisticsTimerHandler();

        /// used for power management control for the filesystem
        void UpdateResourcesAfterCpuFrequencyChange(bsp::CpuFrequencyHz newFrequency);

        MessagePointer handlePhoneModeRequest(PhoneModeRequest *request);
        MessagePointer handleTetheringStateRequest(TetheringStateRequest *request);
        MessagePointer enableTethering(TetheringEnabledResponse *response);

        void batteryCriticalLevelAction(bool charging);
        void batteryNormalLevelAction();
        void batteryShutdownLevelAction();

        bool cpuStatisticsTimerInit{false};

        std::vector<std::unique_ptr<BaseServiceCreator>> systemServiceCreators;
        sys::TimerHandle cpuStatisticsTimer;
        sys::TimerHandle servicesPreShutdownRoutineTimeout;
        sys::TimerHandle lowBatteryShutdownDelay;
        sys::TimerHandle powerManagerEfficiencyTimer;
        std::unique_ptr<phone_modes::Subject> phoneModeSubject;
        InitFunction userInit;
        InitFunction systemInit;
        std::vector<std::string> readyForCloseRegister;

        std::shared_ptr<sys::CpuSentinel> cpuSentinel;

        static std::vector<std::shared_ptr<Service>> servicesList;
        static std::vector<std::shared_ptr<app::Application>> applicationsList;
        static cpp_freertos::MutexStandard serviceDestroyMutex;
        static cpp_freertos::MutexStandard appDestroyMutex;
        static std::unique_ptr<PowerManager> powerManager;
        static std::unique_ptr<CpuStatistics> cpuStatistics;
        static std::unique_ptr<DeviceManager> deviceManager;
    };
} // namespace sys

inline const char *c_str(sys::SystemManager::State state)
{
    switch (state) {
    case sys::SystemManager::State::Running:
        return "Running";
    case sys::SystemManager::State::Suspend:
        return "Suspend";
    case sys::SystemManager::State::Shutdown:
        return "Shutdown";
    case sys::SystemManager::State::Reboot:
        return "Reboot";
    case sys::SystemManager::State::RebootToUpdate:
        return "RebootToUpdate";
    case sys::SystemManager::State::ShutdownReady:
        return "ShutdownReady";
    }
    return "";
}

#endif /* SYSTEMMANAGER_SYSTEMMANAGER_HPP_ */
