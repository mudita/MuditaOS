#pragma once

#include "Application.hpp"

namespace app {

class ApplicationTest: public app::Application {
public:
	ApplicationTest( std::string name="ApplicationTest", bool startBackgound = false);
	virtual ~ApplicationTest();
	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;
	sys::ReturnCodes WakeUpHandler() override;
	sys::ReturnCodes SleepHandler() override;

	void createUserInterface() override;
	void destroyUserInterface() override;
};

}
