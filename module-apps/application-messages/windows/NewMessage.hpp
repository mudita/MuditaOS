// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <chrono>
#include <string>

#include <AppWindow.hpp>
#include <PhoneNumber.hpp>
#include <widgets/Text.hpp>
#include <module-db/Interface/SMSRecord.hpp>
#include <module-db/Interface/ContactRecord.hpp>

namespace gui
{
    class NewMessageWindow : public AppWindow
    {
      public:
        explicit NewMessageWindow(app::Application *app);

        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose() override;
        void buildInterface() override;

      private:
        bool selectContact();
        bool sendSms();
        bool switchToThreadWindow(const utils::PhoneNumber::View &number);
        void updateBottomBar();
        auto getPhoneNumber() const -> utils::PhoneNumber;
        auto handleMessageText() -> bool;
        auto addDraft(const ContactRecord &contactRecord) -> bool;
        auto addDraft(const utils::PhoneNumber &number) -> bool;
        auto addDraftToExistingThread(unsigned int threadId, const utils::PhoneNumber::View &number, const UTF8 &text)
            -> bool;
        void storeMessageDraft(const utils::PhoneNumber::View &number, const UTF8 &text);
        void storeMessageDraft(SMSRecord &sms, const UTF8 &text);

        static inline constexpr std::chrono::milliseconds getThreadTimeout{1000};
        gui::Text *recipient = nullptr;
        gui::Text *message   = nullptr;
        gui::VBox *body      = nullptr;
        std::shared_ptr<ContactRecord> contact;
        utils::PhoneNumber::View phoneNumber;
    };
} /* namespace gui */
