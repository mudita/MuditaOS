#pragma once

#include "application-call/ApplicationCall.hpp"
#include "AppWindow.hpp"
#include "Rect.hpp"
#include "Image.hpp"
#include "application-call/widgets/Icons.hpp"

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

      protected:
        enum class FocusRects
        {
            Speaker,
            Micrphone,
        };

        enum class AudioState
        {
            ON,
            OFF
        };
        //	gui::Label* titleLabel = nullptr;
        // used to display both nnumber and name of contact
        gui::Label *numberLabel = nullptr;
        // used to inform user about call state of call and display duration of call
        gui::Label *durationLabel = nullptr;

        gui::Image *imageSpeaker[2]    = {nullptr, nullptr};
        gui::Image *imageMicrophone[2] = {nullptr, nullptr};
        gui::Rect *rects[2]            = {nullptr};
        gui::SendSmsIcon *sendSmsIcon  = nullptr;
        gui::Image *imageCircleTop     = nullptr;
        gui::Image *imageCircleBottom  = nullptr;

        AudioState microphoneState = AudioState::ON;
        AudioState speakerState    = AudioState::OFF;

        State state = State::IDLE;
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
        void updateDuration(time_t duration);
        void setCallNumber(std::string);

        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool handleSwitchData(SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
