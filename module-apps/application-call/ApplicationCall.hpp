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

inline const std::string name_call = "ApplicationCall";
namespace window
{
    inline const std::string name_call = "CallWindow";
    inline const std::string name_enterNumber = "EnterNumberWindow";
    inline const std::string name_emergencyCall = "EmergencyCallWindow";
} // namespace window
/*
 *
 */
class ApplicationCall: public Application {
protected:
	std::string phoneNumber;
	AppTimer timerCall;
	uint32_t callDuration = 0;
	uint32_t callEndTime = -1;
    void timerCallCallback();
public:
	ApplicationCall( std::string name=name_call, std::string parent = "", bool startBackgound = false );
	virtual ~ApplicationCall();
	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final{return sys::ReturnCodes::Success;}

    void createUserInterface() override;
    void destroyUserInterface() override;
    void setDisplayedNumber( std::string num );
	const std::string& getDisplayedNumber();
	//starts timer that upon triggering, if no user action was performed, will hide application to background and move to previous application
	void runCallTimer();
	void stopCallTimer();
	/**
	 * @brief Performs switch to call application with the number needed to perform the call
	 *
	 */
    static bool messageSwitchToCall(sys::Service *sender, const UTF8 &e164number, bool call = true);
};

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_CALL_APPLICATIONCALL_HPP_ */
