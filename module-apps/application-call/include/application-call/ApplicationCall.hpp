// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include "CallState.hpp"

#include <Timers/TimerHandle.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <Service/Message.hpp>
#include <SystemManager/SystemManagerCommon.hpp>
#include <AppWindowConstants.hpp>

namespace app
{
    inline constexpr auto name_call       = "ApplicationCall";
    inline constexpr auto call_stack_size = 8192U;

    namespace window
    {
        inline constexpr auto name_enterNumber       = gui::name::window::main_window;
        inline constexpr auto name_call              = "CallWindow";
        inline constexpr auto name_emergencyCall     = "EmergencyCallWindow";
        inline constexpr auto name_duplicatedContact = "DuplicatedContactWindow";
        inline constexpr auto name_dialogConfirm     = "DialogConfirm";
        inline constexpr auto name_number            = "NumberWindow";
    } // namespace window
    class CallWindowInterface
    {
      public:
        using NumberChangeCallback              = std::function<void(utils::PhoneNumber::View)>;
        virtual ~CallWindowInterface() noexcept = default;

        virtual void setCallState(app::call::State state) noexcept              = 0;
        [[nodiscard]] virtual auto getCallState() const noexcept -> call::State = 0;

        enum class AudioEvent
        {
            Mute,
            Unmute,
            LoudspeakerOn,
            LoudspeakerOff
        };

        virtual void stopAudio()                           = 0;
        virtual void startAudioRouting()                   = 0;
        virtual void sendAudioEvent(AudioEvent audioEvent) = 0;

        virtual void transmitDtmfTone(uint32_t digit) = 0;
        virtual void hangupCall()                     = 0;
        virtual void answerIncomingCall()             = 0;
    };

    class EnterNumberWindowInterface
    {
      public:
        virtual ~EnterNumberWindowInterface() noexcept                   = default;
        virtual void handleCallEvent(const std::string &number)          = 0;
        virtual void handleEmergencyCallEvent(const std::string &number) = 0;
        virtual void handleAddContactEvent(const std::string &number)    = 0;
    };

    class ApplicationCall : public Application, public CallWindowInterface, public EnterNumberWindowInterface
    {
      private:
        sys::TimerHandle callerIdTimer;

      protected:
        call::State callState = call::State::IDLE;

      public:
        explicit ApplicationCall(std::string name                    = name_call,
                                 std::string parent                  = {},
                                 StatusIndicators statusIndicators   = StatusIndicators{},
                                 StartInBackground startInBackground = {false});
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;

        void handleIncomingCall();
        void handleCallerId(const app::manager::actions::CallParams *params);
        void handleEmergencyCallEvent(const std::string &number) override;
        void handleCallEvent(const std::string &number) override;
        void handleAddContactEvent(const std::string &number) override;

        auto showNotification(std::function<bool()> action, const std::string &icon, const std::string &text) -> bool;

        [[nodiscard]] auto getCallState() const noexcept -> call::State override
        {
            return callState;
        }
        void setCallState(call::State state) noexcept override
        {
            if (state == call::State::CALL_IN_PROGRESS) {
                bus.sendUnicast(
                    std::make_shared<sevm::KeypadBacklightMessage>(bsp::keypad_backlight::Action::turnOnCallMode),
                    service::name::evt_manager);
            }
            else {
                bus.sendUnicast(
                    std::make_shared<sevm::KeypadBacklightMessage>(bsp::keypad_backlight::Action::turnOffCallMode),
                    service::name::evt_manager);
            }
            this->callState = state;
        }

        void stopAudio() override;
        void startAudioRouting() override;
        void sendAudioEvent(AudioEvent audioEvent) override;

        void transmitDtmfTone(uint32_t digit) override;
        void hangupCall() override;
        void answerIncomingCall() override;
    };

    template <> struct ManifestTraits<ApplicationCall>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch,
                     manager::actions::Call,
                     manager::actions::Dial,
                     manager::actions::EmergencyDial,
                     manager::actions::NotAnEmergencyNotification,
                     manager::actions::NoSimNotification,
                     manager::actions::CallRejectedByOfflineNotification,
                     manager::actions::PhoneModeChanged,
                     manager::actions::ActivateCall,
                     {manager::actions::AbortCall, manager::actions::ActionFlag::AcceptWhenInBackground},
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
