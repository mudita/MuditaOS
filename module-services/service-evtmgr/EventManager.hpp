/*
 * EventManager.hpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#ifndef MODULE_SERVICES_SERVICE_KBD_EVENTMANAGER_HPP_
#define MODULE_SERVICES_SERVICE_KBD_EVENTMANAGER_HPP_

#include "bsp/keyboard/key_codes.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "Service/Worker.hpp"
#include "MessageType.hpp"

#include "bsp/common.hpp"

class WorkerEvent;

class EventManager: public sys::Service {
private:
	void HandleAlarmTrigger(sys::DataMessage* msgl);
	void GetNextAlarmTimestamp(time_t timestamp);
protected:
	std::unique_ptr<WorkerEvent> EventWorker;
	//application where key events are sent. This is also only application that is allowed to change keyboard long press settings.
	std::string targetApplication;
	//alarm timestamp in seconds from midnight
	uint32_t alarmTimestamp;
	//ID of alarm waiting to trigger
	uint32_t alarmID;
	//flag set when there is no alarm to trigger in current day
	bool alarmDBEmpty = false;
	//flag set when there is alarm to handle
	bool alarmIsValid = false;
public:
	EventManager(const std::string& name);
    ~EventManager();

    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;


    /**
	* @brief Sends request to application manager to switch from current application to specific window in application with specified name .
	*/
    static bool messageSetApplication( sys::Service* sender, const std::string& applicationName );
};

#endif /* MODULE_SERVICES_SERVICE_KBD_EventManager_HPP_ */
