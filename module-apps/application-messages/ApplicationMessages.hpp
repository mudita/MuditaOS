/*
 * @file ApplicationMessages.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 25 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_MESSAGES_APPLICATIONMESSAGES_HPP_
#define MODULE_APPS_APPLICATION_MESSAGES_APPLICATIONMESSAGES_HPP_

#include "Application.hpp"
#include "Interface/ThreadRecord.hpp"

namespace gui
{
    class OptionWindow;
    namespace name
    {
        namespace window
        {
            inline const std::string thread_rm_confirm = "ConfirmRM";
            inline const std::string thread_search_none = "SearchNone";
        };
    }; // namespace name
};     // namespace gui

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

        bool removeSMS_thread(const ThreadRecord *record);
        /// show dialog with big search icon and text which was used for query
        bool searchEmpty(const std::string &query = "");
        bool sendSms(const UTF8 &number, const UTF8 &body);
        bool newMessageOptions();
    };
} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_MESSAGES_APPLICATIONMESSAGES_HPP_ */
