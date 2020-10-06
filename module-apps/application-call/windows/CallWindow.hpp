#pragma once

#include "application-call/ApplicationCall.hpp"
#include "AppWindow.hpp"
#include "application-call/widgets/Icons.hpp"

#include <gui/input/Translator.hpp>
#include <Rect.hpp>
#include <Image.hpp>
namespace gui
{
    class CallWindow : public AppWindow
    {
      public:
        enum class State
        {
            IDLE,
            INCOMING_CALL,
            OUTGOING_CALL,
            CALL_IN_PROGRESS,
            CALL_ENDED
        };

      private:
        gui::KeyInputMappedTranslation translator;

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

        State state = State::IDLE;
        utils::PhoneNumber::View phoneNumber;
        /**
         * Manipulates widgets to handle currently set state of the window.
         */
        void setVisibleState();
        bool handleLeftButton();
        bool handleRightButton();

      public:
        CallWindow(app::Application *app, std::string windowName = app::window::name_call);
        virtual ~CallWindow();

        /**
         * Used by application to update window's state
         */
        void setState(State state);
        const State &getState();
        void updateDuration(const utils::time::Duration &duration);
        void setCallNumber(std::string);

        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool handleSwitchData(SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool handleDigit(const uint32_t digit);
        void connectTimerOnExit();
    };

} /* namespace gui */
