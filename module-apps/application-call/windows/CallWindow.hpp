// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-call/ApplicationCall.hpp"
#include "AppWindow.hpp"
#include "application-call/widgets/Icons.hpp"

#include <gui/input/Translator.hpp>
#include <Rect.hpp>
#include <Image.hpp>
#include <application-call/data/CallWindowState.hpp>

namespace gui
{
    class CallWindow : public AppWindow
    {
      private:
        gui::KeyInputMappedTranslation translator;
        time_t callDuration = 0;
        bool stop_timer = false;

      protected:
        // used to display both nnumber and name of contact
        gui::Label *numberLabel = nullptr;
        // used to inform user about call state of call and display duration of call
        gui::Label *durationLabel = nullptr;

        gui::SendSmsIcon *sendSmsIcon  = nullptr;
        gui::MicrophoneIcon *microphoneIcon = nullptr;
        gui::SpeakerIcon *speakerIcon       = nullptr;
        gui::Image *imageCircleTop     = nullptr;
        gui::Image *imageCircleBottom  = nullptr;

        call::State state = call::State::IDLE;
        utils::PhoneNumber::View phoneNumber;
        /**
         * Manipulates widgets to handle currently set state of the window.
         */
        void setVisibleState();
        bool handleLeftButton();
        bool handleRightButton();
        void setState(call::State state);

      public:
        CallWindow(app::Application *app, std::string windowName = app::window::name_call);
        virtual ~CallWindow();

        /**
         * Used by application to update window's state
         */
        const call::State &getState();
        void updateDuration(const utils::time::Duration duration);
        void setCallNumber(std::string);

        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool handleSwitchData(SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool handleDigit(const uint32_t digit);
        void connectTimerOnExit();
        void runCallTimer();
        void stopCallTimer();
    };

} /* namespace gui */
