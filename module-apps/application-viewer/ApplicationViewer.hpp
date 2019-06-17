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

#include "vfs.hpp"

#include "Application.hpp"
#include "Service/Message.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Progress.hpp"

namespace app {

class ViewerState {
public:
	std::string stateName;
	std::string screenName;
	bool deepRefresh = false;
	uint32_t id = 0;
	uint32_t leftState = 0, middleState = 0, rightState = 0;

	ViewerState( std::string stateName, std::string screenName, bool deep, uint32_t id, uint32_t l, uint32_t m, uint32_t r ) :
		stateName{stateName}, screenName{screenName}, deepRefresh{deep}, id{id}, leftState{l}, middleState{m}, rightState{r} {};
};

/*
 *
 */
class ApplicationViewer: public Application {
	uint32_t currentState = 0;
	std::vector<gui::Image*> images;
	std::vector<ViewerState> states;
public:
	ApplicationViewer(std::string name,uint32_t stackDepth=4096,sys::ServicePriority priority=sys::ServicePriority::Idle);
	virtual ~ApplicationViewer();

	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;
	void TickHandler(uint32_t id) override;
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
