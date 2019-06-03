/*
 * @file ApplicationClock.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_CLOCK_APPLICATIONCLOCK_HPP_
#define MODULE_APPS_APPLICATION_CLOCK_APPLICATIONCLOCK_HPP_

#include "Application.hpp"
#include "Service/Message.hpp"
#include "gui/widgets/Label.hpp"

namespace app {

/*
 *
 */
class ApplicationClock: public Application {
	uint32_t timer_id= 0;

	uint32_t seconds = 0;
	uint32_t hour = 0;
	uint32_t minute = 0;
	gui::Label* timeLabel = nullptr;
public:
	ApplicationClock(std::string name,uint32_t stackDepth=4096,sys::ServicePriority priority=sys::ServicePriority::Idle);
	virtual ~ApplicationClock();

	 sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;
	void TickHandler(uint32_t id) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;
	sys::ReturnCodes WakeUpHandler() override;
	sys::ReturnCodes SleepHandler() override;

	void createUserInterface() ;
	void destroyUserInterface();
};

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_CLOCK_APPLICATIONCLOCK_HPP_ */
