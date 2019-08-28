/*
 * @file ApplicationNotes.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 30 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_NOTES_APPLICATIONNOTES_HPP_
#define MODULE_APPS_APPLICATION_NOTES_APPLICATIONNOTES_HPP_

#include "NotesModel.hpp"
#include "Application.hpp"

namespace app {

class ApplicationNotes: public Application {
protected:

public:
	ApplicationNotes( std::string name="ApplicationNotes", std::string parent = "", bool startBackgound = false);
	virtual ~ApplicationNotes();
	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;
	sys::ReturnCodes WakeUpHandler() override;
	sys::ReturnCodes SleepHandler() override;

	void createUserInterface() ;
	void destroyUserInterface();
};

class ApplicationNotesLauncher : public ApplicationLauncher {
public:
	ApplicationNotesLauncher() : ApplicationLauncher("ApplicationNotes", true ) {};
	bool run(sys::Service* caller = nullptr ) override {
		parent = (caller==nullptr?"":caller->GetName());
		return sys::SystemManager::CreateService( std::make_shared<ApplicationNotes>(name, parent), caller );
	};
	bool runBackground(sys::Service* caller ) override {
		parent = (caller==nullptr?"":caller->GetName());
		return sys::SystemManager::CreateService( std::make_shared<ApplicationNotes>(name, parent ),caller);
	};
};

} /* namespace app */




#endif /* MODULE_APPS_APPLICATION_NOTES_APPLICATIONNOTES_HPP_ */
