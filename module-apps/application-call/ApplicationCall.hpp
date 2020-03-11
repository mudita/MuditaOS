#pragma once

#include "Application.hpp"
#include "Service/Message.hpp"
#include "SystemManager/SystemManager.hpp"
#include <service-cellular/api/CellularServiceAPI.hpp>

namespace app {

inline const std::string name_call = "ApplicationCall";
namespace window
{
    inline const std::string name_call = "CallWindow";
    inline const std::string name_enterNumber = "EnterNumberWindow";
    inline const std::string name_emergencyCall = "EmergencyCallWindow";
    inline const std::string name_duplicatedContact = "DuplicatedContactWindow";
} // namespace window
/*
 *
 */
class ApplicationCall: public Application {
  private:
    void CallAbortHandler();
    void CallActiveHandler();
    void IncomingCallHandler(const CellularNotificationMessage *const msg);
    void RingingHandler(const CellularNotificationMessage *const msg);

  protected:
	std::string phoneNumber;
	AppTimer timerCall;
    time_t callStartTime = std::numeric_limits<time_t>::max();
    ;
    time_t callDuration = 0;
    time_t callDelayedDuration = std::numeric_limits<time_t>::max();
    void timerCallCallback();
public:
	ApplicationCall( std::string name=name_call, std::string parent = "", bool startBackgound = false );
    ~ApplicationCall() override = default;
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
};

} /* namespace app */
