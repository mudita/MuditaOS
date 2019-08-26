/*
 * @file ApplicationViewer.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_CLOCK_ApplicationViewer_HPP_
#define MODULE_APPS_APPLICATION_CLOCK_ApplicationViewer_HPP_

#include "Application.hpp"
#include "Service/Message.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Progress.hpp"

namespace app {

/*
 *
 */
class ApplicationViewer: public Application {

public:
	ApplicationViewer(std::string name,uint32_t stackDepth=4096,sys::ServicePriority priority=sys::ServicePriority::Idle);
	virtual ~ApplicationViewer();

	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;
	sys::ReturnCodes WakeUpHandler() override;
	sys::ReturnCodes SleepHandler() override;

	void createUserInterface() ;
	void destroyUserInterface();
};

class ApplicationViewerLauncher : public ApplicationLauncher {
public:
	ApplicationViewerLauncher() : ApplicationLauncher("ApplicationViewer", true) {};
	bool run(sys::SystemManager* sysmgr) override {
		return sysmgr->CreateService(std::make_shared<app::ApplicationViewer>(name),sysmgr,1000);
	};
};

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_CLOCK_ApplicationViewer_HPP_ */
