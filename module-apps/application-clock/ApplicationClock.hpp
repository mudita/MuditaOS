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
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Progress.hpp"


namespace app {

/*
 *
 */
class ApplicationClock: public Application {
	uint32_t timer_id= 0;

	uint32_t seconds = 0;
	uint32_t hour = 0;
	uint32_t minute = 0;
	gui::Label* hourLabel = nullptr;
	gui::Label* minuteLabel = nullptr;
	gui::Image* dotImage[64];
	gui::Progress* progressBar = nullptr;

	/**
	 * @brief Increments hours counter
	 */
	bool incrementHour();
	/**
	 * @brief Increments minutes counter
	 */
	bool incrementMinute();
	/**
	 * @brief Increments seconds counter
	 */
	bool incrementSecond();
	/**
	 * @brief Updates strings for hour and minutes
	 */
	void updateLabels();
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

class ApplicationClockLauncher : public ApplicationLauncher {
public:
	ApplicationClockLauncher() : ApplicationLauncher("ApplicationClock", true) {};
	bool run(sys::SystemManager* sysmgr) override {
		return sysmgr->CreateService(std::make_shared<app::ApplicationClock>(name),sysmgr,1000);
	};
};

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_CLOCK_APPLICATIONCLOCK_HPP_ */
