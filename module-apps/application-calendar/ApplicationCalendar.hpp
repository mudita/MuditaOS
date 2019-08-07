/*
 * @file ApplicationCalendar.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 3 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_CALENDAR_APPLICATIONCALENDAR_HPP_
#define MODULE_APPS_APPLICATION_CALENDAR_APPLICATIONCALENDAR_HPP_

#include "Application.hpp"
#include "Service/Message.hpp"
#include "SystemManager/SystemManager.hpp"
#include "gui/widgets/Label.hpp"

namespace app {

/*
 * @brief Class representing calendar application
 */
class ApplicationCalendar: public Application {
public:

	ApplicationCalendar(std::string name,uint32_t stackDepth=4096,sys::ServicePriority priority=sys::ServicePriority::Idle);
	virtual ~ApplicationCalendar();

	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;
	void TickHandler(uint32_t id) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;
	sys::ReturnCodes WakeUpHandler() override;
	sys::ReturnCodes SleepHandler() override;

	void createUserInterface() ;
	void destroyUserInterface();
};

class ApplicationCalendarLauncher : public ApplicationLauncher {
public:
	ApplicationCalendarLauncher() : ApplicationLauncher("ApplicationCalendar", true) {};
	bool run(sys::SystemManager* sysmgr) override {
		return sysmgr->CreateService(std::make_shared<app::ApplicationCalendar>(name),sysmgr,1000);
	};
	bool runBackground(sys::SystemManager* sysmgr) {
		startBackground = true;
		return sysmgr->CreateService(std::make_shared<app::ApplicationCalendar>(name),sysmgr,1000);
	};
};

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_CALENDAR_APPLICATIONCALENDAR_HPP_ */
