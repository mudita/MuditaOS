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

	ApplicationCalendar(std::string name, std::string parent, uint32_t stackDepth=4096,sys::ServicePriority priority=sys::ServicePriority::Idle);
	virtual ~ApplicationCalendar();

	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) override;
	void TickHandler(uint32_t id) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final{return sys::ReturnCodes::Success;}

	void createUserInterface() override;
	void destroyUserInterface() override;
};

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_CALENDAR_APPLICATIONCALENDAR_HPP_ */
