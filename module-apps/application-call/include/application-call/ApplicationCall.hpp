// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-call/model/CallModel.hpp>
#include <application-call/presenter/CallPresenter.hpp>
#include <application-call/windows/WindowNames.hpp>
#include <Timers/TimerHandle.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-evtmgr/ServiceEventManagerName.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <Service/Message.hpp>
#include <service-audio/AudioMessage.hpp>
#include <SystemManager/SystemManagerCommon.hpp>
#include <AppWindowConstants.hpp>
#include <products/PurePhone/apps/include/Application.hpp>

namespace app
{
    inline constexpr auto name_call = "ApplicationCall";

    class EnterNumberWindowInterface
    {
      public:
        virtual ~EnterNumberWindowInterface() noexcept = default;
        enum class ExternalRequest
        {
            True,
            False
        };
        virtual void handleCallEvent(const std::string &number,
                                     ExternalRequest isExternalRequest = ExternalRequest::False) = 0;
        virtual void handleEmergencyCallEvent(const std::string &number)                         = 0;
        virtual void handleAddContactEvent(const std::string &number)                            = 0;
    };

    class ApplicationCall : public Application, public EnterNumberWindowInterface
    {
      public:
        explicit ApplicationCall(std::string name                    = name_call,
                                 std::string parent                  = {},
                                 StatusIndicators statusIndicators   = StatusIndicators{},
                                 StartInBackground startInBackground = {false});
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::MessagePointer handleAppClose(sys::Message *msgl) override;

        sys::ReturnCodes SwitchPowerModeHandler([[maybe_unused]] const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;

        void handleEmergencyCallEvent(const std::string &number) override;
        void handleCallEvent(const std::string &number, ExternalRequest isExternalRequest) override;
        void handleAddContactEvent(const std::string &number) override;

        sys::MessagePointer handleRoutingNotification(AudioRoutingNotification *message);

        auto showNotification(std::function<bool()> action, const std::string &icon, const std::string &text) -> bool;
        enum class NotificationType
        {
            Info,
            Failure
        };
        auto showNotificationAndRestartCallFlow(NotificationType type, const std::string &text) -> bool;

      private:
        std::shared_ptr<call::AbstractCallModel> callModel;
        std::unique_ptr<call::CallWindowContract::Presenter> callPresenter;

      protected:
        ExternalRequest externalRequest = ExternalRequest::False;

        bool conditionalReturnToPreviousView();
    };

    template <>
    struct ManifestTraits<ApplicationCall>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch,
                     manager::actions::Call,
                     manager::actions::Dial,
                     manager::actions::EmergencyDial,
                     manager::actions::NotAnEmergencyNotification,
                     manager::actions::NoSimNotification,
                     manager::actions::NoNetworkConnectionNotification,
                     manager::actions::CallRequestGeneralErrorNotification,
                     manager::actions::CallRejectedByOfflineNotification,
                     manager::actions::PhoneModeChanged,
                     manager::actions::ActivateCall,
                     manager::actions::HandleOutgoingCall,
                     manager::actions::HandleIncomingCall,
                     manager::actions::HandleCallerId,
                     manager::actions::HandleCallerId,
                     manager::actions::BluetoothModeChanged,
                     manager::actions::AlarmClockStatusChanged},
                    locks::AutoLockPolicy::PreventPermanently};
        }
    };
} /* namespace app */
