#pragma once

#include <AppWindow.hpp>
#include <application-messages/widgets/SMSInputWidget.hpp>
#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/Image.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/Window.hpp>
#include <ListView.hpp>
#include <PhoneNumber.hpp>
#include <service-db/api/DBServiceAPI.hpp>
#include <Text.hpp>

#include <functional>
#include <string>

namespace gui
{
    class SMSThreadViewWindow : public AppWindow
    {
      private:
        gui::VBox *body         = nullptr;
        uint16_t elements_width = this->getWidth() - style::window::default_left_margin * 2;
        void cleanView();
        enum class Action
        {
            Init,        /// first load of sms thread view
            NewestPage,  /// show a sms thread page from the latest sms
            Refresh,     /// just refresh current view
            NextPage,    /// load previous page
            PreviousPage /// load next page
        };
        /// return if request was handled
        bool showMessages(Action what);
        void addSMS(Action what);
        bool smsBuild(const SMSRecord &smsRecord);
        Label *timeLabelBuild(time_t timestamp) const;
        HBox *smsSpanBuild(Text *smsBubble, const SMSRecord &el) const;
        const ssize_t maxsmsinwindow = 7;

        std::shared_ptr<ContactRecord> contact;
        std::unique_ptr<utils::PhoneNumber::View> number;

        struct
        {
            int start                                   = 0;       // actual shown position start
            int end                                     = 7;       // actual shown position end
            int thread                                  = 0;       // thread we are showing
            int dbsize                                  = 0;       // size of elements in db
            std::unique_ptr<std::vector<SMSRecord>> sms = nullptr; // loaded sms from db
        } SMS;

        gui::SMSInputWidget *inputMessage                 = nullptr;
        inline static const std::uint32_t numberIdTimeout = 1000;

      public:
        SMSThreadViewWindow(app::Application *app);

        virtual ~SMSThreadViewWindow();
        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;

        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        bool onDatabaseMessage(sys::Message *msgl) override;
        void rebuild() override;
        void buildInterface() override;

        void destroyInterface() override;
        void destroyTextItem();
        void refreshTextItem();
        void addTimeLabel(HBox *layout, Label *timeLabel, uint16_t widthAvailable) const;
        void addErrorIcon(HBox *layout) const;
    };

} /* namespace gui */
