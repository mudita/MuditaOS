#pragma once

#include "Application.hpp"

namespace app {

class ApplicationPhonebook: public app::Application {
public:
	ApplicationPhonebook( std::string name="ApplicationPhonebook");
	virtual ~ApplicationPhonebook();
	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;
	sys::ReturnCodes WakeUpHandler() override;
	sys::ReturnCodes SleepHandler() override;

	void createUserInterface() ;
	void destroyUserInterface();
};

class ApplicationPhonebookLauncher : public ApplicationLauncher {
public:
	ApplicationPhonebookLauncher() : ApplicationLauncher("ApplicationPhonebook", true) {};
	bool run(sys::SystemManager* sysmgr) override {
		return sysmgr->CreateService(std::make_shared<ApplicationPhonebook>(name),sysmgr, 1000);
	};
};

}
