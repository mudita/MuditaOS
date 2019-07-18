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
	uint32_t timer_id= 0;
public:
	ApplicationCall( std::string name="ApplicationCall");
	virtual ~ApplicationCall();
	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;
	sys::ReturnCodes WakeUpHandler() override;
	sys::ReturnCodes SleepHandler() override;

	void createUserInterface() ;
	void destroyUserInterface();
	void setDisplayedNumber( std::string num );
	const std::string& getDisplayedNumber();

	/**
	 * @brief Performs switch to call application providing first character in number.
	 */
	void messageSwitchToEnterPhone( sys::Service* sender );
};

class ApplicationCallLauncher : public ApplicationLauncher {
public:
	ApplicationCallLauncher() : ApplicationLauncher("ApplicationCall", false) {};
	bool run(sys::SystemManager* sysmgr) override {
		return sysmgr->CreateService(std::make_shared<ApplicationCall>(name),sysmgr);
	};
};

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_CALL_APPLICATIONCALL_HPP_ */
