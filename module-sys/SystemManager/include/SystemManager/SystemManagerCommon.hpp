// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

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
#include <hal/key_input/RawKey.hpp>
#include <system/Constants.hpp>
#include "CpuStatistics.hpp"
#include "TaskStatistics.hpp"
#include "DeviceManager.hpp"
#include <chrono>
#include <vector>

namespace app
{
    class ApplicationCommon;
}

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

    enum class Code
    {
        CloseSystem,
        Restore,
        Reboot,
        RebootToRecovery,
        FactoryReset,
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
        explicit SystemManagerCmd(Code type                     = Code::None,
                                  CloseReason closeReason       = CloseReason::RegularPowerDown,
                                  RecoveryReason recoveryReason = RecoveryReason::Update)
            : DataMessage(BusChannel::SystemManagerRequests), type(type), closeReason(closeReason),
              recoveryReason(recoveryReason)
        {}

        Code type;
        CloseReason closeReason;
        RecoveryReason recoveryReason;
    };

    class SystemManagerCommon : public Service
    {
      public:
        using InitFunction   = std::function<bool()>;
        using DeinitFunction = std::function<bool()>;
        enum class State
        {
            Running,
            Suspend,
            Shutdown,
            ShutdownReady,
            Reboot,
            RebootToRecovery
        } state = State::Running;

        explicit SystemManagerCommon(std::vector<std::unique_ptr<BaseServiceCreator>> &&creators);
        ~SystemManagerCommon() override;

        void set(enum State state);

        void initialize();

        virtual void StartSystem(InitFunction sysInit, InitFunction appSpaceInit, DeinitFunction sysDeinit);

        static bool Restore(Service *s);

        static bool FactoryReset(Service *s);

        static bool Reboot(Service *s);

        static bool RebootToRecovery(Service *s, RecoveryReason recoveryReason);

        static bool SuspendService(const std::string &name, Service *caller);

        static bool ResumeService(const std::string &name, Service *caller);

        /// Runs a service
        static bool RunSystemService(std::shared_ptr<Service> service, Service *caller, TickType_t timeout = 5000);
        /// Runs an application
        static bool RunApplication(std::shared_ptr<app::ApplicationCommon> app,
                                   Service *caller,
                                   TickType_t timeout = 5000);

        /// Destroy existing service
        /// @note there is no fallback
        static bool DestroySystemService(const std::string &name, Service *caller);
        /// Destroy existing application
        static bool DestroyApplication(const std::string &name, Service *caller);

        static std::string ServiceProcessor(const uint32_t &t);

        /// Kill service
        /// @note - this is final, it straight takes service, calls it's close callback and it's gone
        /// please mind that services & apps not registered in SystemManager cant be killed - these should be handled by
        /// parents (as above in Destroy) ApplicationManager somehow propagates this, but I would call how it's done
        /// `imperfect`
        ///
        /// @note there is no timeout on deinit handler, might be worth to consider blocking `message` DeinitHandler
        /// instead
        void kill(std::shared_ptr<Service> const &toKill);

      protected:
        ReturnCodes InitHandler() override;
        virtual void batteryNormalLevelAction();
        virtual void batteryCriticalLevelAction(bool charging);
        virtual void batteryShutdownLevelAction();
        virtual void handleShutdown() = 0;

      private:
        MessagePointer DataReceivedHandler(DataMessage *msg, ResponseMessage *resp) override;

        ReturnCodes DeinitHandler() override
        {
            return ReturnCodes::Success;
        }

        ReturnCodes SwitchPowerModeHandler(const ServicePowerMode mode) final
        {
            return ReturnCodes::Success;
        }

        void Run() override;

        void LogPowerOffReason();

        void PowerOff();

        void StartSystemServices();

        static bool RunService(std::shared_ptr<Service> service, Service *caller, TickType_t timeout = 5000);
        static bool RequestServiceClose(const std::string &name, Service *caller, TickType_t timeout = 5000);

        template <typename T>
        void DestroyServices(const T &whitelist);
        /// Sysmgr stores list of all active services but some of them are under control of parent services.
        /// Parent services ought to manage lifetime of child services hence we are sending DestroyRequests only to
        /// parents.
        /// It closes all workers except EventManager -as it needs information from Eventmanager that it's safe to
        /// shutdown
        void CloseSystemHandler(CloseReason closeReason = CloseReason::RegularPowerDown);

        void CloseServices();

        void InitiateSystemCloseSequence(CloseReason closeReason);
        void postStartRoutine();

        void ServiceReadyToCloseHandler(Message *msg);

        void RestoreSystemHandler();

        void RebootHandler();

        void RebootToRecoveryHandler(CloseReason closeReason, RecoveryReason recoveryReason);

        void FreqUpdateTick();

        /// used for power management control for the filesystem
        void UpdateResourcesAfterCpuFrequencyChange(bsp::CpuFrequencyMHz newFrequency);

        bool cpuStatisticsTimerInit{false};
        bool serviceListReversed{false};

        CloseReason closeReason{CloseReason::RegularPowerDown};
        RecoveryReason recoveryReason{RecoveryReason::Update};
        std::vector<std::unique_ptr<BaseServiceCreator>> systemServiceCreators;
        sys::TimerHandle freqTimer;
        sys::TimerHandle serviceCloseTimer;
        sys::TimerHandle lowBatteryShutdownDelay;
        sys::TimerHandle powerManagerStatisticsTimer;
        InitFunction userInit;
        InitFunction systemInit;
        DeinitFunction systemDeinit;
        std::vector<std::string> servicesToClose;

        std::shared_ptr<sys::CpuSentinel> cpuSentinel;

        static std::vector<std::shared_ptr<Service>> servicesList;
        static std::vector<std::shared_ptr<app::ApplicationCommon>> applicationsList;
        static cpp_freertos::MutexStandard serviceDestroyMutex;
        static cpp_freertos::MutexStandard appDestroyMutex;
        std::unique_ptr<CpuStatistics> cpuStatistics;
        std::unique_ptr<TaskStatistics> taskStatistics;
        std::unique_ptr<PowerManager> powerManager;
        std::unique_ptr<DeviceManager> deviceManager;
    };
} // namespace sys

inline const char *c_str(sys::SystemManagerCommon::State state)
{
    switch (state) {
    case sys::SystemManagerCommon::State::Running:
        return "Running";
    case sys::SystemManagerCommon::State::Suspend:
        return "Suspend";
    case sys::SystemManagerCommon::State::Shutdown:
        return "Shutdown";
    case sys::SystemManagerCommon::State::Reboot:
        return "Reboot";
    case sys::SystemManagerCommon::State::RebootToRecovery:
        return "RebootToRecovery";
    case sys::SystemManagerCommon::State::ShutdownReady:
        return "ShutdownReady";
    }
    return "";
}
