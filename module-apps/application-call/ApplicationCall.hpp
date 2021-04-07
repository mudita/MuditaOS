// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "data/CallState.hpp"

#include <Application.hpp>
#include <Service/Message.hpp>
#include <module-sys/Timers/TimerHandle.hpp>
#include <SystemManager/SystemManager.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <time/time_conversion.hpp>

namespace app
{
    inline constexpr auto name_call       = "ApplicationCall";
    inline constexpr auto call_stack_size = 8192U;

    namespace window
    {
        inline constexpr auto name_call              = gui::name::window::main_window;
        inline constexpr auto name_enterNumber       = "EnterNumberWindow";
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

        virtual void setState(app::call::State state) noexcept              = 0;
        [[nodiscard]] virtual auto getState() const noexcept -> call::State = 0;

        enum class AudioEvent
        {
            Mute,
            Unmute,
            LoudspeakerOn,
            LoudspeakerOff
        };

        virtual void stopAudio()                           = 0;
        virtual void startAudioRinging()                   = 0;
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
        void CallAbortHandler();
        void CallActiveHandler();
        void IncomingCallHandler(const CellularIncominCallMessage *const msg);
        void CallerIdHandler(const CellularCallerIdMessage *const msg);
        void RingingHandler(const CellularRingingMessage *const msg);

        sys::TimerHandle callerIdTimer;

      protected:
        call::State state = call::State::IDLE;

      public:
        ApplicationCall(std::string name                    = name_call,
                        std::string parent                  = {},
                        sys::phone_modes::PhoneMode mode    = sys::phone_modes::PhoneMode::Connected,
                        StartInBackground startInBackground = {false});
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        bool isPopupPermitted(gui::popup::ID popupId) const override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;

        void handleEmergencyCallEvent(const std::string &number) override;
        void handleCallEvent(const std::string &number) override;
        void handleAddContactEvent(const std::string &number) override;

        auto showNotification(std::function<bool()> action, const std::string &icon, const std::string &text) -> bool;

        [[nodiscard]] auto getState() const noexcept -> call::State override
        {
            return state;
        }
        void setState(call::State state) noexcept override
        {
            this->state = state;
        }

        void stopAudio() override;
        void startAudioRinging() override;
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
                     manager::actions::PhoneModeChanged}};
        }
    };
} /* namespace app */
