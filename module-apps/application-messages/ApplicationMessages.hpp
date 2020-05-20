#pragma once

#include <Application.hpp>
#include <Interface/ThreadRecord.hpp>
#include <Interface/SMSTemplateRecord.hpp>
#include <Interface/SMSRecord.hpp>

namespace gui
{
    // fw declarations
    class OptionWindow;
    class Text;
    namespace name
    {
        namespace window
        {
            inline const std::string dialog_yes_no     = "DialogYesNo";
            inline const std::string dialog_confirm    = "DialogConfirm";
            inline const std::string dialog            = "Dialog";
            inline const std::string new_sms           = "NewSMS";
            inline const std::string thread_sms_search = "SMSSearch";
            inline const std::string sms_templates     = "SMSTemplates";
            inline const std::string thread_view       = "ThreadViewWindow";

        }; // namespace window
    };     // namespace name
};         // namespace gui

namespace app
{

    inline const std::string name_messages = "ApplicationMessages";

    class ApplicationMessages : public app::Application
    {
      public:
        ApplicationMessages(std::string name = name_messages, std::string parent = "", bool startBackgound = false);
        virtual ~ApplicationMessages();

        gui::OptionWindow *windowOptions = nullptr;

        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;

        bool removeSMS(const SMSRecord &record);
        bool removeSMS_thread(const ThreadRecord *record);
        /// show dialog with big search icon and text which was used for query
        bool searchEmpty(const std::string &query = "");
        bool showSearchResults(const UTF8 &title, const UTF8 &search_text);
        bool sendSms(const UTF8 &number, const UTF8 &body);
        bool resendSms(const SMSRecord &record);
        bool newMessageOptions(const std::string &requestingWindow, gui::Text *text);
        bool showNotification(std::function<bool()> action, bool ignoreCurrentWindowOnStack = false);
        bool handleSendSmsFromThread(const UTF8 &number, const UTF8 &body);

        // used by sms template items
        std::function<bool(std::shared_ptr<SMSTemplateRecord> templ)> templatesCallback;
    };
} /* namespace app */
