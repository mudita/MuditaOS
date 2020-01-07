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
        };
    }; // namespace name
};     // namespace gui

namespace app {

inline const std::string name_messages =  "ApplicationMessages";

class ApplicationMessages: public app::Application {
public:
	ApplicationMessages(std::string name = name_messages,
			std::string parent = "", bool startBackgound = false);
	virtual ~ApplicationMessages();

    gui::OptionWindow *threadOptionsWindow = nullptr;

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl,
			sys::ResponseMessage *resp) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;

	sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode)
			override final {
		return sys::ReturnCodes::Success;
	}

	void createUserInterface() override;
	void destroyUserInterface() override;

    bool removeSMS_thread(const ThreadRecord *record);
};
} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_MESSAGES_APPLICATIONMESSAGES_HPP_ */
