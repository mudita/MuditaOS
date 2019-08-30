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

public:
	ApplicationClock(std::string name, std::string parent = "", uint32_t stackDepth=4096,sys::ServicePriority priority=sys::ServicePriority::Idle);
	virtual ~ApplicationClock();

	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) override;
	void TickHandler(uint32_t id) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final{return sys::ReturnCodes::Success;}

	void createUserInterface() ;
	void destroyUserInterface();
};

class ApplicationClockLauncher : public ApplicationLauncher {
public:
	ApplicationClockLauncher() : ApplicationLauncher("ApplicationClock", true ) {};
	bool run(sys::Service* caller = nullptr ) override {
		parent = (caller==nullptr?"":caller->GetName());
		return sys::SystemManager::CreateService( std::make_shared<ApplicationClock>(name, parent), caller );
	};
	bool runBackground(sys::Service* caller ) override {
		parent = (caller==nullptr?"":caller->GetName());
		return sys::SystemManager::CreateService( std::make_shared<ApplicationClock>(name, parent ),caller);
	};
};

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_CLOCK_APPLICATIONCLOCK_HPP_ */
