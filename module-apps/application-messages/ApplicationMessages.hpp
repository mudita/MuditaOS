// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <Interface/ThreadRecord.hpp>
#include <Interface/SMSTemplateRecord.hpp>
#include <Interface/SMSRecord.hpp>
#include <PhoneNumber.hpp>

namespace gui
{
    // fw declarations
    class OptionWindow;
    class Text;
    namespace name
    {
        namespace window
        {
            inline constexpr auto dialog_yes_no     = "DialogYesNo";
            inline constexpr auto dialog_confirm    = "DialogConfirm";
            inline constexpr auto dialog            = "Dialog";
            inline constexpr auto new_sms           = "NewSMS";
            inline constexpr auto thread_sms_search = "SMSSearch";
            inline constexpr auto sms_templates     = "SMSTemplates";
            inline constexpr auto thread_view       = "ThreadViewWindow";

        }; // namespace window
    };     // namespace name
};         // namespace gui

namespace app
{

    inline constexpr auto name_messages = "ApplicationMessages";

    class ApplicationMessages : public app::Application, public app::AsyncCallbackReceiver
    {
      public:
        ApplicationMessages(std::string name                    = name_messages,
                            std::string parent                  = {},
                            StartInBackground startInBackground = {false});

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;

        bool removeSms(const SMSRecord &record);
        bool removeSmsThread(const ThreadRecord *record);
        bool markSmsThreadAsRead(const uint32_t id);
        bool markSmsThreadAsUnread(const uint32_t id);
        /// show dialog with big search icon and text which was used for query
        bool searchEmpty(const std::string &query = "");
        bool showSearchResults(const UTF8 &title, const UTF8 &search_text);
        bool sendSms(const utils::PhoneNumber::View &number, const UTF8 &body);
        bool resendSms(const SMSRecord &record);
        bool newMessageOptions(const std::string &requestingWindow, gui::Text *text);
        bool showNotification(std::function<bool()> action, bool ignoreCurrentWindowOnStack = false);
        bool handleSendSmsFromThread(const utils::PhoneNumber::View &number, const UTF8 &body);

        std::pair<SMSRecord, bool> createDraft(const utils::PhoneNumber::View &number, const UTF8 &body);
        bool updateDraft(SMSRecord &record, const UTF8 &body);
        bool removeDraft(const SMSRecord &record);

        bool onRemoveSmsThreadConfirmed(const ThreadRecord &record);
        bool onRemoveSmsConfirmed(const SMSRecord &record);

        // used by sms template items
        std::function<bool(std::shared_ptr<SMSTemplateRecord> templ)> templatesCallback;
    };

    template <> struct ManifestTraits<ApplicationMessages>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch, manager::actions::CreateSms, manager::actions::ShowSmsTemplates}};
        }
    };
} /* namespace app */
