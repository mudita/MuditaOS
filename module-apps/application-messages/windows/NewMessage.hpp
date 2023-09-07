// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <chrono>
#include <string>

#include <AsyncTask.hpp>
#include <AppWindow.hpp>
#include <PhoneNumber.hpp>
#include <widgets/text/Text.hpp>
#include <module-db/Interface/SMSRecord.hpp>
#include <module-db/Interface/ContactRecord.hpp>

namespace gui
{
    class NewMessageWindow : public AppWindow,
                             public app::AsyncCallbackReceiver,
                             public gui::InfoAboutPreviousAppWhereWeComeFrom
    {
      public:
        explicit NewMessageWindow(app::ApplicationCommon *app);

        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void buildInterface() override;
        void onClose(CloseReason reason) override;

      private:
        bool selectContact();
        bool sendSms();
        bool switchToThreadWindow(const utils::PhoneNumber::View &number);
        void updateNavBar();
        auto getPhoneNumber() const -> utils::PhoneNumber;
        auto handleMessageText() -> bool;
        auto addDraft(const utils::PhoneNumber &number) -> bool;
        auto addDraftToExistingThread(unsigned int threadId, const utils::PhoneNumber::View &number, const UTF8 &text)
            -> bool;
        void storeMessageDraft(const utils::PhoneNumber::View &number, const UTF8 &text);
        void storeMessageDraft(SMSRecord &sms, const UTF8 &text);
        gui::Text *recipient = nullptr;
        gui::Text *message   = nullptr;
        gui::VBox *body      = nullptr;
        std::shared_ptr<ContactRecord> contact;
        utils::PhoneNumber::View phoneNumber;
        unsigned int selectedNumber = 0;

        /// MessageMemento class provides buffer-like functionality for message field of NewMessageWindow.
        /// MessageMemento shall be used whenever there is a need for temporary window switch in normal (uninterrupted)
        /// flow of new message preparation, such as using options or recipient selection.
        class MessageMemento;
        static std::unique_ptr<MessageMemento> mementoMessage;
        static std::unique_ptr<MessageMemento> mementoRecipient;
    };
} /* namespace gui */
