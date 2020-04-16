#pragma once

#include <AppWindow.hpp>
#include <widgets/Text.hpp>
#include <service-db/api/DBServiceAPI.hpp>

#include <string>

namespace gui
{

    namespace name
    {
        namespace window
        {
            inline std::string new_sms = "NewSMS";
        };
    }; // namespace name

    class NewSMS_Window : public AppWindow
    {
      private:
        gui::Text *recipient = nullptr;
        gui::Text *message   = nullptr;
        gui::VBox *body      = nullptr;
        std::shared_ptr<ContactRecord> contact;

        bool selectContact();
        bool sendSms();

      public:
        NewSMS_Window(app::Application *app);
        virtual ~NewSMS_Window() = default;

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void buildInterface() override;
    };

} /* namespace gui */
