// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "data/CallState.hpp"

#include <Application.hpp>
#include <Service/Message.hpp>
#include <Service/Timer.hpp>
#include <SystemManager/SystemManager.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <time/time_conversion.hpp>

namespace app
{
    inline constexpr auto name_call       = "ApplicationCall";
    inline constexpr auto call_stack_size = 8192U;

    namespace window
    {
        inline constexpr auto name_call              = "CallWindow";
        inline constexpr auto name_enterNumber       = "EnterNumberWindow";
        inline constexpr auto name_emergencyCall     = "EmergencyCallWindow";
        inline constexpr auto name_duplicatedContact = "DuplicatedContactWindow";
        inline constexpr auto name_dialogConfirm     = "DialogConfirm";
    } // namespace window

    inline constexpr auto ringtone_path = "assets/audio/Ringtone-drum2.mp3"; // Should bo moved to database

    class CallWindowInterface
    {
      public:
        virtual ~CallWindowInterface() noexcept = default;

        virtual void setState(app::call::State state) noexcept              = 0;
        [[nodiscard]] virtual auto getState() const noexcept -> call::State = 0;

        virtual void transmitDtmfTone(uint32_t digit) = 0;
        virtual void stopAudio()                      = 0;
        virtual void startRinging()                   = 0;
        virtual void startRouting()                   = 0;
    };

    class EnterNumberWindowInterface
    {
      public:
        virtual ~EnterNumberWindowInterface() noexcept                = default;
        virtual void handleCallEvent(const std::string &number)       = 0;
        virtual void handleAddContactEvent(const std::string &number) = 0;
    };

    class ApplicationCall : public Application, public CallWindowInterface, public EnterNumberWindowInterface
    {
      private:
        void CallAbortHandler();
        void CallActiveHandler();
        void IncomingCallHandler(const CellularCallMessage *const msg);
        void RingingHandler(const CellularCallMessage *const msg);

      protected:
        call::State state = call::State::IDLE;

      public:
        ApplicationCall(std::string name                    = name_call,
                        std::string parent                  = {},
                        StartInBackground startInBackground = {false});
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;

        void handleCallEvent(const std::string &number) override;
        void handleAddContactEvent(const std::string &number) override;

        auto showNotification(std::function<bool()> action) -> bool;

        [[nodiscard]] auto getState() const noexcept -> call::State override
        {
            return state;
        }
        void setState(call::State state) noexcept override
        {
            this->state = state;
        }
        void transmitDtmfTone(uint32_t digit) override;
        void stopAudio() override;
        void startRinging() override;
        void startRouting() override;
    };

    template <> struct ManifestTraits<ApplicationCall>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch, manager::actions::Call, manager::actions::Dial}};
        }
    };
} /* namespace app */
