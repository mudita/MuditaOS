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

namespace app {

class ApplicationMessages: public app::Application {
public:
	ApplicationMessages( std::string name="ApplicationMessages", std::string parent="", bool startBackgound = false);
	virtual ~ApplicationMessages();

	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;

	sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final{return sys::ReturnCodes::Success;}

	void createUserInterface() override;
	void destroyUserInterface() override;
};

class ApplicationMessagesLauncher : public ApplicationLauncher {
public:
	ApplicationMessagesLauncher() : ApplicationLauncher("ApplicationMessages", true ) {};
	bool run(sys::Service* caller = nullptr ) override {
		parent = (caller==nullptr?"":caller->GetName());
		return sys::SystemManager::CreateService( std::make_shared<ApplicationMessages>(name, parent), caller );
	};
	bool runBackground(sys::Service* caller ) override {
		parent = (caller==nullptr?"":caller->GetName());
		return sys::SystemManager::CreateService( std::make_shared<ApplicationMessages>(name, parent ),caller);
	};
};
} /* namespace app */




#endif /* MODULE_APPS_APPLICATION_MESSAGES_APPLICATIONMESSAGES_HPP_ */
