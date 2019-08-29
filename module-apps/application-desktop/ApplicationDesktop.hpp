/*
 * @file ApplicationDesktop.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 18 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_DESKTOP_APPLICATIONDESKTOP_HPP_
#define MODULE_APPS_APPLICATION_DESKTOP_APPLICATIONDESKTOP_HPP_

#include "Application.hpp"
#include "Service/Message.hpp"

namespace app {

/*
 *
 */
class ApplicationDesktop : public Application {
protected:
	//determines whether screen should be protected by pin verification
	bool screenLocked = true;
	bool pinLocked = false;
	uint32_t unreadMessages = 0;
	uint32_t missedCalls = 0;
public:
	ApplicationDesktop( std::string name="ApplicationDesktop", std::string parent = "" );
	virtual ~ApplicationDesktop();
	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final{return sys::ReturnCodes::Success;}

	void createUserInterface() ;
	void destroyUserInterface();
	bool getScreenLocked();
	void setScreenLocked( bool val );
	bool getPinLocked();

	uint32_t getMisseedCalls();
	uint32_t getUnreadMessages();

	/**
	 * This static method will be used to lock the phone
	 */
//	static bool messageLockPhone( sys::Service* sender, std::string application , const gui::InputEvent& event );
};

class ApplicationDesktopLauncher : public ApplicationLauncher {
public:
	ApplicationDesktopLauncher() : ApplicationLauncher("ApplicationDesktop", false) {};
	bool run(sys::Service* caller = nullptr ) override {
		parent = (caller==nullptr?"":caller->GetName());
		return sys::SystemManager::CreateService( std::make_shared<ApplicationDesktop>(name, parent), caller );
	};
	bool runBackground(sys::Service* caller ) override {
		parent = (caller==nullptr?"":caller->GetName());
		return sys::SystemManager::CreateService( std::make_shared<ApplicationDesktop>(name, parent ),caller);
	};
};


} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_DESKTOP_APPLICATIONDESKTOP_HPP_ */
