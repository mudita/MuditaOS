/*
 * @file ApplicationCall.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_CALL_APPLICATIONCALL_HPP_
#define MODULE_APPS_APPLICATION_CALL_APPLICATIONCALL_HPP_

#include "Application.hpp"
#include "SystemManager/SystemManager.hpp"
#include "Service/Message.hpp"

namespace app {

/*
 *
 */
class ApplicationCall: public Application {
protected:
	std::string phoneNumber;
	uint32_t timerCall = 0;
	uint32_t callDuration = 0;
	uint32_t callEndTime = -1;
public:
	ApplicationCall( std::string name="ApplicationCall", std::string parent = "", bool startBackgound = false );
	virtual ~ApplicationCall();
	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;
	void TickHandler(uint32_t id) override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final{return sys::ReturnCodes::Success;}

	void createUserInterface() ;
	void destroyUserInterface();
	void setDisplayedNumber( std::string num );
	const std::string& getDisplayedNumber();
	//starts timer that upon triggering, if no user action was performed, will hide application to background and move to previous application
	void runCallTimer();
	void stopCallTimer();
	/**
	 * @brief Performs switch to call application providing first character in number.
	 */
	void messageSwitchToEnterPhone( sys::Service* sender );
};

class ApplicationCallLauncher : public ApplicationLauncher {
public:
	ApplicationCallLauncher() : ApplicationLauncher("ApplicationCall", false ) {};
	bool run(sys::Service* caller = nullptr ) override {
		parent = (caller==nullptr?"":caller->GetName());
		return sys::SystemManager::CreateService( std::make_shared<ApplicationCall>(name, parent), caller );
	};
	bool runBackground(sys::Service* caller ) override {
		parent = (caller==nullptr?"":caller->GetName());
		return sys::SystemManager::CreateService( std::make_shared<ApplicationCall>(name, parent ),caller);
	};
};

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_CALL_APPLICATIONCALL_HPP_ */
