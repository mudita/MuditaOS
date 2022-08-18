// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Constants.hpp"

#include <Application.hpp>
#include <Interface/SMSRecord.hpp>
#include <Interface/SMSTemplateRecord.hpp>
#include <Interface/ThreadRecord.hpp>
#include <PhoneNumber.hpp>

namespace gui
{
    // fw declarations
    class OptionWindow;
    class Text;
} // namespace gui

namespace app
{

    class ApplicationMessages : public app::Application, public app::AsyncCallbackReceiver
    {
      public:
        explicit ApplicationMessages(std::string name                    = name_messages,
                                     std::string parent                  = {},
                                     StatusIndicators statusIndicators   = StatusIndicators{},
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
        bool showNotification(std::function<bool()> action,
                              const std::string &notification,
                              bool ignoreCurrentWindowOnStack = false);
        bool handleSendSmsFromThread(const utils::PhoneNumber::View &number, const UTF8 &body);

        void createDraft(const utils::PhoneNumber::View &number,
                         const UTF8 &body,
                         std::function<void(const SMSRecord &)> onSuccess = {});
        bool updateDraft(SMSRecord &record, const UTF8 &body);
        bool removeDraft(const SMSRecord &record);

        bool onRemoveSmsThreadConfirmed(const ThreadRecord &record);
        bool onRemoveSmsConfirmed(const SMSRecord &record);

        // used by sms template items
        std::function<bool(std::shared_ptr<SMSTemplateRecord> templ)> templatesCallback;
    };

    template <>
    struct ManifestTraits<ApplicationMessages>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch,
                     manager::actions::CreateSms,
                     manager::actions::ShowSmsTemplates,
                     manager::actions::SmsRejectNoSim,
                     manager::actions::SMSRejectedByOfflineNotification,
                     manager::actions::PhoneModeChanged,
                     manager::actions::BluetoothModeChanged,
                     manager::actions::AlarmClockStatusChanged}};
        }
    };
} /* namespace app */
