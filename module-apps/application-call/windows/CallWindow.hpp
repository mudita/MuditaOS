// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationCall.hpp"
#include "CallState.hpp"
#include "StateIcons.hpp"

#include <AppWindow.hpp>
#include <gui/input/Translator.hpp>
#include <Image.hpp>
#include <Rect.hpp>
#include <time/time_conversion.hpp>

namespace gui
{
    class CallWindow : public AppWindow
    {
      private:
        enum class CallEndType
        {
            None,
            Ended,
            Rejected,
        } callEndType = CallEndType::None;

        gui::KeyInputMappedTranslation translator;
        sys::TimerHandle callTimer;
        sys::TimerHandle delayedExitTimer;
        std::chrono::seconds callDuration                = std::chrono::seconds().zero();
        static constexpr inline auto callDelayedStopTime = std::chrono::milliseconds{3000};

        [[nodiscard]] auto getDelayedStopTime() const
        {
            return callDelayedStopTime;
        }
        void setCallEndMessage();

      protected:
        app::CallWindowInterface *interface = nullptr;
        // used to display both number and name of contact
        gui::Label *numberLabel = nullptr;
        // used to inform user about call state of call and display duration of call
        gui::Label *durationLabel = nullptr;

        gui::HBox *iconsBox                 = nullptr;
        gui::SendSmsIcon *sendSmsIcon       = nullptr;
        gui::MicrophoneIcon *microphoneIcon = nullptr;
        gui::SpeakerIcon *speakerIcon       = nullptr;
        gui::Image *imageCircleTop          = nullptr;
        gui::Image *imageCircleBottom       = nullptr;

        utils::PhoneNumber::View phoneNumber;

        bool handleLeftButton();
        bool handleRightButton();
        bool handleHeadsetOkButton();
        void setState(app::call::State state);
        [[nodiscard]] auto getState() const noexcept -> app::call::State;

      public:
        CallWindow(app::ApplicationCommon *app,
                   app::CallWindowInterface *interface,
                   std::string windowName = app::window::name_call);

        void updateDuration(const utils::time::Duration duration);

        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;

        bool handleDigit(const uint32_t digit);
        void connectTimerOnExit();
        void runCallTimer();
        void stopCallTimer();
    };

} /* namespace gui */
