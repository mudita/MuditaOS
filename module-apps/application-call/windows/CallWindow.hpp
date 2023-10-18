// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationCall.hpp"
#include "StateIcons.hpp"
#include <application-call/presenter/CallPresenter.hpp>

#include <AppWindow.hpp>
#include <gui/input/Translator.hpp>
#include <Image.hpp>
#include <Rect.hpp>
#include <time/time_conversion.hpp>

namespace gui
{
    class CallWindow : public AppWindow, public app::call::CallWindowContract::View
    {
      private:
        gui::KeyInputMappedTranslation translator;
        sys::TimerHandle delayedExitTimer;
        static constexpr inline auto callDelayedStopTime = std::chrono::milliseconds{3000};

        [[nodiscard]] auto getDelayedStopTime() const
        {
            return callDelayedStopTime;
        }
        app::call::CallWindowContract::Presenter &presenter;

      protected:
        // used to display both number and name of contact
        gui::Label *numberLabel = nullptr;
        // used to inform user about call state of call and display duration of call
        gui::Label *durationLabel = nullptr;
        // used to inform user about provided numbers during the call
        gui::Label *enteredDigitsLabel = nullptr;

        gui::HBox *iconsBox                 = nullptr;
        gui::SendSmsIcon *sendSmsIcon       = nullptr;
        gui::MicrophoneIcon *microphoneIcon = nullptr;
        gui::SpeakerIcon *speakerIcon       = nullptr;
        gui::Image *imageCircleTop          = nullptr;
        gui::Image *imageCircleBottom       = nullptr;

        utils::PhoneNumber::View phoneNumber;

      public:
        CallWindow(app::ApplicationCommon *app, app::call::CallWindowContract::Presenter &presenter);
        ~CallWindow() noexcept override;

        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;

        void connectTimerOnExit();
        void refreshWindow() override;

        void updateDuration(const UTF8 &text, bool isVisible = true) override;
        void updateEnteredNumber(const char &newCharacter, bool isVisible = true) override;
        void setEnteredNumberVisible(bool isVisible) override;
        void setNavBarForActiveCall() override;
        void setNavBarForIncomingCall() override;
        void clearNavBar() override;
        void setIncomingCallLayout(bool isValidCallerId) override;
        void setActiveCallLayout() override;
        void setCallEndedLayout(bool delayedClose = true) override;
        void updateNumber(const UTF8 &text) override;
        gui::SpeakerIconState getSpeakerIconState() override;
        void setSpeakerIconState(const gui::SpeakerIconState &icon) override;
    };
} /* namespace gui */
