/*
 * @file ApplicationSesttings.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 8 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_SETTINGS_APPLICATIONSETTINGS_HPP_
#define MODULE_APPS_APPLICATION_SETTINGS_APPLICATIONSETTINGS_HPP_

#include "Application.hpp"

namespace app {

/*
 *
 */
class ApplicationSettings: public app::Application {
public:
	ApplicationSettings( std::string name="ApplicationSettings", std::string parent = "", bool startBackgound = false);
	virtual ~ApplicationSettings();
	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;
	sys::ReturnCodes WakeUpHandler() override;
	sys::ReturnCodes SleepHandler() override;

	void createUserInterface() ;
	void destroyUserInterface();
};

class ApplicationSettingsLauncher : public ApplicationLauncher {
public:
	ApplicationSettingsLauncher() : ApplicationLauncher("ApplicationSettings", true ) {};
	bool run(sys::Service* caller = nullptr ) override {
		parent = (caller==nullptr?"":caller->GetName());
		return sys::SystemManager::CreateService( std::make_shared<ApplicationSettings>(name, parent), caller );
	};
	bool runBackground(sys::Service* caller ) override {
		parent = (caller==nullptr?"":caller->GetName());
		return sys::SystemManager::CreateService( std::make_shared<ApplicationSettings>(name, parent ),caller);
	};
};

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_SETTINGS_APPLICATIONSETTINGS_HPP_ */
