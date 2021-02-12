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
#include "thread.hpp"
#include "condition_variable.hpp"
#include "mutex.hpp"
#include "Service/Mailbox.hpp"
#include "Service/Service.hpp"
#include "Service/ServiceCreator.hpp"
#include "PowerManager.hpp"
#include "PhoneModes/Subject.hpp"
#include <common_data/RawKey.hpp>
#include "Constants.hpp"
#include "CpuStatistics.hpp"
#include "DeviceManager.hpp"
#include <chrono>
#include <vector>

namespace sys
{
    namespace constants
    {
        using namespace std::chrono_literals;
        inline constexpr std::chrono::milliseconds timerInitInterval{30s};
        inline constexpr std::chrono::milliseconds timerPeriodInterval{100ms};
    } // namespace constants

    class PhoneModeRequest; // Forward declaration

    enum class Code
    {
        CloseSystem,
        Update,
        Reboot,
        None,
    };

    class SystemManagerCmd : public DataMessage
    {
      public:
        SystemManagerCmd(Code type = Code::None) : DataMessage(BusChannel::SystemManagerRequests), type(type)
        {}

        Code type;
    };

    class SystemManager : public Service
    {
      public:
        using InitFunction = std::function<bool()>;

        enum class State
        {
            Running,
            Suspend,
            Shutdown,
            ShutdownReady,
            Reboot,
        } state = State::Running;

        explicit SystemManager(std::vector<std::unique_ptr<BaseServiceCreator>> &&creators);
        ~SystemManager() override;

        void set(enum State state);

        void initialize();

        void StartSystem(InitFunction sysInit, InitFunction appSpaceInit);

        // Invoke system close procedure
        static bool CloseSystem(Service *s);

        static bool Update(Service *s);

        static bool Reboot(Service *s);

        static bool SuspendService(const std::string &name, Service *caller);

        static bool ResumeService(const std::string &name, Service *caller);

        /// Runs a service
        static bool RunService(std::shared_ptr<Service> service, Service *caller, TickType_t timeout = 5000);

        /// Destroy existing service
        /// @note there is no fallback
        static bool DestroyService(const std::string &name, Service *caller, TickType_t timeout = 5000);

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

        /// Sysmgr stores list of all active services but some of them are under control of parent services.
        /// Parent services ought to manage lifetime of child services hence we are sending DestroyRequests only to
        /// parents.
        /// It closes all workers except EventManager -as it needs information from Eventmanager that it's safe to
        /// shutdown
        void CloseSystemHandler();

        void UpdateSystemHandler();

        void RebootHandler();

        /// loop to handle prior to full system close
        /// for now for rt1051 we need to
        /// 1. check if we have power plug in
        /// 2. if power is in, if we didn't get request for poweron ( this triggers reboot)
        /// this is needed as we can't just shutdown phone with power in - it would end in phone unable to boot (as
        /// red key won't work to power it on. For more information follow up with schematics
        bool PreShutdownLoop();

        /// periodic update of cpu statistics
        void CpuStatisticsTimerHandler();

        MessagePointer handlePhoneModeRequest(PhoneModeRequest *request);

        bool cpuStatisticsTimerInit{false};

        std::vector<std::unique_ptr<BaseServiceCreator>> systemServiceCreators;
        std::unique_ptr<sys::Timer> cpuStatisticsTimer;
        std::unique_ptr<phone_modes::Subject> phoneModeSubject;
        InitFunction userInit;
        InitFunction systemInit;

        static std::vector<std::shared_ptr<Service>> servicesList;
        static cpp_freertos::MutexStandard destroyMutex;
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
    case sys::SystemManager::State::ShutdownReady:
        return "ShutdownReady";
    }
    return "";
}

#endif /* SYSTEMMANAGER_SYSTEMMANAGER_HPP_ */
