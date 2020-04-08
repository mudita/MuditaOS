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
#include "timer.hpp"
#include "condition_variable.hpp"
#include "mutex.hpp"
#include "Service/Mailbox.hpp"
#include "Service/Bus.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "PowerManager.hpp"

namespace sys
{

    enum class SystemManagerMsgType
    {
        CloseSystem,
        Reboot,
    };

    class SystemManagerMsg : public DataMessage
    {
      public:
        SystemManagerMsg(SystemManagerMsgType type) : DataMessage(BusChannels::SystemManagerRequests), type(type)
        {}

        SystemManagerMsgType type;
    };

    class SystemManager : public Service
    {
      public:
        enum class State
        {
            Running,
            Suspend,
            Shutdown,
            Reboot,
        } state = State::Running;
        SystemManager(TickType_t pingInterval);

        ~SystemManager();

        void StartSystem(std::function<int()> init);

        // Invoke system close procedure
        static bool CloseSystem(Service *s);

        static bool SuspendSystem(Service *caller);

        static bool ResumeSystem(Service *caller);

        static bool SuspendService(const std::string &name, Service *caller);

        static bool ResumeService(const std::string &name, Service *caller);

        // Create new service
        static bool CreateService(std::shared_ptr<Service> service, Service *caller, TickType_t timeout = 5000);

        /// Destroy existing service
        /// @note there is no fallback
        static bool DestroyService(const std::string &name, Service *caller, TickType_t timeout = 5000);

        /// Kill service
        /// @note - this is final, it straight takes service, calls it's close callback and it's gone
        /// please mind that services & apps not registered in SystemManager cant be killed - these should ba handled by
        /// parenst (as above in Destroy) ApplicationManager somehow propagates this, but I would call how it's done
        /// `imperfect`
        bool kill(std::shared_ptr<Service> const &toKill);

      private:
        void TickHandler(uint32_t id) override;

        Message_t DataReceivedHandler(DataMessage *msg, ResponseMessage *resp) override;

        ReturnCodes InitHandler() override;

        ReturnCodes DeinitHandler() override
        {
            return ReturnCodes::Success;
        }

        ReturnCodes SwitchPowerModeHandler(const ServicePowerMode mode) override final
        {
            return ReturnCodes::Success;
        }

        void Run() override;

        // Sysmgr stores list of all active services but some of them are under control of parent services.
        // Parent services ought to manage lifetime of child services hence we are sending DestroyRequests only to
        // parents.
        void CloseSystemHandler();

        void RebootHandler();

        /// loop to handle prior to full system close
        /// for now for rt1051 we need to
        /// 1. check if we have power plug in
        /// 2. if power is in, if we didn't get request for poweron ( this triggers reboot)
        /// this is needed as we can't just shutdown phone with power in - it would end in phone unable to boot (as
        /// red key won't work to power it on. For more information follow up with schematics
        bool PreShutdownLoop();

        TickType_t pingInterval;
        uint32_t pingPongTimerID;

        std::function<int()> userInit;

        static std::vector<std::shared_ptr<Service>> servicesList;
        static cpp_freertos::MutexStandard destroyMutex;
        static std::unique_ptr<PowerManager> powerManager;
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
    }
    return "";
}

#endif /* SYSTEMMANAGER_SYSTEMMANAGER_HPP_ */
