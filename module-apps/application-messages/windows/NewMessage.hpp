#pragma once

#include <AppWindow.hpp>
#include <PhoneNumber.hpp>
#include <widgets/Text.hpp>
#include <service-db/api/DBServiceAPI.hpp>

#include <string>

namespace gui
{
    class NewSMS_Window : public AppWindow
    {
      private:
        inline static const std::uint32_t getThreadTimeout = 1000;

        gui::Text *recipient = nullptr;
        gui::Text *message   = nullptr;
        gui::VBox *body      = nullptr;
        std::shared_ptr<ContactRecord> contact;
        utils::PhoneNumber::View phoneNumber;
        std::uint32_t numberId = 0;

        bool selectContact();
        bool sendSms();
        bool switchToThreadWindow(const utils::PhoneNumber::View &number);
        void updateBottomBar();

      public:
        NewSMS_Window(app::Application *app);
        virtual ~NewSMS_Window() = default;

        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void buildInterface() override;
    };

} /* namespace gui */
