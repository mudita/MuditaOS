/*
 * @file ApplicationTest.hpp
 * @author Kuba Kleczkowski (jakub.kleczkowski@mudita.com)
 * @date 5 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_TEST_APPLICATIONTEST_HPP_
#define MODULE_APPS_APPLICATION_TEST_APPLICATIONTEST_HPP_

#include "Application.hpp"
#include "Service/Message.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Progress.hpp"
namespace app{

class ApplicationTest : public Application{

	uint32_t timer_id= 0;
	gui::Rect* rectangle;
	gui::Rect* rect;
	gui::Progress* bar;

public:
	ApplicationTest(std::string name,uint32_t stackDepth=4096,sys::ServicePriority priority=sys::ServicePriority::Idle);
	virtual ~ApplicationTest();

	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;
	void TickHandler(uint32_t id) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;
	sys::ReturnCodes WakeUpHandler() override;
	sys::ReturnCodes SleepHandler() override;

	void createUserInterface() ;
	void destroyUserInterface();
};
class ApplicationTestLauncher : public ApplicationLauncher {
public:
	ApplicationTestLauncher() : ApplicationLauncher("ApplicationTest", true) {};
	bool run(sys::SystemManager* sysmgr) override {
		return sysmgr->CreateService(std::make_shared<app::ApplicationTest>(name),sysmgr,1000);
	};
};
}

#endif /* MODULE_APPS_APPLICATION_TEST_APPLICATIONTEST_HPP_ */
