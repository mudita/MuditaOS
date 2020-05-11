#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "ListView.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include <Text.hpp>
#include <gui/widgets/BoxLayout.hpp>

namespace gui
{
    class ThreadViewWindow : public AppWindow
    {
      private:
        gui::VBox *body         = nullptr;
        uint16_t elements_width = this->getWidth() - style::window::default_left_margin * 2;
        void cleanView();
        enum class Action
        {
            Start,   /// first load of sms
            Next,    /// load previous sms
            Previous /// load next sms
        };
        /// return if request was handled
        bool showMessages(Action what);
        void addSMS(Action what);
        bool smsBuild(const SMSRecord &smsRecord);
        Label *timeLabelBuild(time_t timestamp) const;
        HBox *smsSpanBuild(Text *smsBubble, const SMSRecord &el) const;
        const ssize_t maxsmsinwindow = 7;

        std::shared_ptr<ContactRecord> contact;

        struct
        {
            int start                                   = 0;       // actual shown position start
            int end                                     = 7;       // actual shown position end
            int thread                                  = 0;       // thread we are showing
            int dbsize                                  = 0;       // size of elements in db
            std::unique_ptr<std::vector<SMSRecord>> sms = nullptr; // loaded sms from db
        } SMS;
      public:
        ThreadViewWindow(app::Application *app);

        virtual ~ThreadViewWindow();
        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;

        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        bool onDatabaseMessage(sys::Message *msgl) override;
        void rebuild() override;
        void buildInterface() override;

        void destroyInterface() override;
        gui::Text *text = nullptr;
        void rebuildText();
        void addTimeLabel(HBox *layout, Label *timeLabel, uint16_t widthAvailable) const;
        void addErrorLabel(HBox *layout, uint16_t widthAvailable) const;
    };

} /* namespace gui */
