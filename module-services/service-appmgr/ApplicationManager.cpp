/*
 * @file ApplicationManager.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 3 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <memory>

#include "SystemManager/SystemManager.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-evtmgr/EventManager.hpp"
#include "messages/APMMessage.hpp"
#include "application-call/data/CallSwitchData.hpp"

#include "service-db/api/DBServiceAPI.hpp"
#include "service-cellular/ServiceCellular.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"

#include <memory>
#include <utility>

// services
#include "service-eink/ServiceEink.hpp"
#include "service-gui/ServiceGUI.hpp"

#include "application-call/ApplicationCall.hpp"
#include "application-desktop/ApplicationDesktop.hpp"
#include "application-special-input/AppSpecialInput.hpp"

/// Auto phone lock disabled for now till the times when it's debugged
/// #define AUTO_PHONE_LOCK_ENABLED

//module-utils
#include "log/log.hpp"
#include "i18/i18.hpp"

namespace sapm {

    ApplicationDescription::ApplicationDescription(std::unique_ptr<app::ApplicationLauncher> launcher) : switchData{nullptr}
    {
        this->launcher = std::move(launcher);
    }

    VirtualAppManager::VirtualAppManager(std::vector<std::unique_ptr<app::ApplicationLauncher>> &launchers)
    {
        for (uint32_t i = 0; i < launchers.size(); ++i)
        {
            applications.push_back(new ApplicationDescription(std::move(launchers[i])));
        }
    }

VirtualAppManager::~VirtualAppManager()
{
    for (auto it = applications.begin(); it != applications.end(); it++)
    {
        delete *it;
    }
}

const char *VirtualAppManager::stateStr(State st)
{
    switch (st)
    {
    case State::IDLE:
        return "IDLE";
    case State::CLOSING_PREV_APP:
        return "CLOSING_PREV_APP";
    case State::WAITING_CLOSE_CONFIRMATION:
        return "WAITING_CLOSE_CONFIRMATION";
    case State::STARTING_NEW_APP:
        return "STARTING_NEW_APP";
    case State::WAITING_NEW_APP_REGISTRATION:
        return "WAITING_NEW_APP_REGISTRATION";
    case State::WAITING_LOST_FOCUS_CONFIRMATION:
        return "WAITING_LOST_FOCUS_CONFIRMATION";
    case State::WAITING_GET_FOCUS_CONFIRMATION:
        return "WAITING_GET_FOCUS_CONFIRMATION";
    }
    // there was enum added - fix it adding it to case
    return "FIX_ME";
}

ApplicationDescription *VirtualAppManager::appFront()
{
    return applications.front();
}

ApplicationDescription *VirtualAppManager::appGet(const std::string &name)
{
    auto el = std::find_if(applications.begin(), applications.end(), [=](auto a) {
        if (a->name() == name)
            return true;
        else
            return false;
    });
    if (el == applications.end())
        return nullptr;
    else
    {
        return *el;
    }
}

/// set application as first on the applications vector
bool VirtualAppManager::appMoveFront(ApplicationDescription *app)
{
    if (!app)
    {
        return false;
    }
    auto el = std::find_if(applications.begin(), applications.end(), [=](auto a) { return a == app; });
    if (el != applications.end())
    {
        applications.push_front(std::move(*el));
        applications.erase(el);
        return true;
    }
    return false;
}

/// get previous visible app - one on FOREGROUND
ApplicationDescription *VirtualAppManager::appPrev()
{
    static bool init = true;
    if (init)
    {
        init = false;
        return nullptr;
    }
    if (applications.size() < 2)
    {
        return nullptr;
    }
    return *std::next(applications.begin());
}

void VirtualAppManager::setState(State st)
{
#ifdef DEBUG_APPLICATION_MANAGEMENT
    LOG_DEBUG("app: [%s] prev: [%s], state: (%s) -> (%s)", appFront()->name().c_str(), appPrev() ? appPrev()->name().c_str() : "", stateStr(state),
              stateStr(st));
#endif
    state = st;
}

std::list<ApplicationDescription *> &VirtualAppManager::getApps()
{
    return applications;
}

void VirtualAppManager::debug_log_app_list()
{
#ifdef DEBUG_APPLICATION_MANAGEMENT
    std::string apps = "\n";
    for (auto &el : getApps())
    {
        apps += "-> " + el->name() + " " + app::Application::stateStr(el->getState()) + "\n";
    }
    LOG_DEBUG(apps.c_str());
#endif
}

ApplicationManager::ApplicationManager(const std::string &name, sys::SystemManager *sysmgr, std::vector<std::unique_ptr<app::ApplicationLauncher>> &launchers)
    : Service(name), VirtualAppManager(launchers), systemManager{sysmgr}
{
    blockingTimerID = CreateTimer(0xFFFFFFFF, false);
}

ApplicationManager::~ApplicationManager()
{
    systemManager = nullptr;
}

bool ApplicationManager::closeServices() {
	bool ret = sys::SystemManager::DestroyService( "ServiceGUI", this );
	if( ret ) {
		LOG_INFO("Service: %s closed", "ServiceGUI");
	}else {
		LOG_FATAL("Service: %s is still running", "ServiceGUI");
	}
	ret = sys::SystemManager::DestroyService( "ServiceEink", this );
	if( ret ) {
		LOG_INFO("Service: %s closed", "ServiceEink");
	}else {
		LOG_FATAL("Service: %s is still running", "ServiceEink");
	}
	return true;
}

bool ApplicationManager::closeApplications() {

	//if application is started, its in first plane or it's working in background
	//it will be closed using SystemManager's API.
    for (auto &app : getApps())
    {
        if (app != nullptr && ((app->getState() == app::Application::State::ACTIVE_FORGROUND) ||
                               (app->getState() == app::Application::State::ACTIVE_BACKGROUND) || (app->getState() == app::Application::State::ACTIVATING)))
        {
            LOG_INFO("Closing application: %s", app->name().c_str());
            bool ret = sys::SystemManager::DestroyService(app->name(), this);
            if (ret)
            {
                LOG_INFO("Application: %s closed", app->name().c_str());
            }
            else
            {
                LOG_FATAL("Application: %s is still running", app->name().c_str());
            }
            app->setState(app::Application::State::DEACTIVATED);
        }
    }
    return true;
}

sys::Message_t ApplicationManager::DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) {

	uint32_t msgType = msgl->messageType;

	switch( msgType ) {
		case static_cast<uint32_t>( MessageType::APMInitPowerSaveMode ): {
			handlePowerSavingModeInit();
		} break;
		case static_cast<uint32_t>( MessageType::APMPreventBlocking ): {
//			LOG_INFO("Restarting screen locking timer");
			ReloadTimer(blockingTimerID);
		} break;
		case static_cast<uint32_t>(MessageType::APMSwitch): {
			sapm::APMSwitch* msg = reinterpret_cast<sapm::APMSwitch*>( msgl );
			handleSwitchApplication( msg );
		}break;
		case static_cast<uint32_t>(MessageType::APMSwitchPrevApp): {
			sapm::APMSwitchPrevApp* msg = reinterpret_cast<sapm::APMSwitchPrevApp*>( msgl );
            if (!handleSwitchPrevApplication(msg))
            {
                LOG_ERROR("Switch from app: failed", msg->getSenderName().c_str());
            }
        }break;
		case static_cast<uint32_t>(MessageType::APMConfirmSwitch): {
			sapm::APMConfirmSwitch* msg = reinterpret_cast<sapm::APMConfirmSwitch*>( msgl );
			handleSwitchConfirmation( msg );
		}break;
		case static_cast<uint32_t>(MessageType::APMConfirmClose): {
			sapm::APMConfirmClose* msg = reinterpret_cast<sapm::APMConfirmClose*>( msgl );
			LOG_INFO("APMConfirmClose %s", msg->getSenderName().c_str());
			handleCloseConfirmation( msg );

			//if application manager was waiting for close confirmation and name of the application
			//for launching is defined then start application function is called
            if ((getState() == State::WAITING_CLOSE_CONFIRMATION) && (launchApplicationName.empty() == false))
            {
                startApplication( launchApplicationName );
            }
        }break;
		case static_cast<int32_t>(MessageType::APMDeleydClose) : {
			sapm::APMDelayedClose* msg = reinterpret_cast<sapm::APMDelayedClose*>( msgl );
			LOG_INFO("APMDeleydClose %s", msg->getApplication().c_str() );
			sys::SystemManager::DestroyService(msg->getApplication().c_str(),this);
		} break;
		case static_cast<int32_t>(MessageType::APMRegister) : {
			sapm::APMRegister* msg = reinterpret_cast<sapm::APMRegister*>( msgl );
            LOG_INFO("APMregister [%s] (%s)", msg->getSenderName().c_str(), (msg->getStatus() ? "true" : "false"));
            handleRegisterApplication( msg );
		} break;
		case static_cast<int32_t>(MessageType::APMChangeLanguage) : {
			sapm::APMChangeLanguage* msg = reinterpret_cast<sapm::APMChangeLanguage*>( msgl );
			std::string lang;
			if( msg->getLanguage() == utils::Lang::En ) lang = "English";
			if( msg->getLanguage() == utils::Lang::Pl ) lang = "Polish";
			if( msg->getLanguage() == utils::Lang::De ) lang = "German";
			if( msg->getLanguage() == utils::Lang::Sp ) lang = "Spanish";
			LOG_INFO("APChangeLanguage; %s %s", msg->getSenderName().c_str(), lang.c_str());
			handleLanguageChange( msg );
		} break;
		case static_cast<uint32_t>(MessageType::APMClose): {
			closeApplications();
			closeServices();
		} break;
		default : {
		} break;
	};


	return std::make_shared<sys::ResponseMessage>();
}
// Invoked when timer ticked
void ApplicationManager::TickHandler(uint32_t id) {
#ifdef AUTO_PHONE_LOCK_ENABLED
	if( id == blockingTimerID ) {
		LOG_INFO("screen Locking Timer Triggered");
		stopTimer(blockingTimerID);

		//check if application that has focus doesn't have a flag that prevents system from blocking and going to low power mode
		ApplicationDescription* appDescription = applications.find( focusApplicationName )->second;
		if( appDescription->preventLocking ) {
			//restart timer
			ReloadTimer(blockingTimerID);
			return;
		}

		//if desktop has focus switch to main window and set it locked.
		if( focusApplicationName == "ApplicationDesktop") {
			//switch data must contain target window and information about blocking

            app::Application::messageSwitchApplication(this, "ApplicationDesktop", gui::name::window::main_window, std::make_unique<gui::LockPhoneData>());
        }
        else {
			//get the application description for application that is on top and set blocking flag for it
			appDescription->blockClosing = true;

			std::unique_ptr<gui::LockPhoneData> data = std::make_unique<gui::LockPhoneData>();
			data->setPrevApplication( focusApplicationName );

			//run normal flow of applications change
            messageSwitchApplication(this, "ApplicationDesktop", gui::name::window::main_window, std::move(data));
        }
    }
#endif
}

// Invoked during initialization
sys::ReturnCodes ApplicationManager::InitHandler() {

	//get settings to initialize language in applications
	settings = DBServiceAPI::SettingsGet(this);

	//reset blocking timer to the value specified in settings
	uint32_t lockTime = settings.lockTime;
	if( lockTime == 0 ) {
		lockTime = default_application_locktime;
    }
	setTimerPeriod( blockingTimerID, lockTime );

	if( settings.language == SettingsLanguage::ENGLISH ) {
		utils::localize.Switch( utils::Lang::En );
	}
	else if( settings.language == SettingsLanguage::POLISH ) {
		utils::localize.Switch( utils::Lang::Pl );
	}
	else if( settings.language == SettingsLanguage::GERMAN ) {
		utils::localize.Switch( utils::Lang::De );
	}
	else if( settings.language == SettingsLanguage::SPANISH ) {
		utils::localize.Switch( utils::Lang::Sp );
	}

	bool ret;
	ret = sys::SystemManager::CreateService(std::make_shared<sgui::ServiceGUI>("ServiceGUI", GetName(), 480, 600), this );
	if( !ret ) {
		LOG_ERROR("Failed to initialize GUI service");
	}
	ret = sys::SystemManager::CreateService(std::make_shared<ServiceEink>("ServiceEink", GetName() ), this );
	if( !ret ) {
		LOG_ERROR("Failed to initialize EINK service");
	}

	//search for application with specified name and run it
#if 1 //change to 0 if you want to run only viewer application for kickstarter

    // TODO this should be checked by parameter in launcher, not started by hand (if bg task -> runBackground())
    const std::string app_desktop = app::name_desktop;
    const std::vector<std::string> bg_apps = {app::name_call, app::special_input};

    for (auto &el : bg_apps)
    {
        auto app = appGet(el);
        if (app != nullptr)
        {
            app->launcher->runBackground(this);
        }
    }

    auto app = appGet(app_desktop);
    if (app != nullptr)
    {
        messageSwitchApplication(this, app->launcher->getName(), "", nullptr);
    }

#else
	std::string runCallAppName = "ApplicationViewer";

	auto it = applications.find(runCallAppName);
		if( it!= applications.end()){
			messageSwitchApplication( this, it->second->lanucher->getName(), "", nullptr );
		}
#endif

  	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationManager::DeinitHandler() {
	closeApplications();
	closeServices();
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationManager::SwitchPowerModeHandler(const sys::ServicePowerMode mode) {
    LOG_FATAL("[ServiceAppMgr] PowerModeHandler: %d", static_cast<uint32_t>(mode));

    switch (mode){
        case sys::ServicePowerMode ::Active:
            sys::SystemManager::ResumeService("ServiceEink",this);
            sys::SystemManager::ResumeService("ServiceGUI",this);
            break;
        case sys::ServicePowerMode ::SuspendToRAM:
        case sys::ServicePowerMode ::SuspendToNVM:
            sys::SystemManager::SuspendService("ServiceGUI",this);
            sys::SystemManager::SuspendService("ServiceEink",this);
            break;
    }

    return sys::ReturnCodes::Success;
}

bool ApplicationManager::startApplication(const std::string &appName)
{

    setState(State::STARTING_NEW_APP);
    // search map for application's description structure with specified name
    auto app = appGet(appName);
    if (app == nullptr)
    {
        LOG_ERROR("Can't run: %s no such app", appName.c_str());
        return false;
    }

    if (app->getState() == app::Application::State::ACTIVE_BACKGROUND)
    {
        setState(State::WAITING_GET_FOCUS_CONFIRMATION);
        LOG_INFO("switching focus to application: [%s] window [%s]", appName.c_str(), app->switchWindow.c_str());
        app::Application::messageSwitchApplication(this, launchApplicationName, app->switchWindow, std::move(app->switchData));
    }
    else
    {
        setState(State::WAITING_NEW_APP_REGISTRATION);
        LOG_INFO("starting application: %s", appName.c_str());
        app->launcher->run(this);
    }

    return true;
}

bool ApplicationManager::handlePowerSavingModeInit() {

	LOG_INFO("Going to suspend mode");

	sys::SystemManager::SuspendService("ServiceGUI",this);
	sys::SystemManager::SuspendService("ServiceEink",this);

	sys::SystemManager::SuspendSystem(this);

	return true;
}

// tries to switch the application
bool ApplicationManager::handleSwitchApplication(APMSwitch *msg)
{

    // first check if there is application specified in the message
    auto app = appGet(msg->getName());
    if (!app)
    {
        LOG_ERROR("Cant switch to app: %s , doesn't exist", msg->getName().c_str());
        return false;
    }

    // check if specified application is not the application that is currently running
    // this is applicable to all applications except desktop
    if ((focusApplicationName == msg->getName()))
    {
        LOG_WARN("Trying to return currently active application");
        return false;
    }

    // store the name of the application to be executed and start closing previous application
    launchApplicationName = msg->getName();

    // store window and data if there is any
    app->switchData = std::move(msg->getData());
    app->switchWindow = msg->getWindow();
    setState(State::CLOSING_PREV_APP);

    // check if there was previous application
    if (!focusApplicationName.empty())
    {
        if (launchApplicationName == app::name_desktop)
        {
            appStack.clear();
        }
        else
        {
            appStack.push_back(focusApplicationName);
#ifdef DEBUG_APPLICATION_MANAGEMENT
            LOG_DEBUG("LaunchApp: %s", launchApplicationName.c_str());
#endif
        }
        /// if we want to disable closing previous app - then forbid killing it - it will be moved to background instead
        bool kill_prev = true;
        if (app->switchData != nullptr && app->switchData->disableAppClose)
        {
            kill_prev = false;
        }
        previousApplicationName = focusApplicationName;
        auto app = appGet(previousApplicationName);
        // if application's launcher defines that it can be closed send message with close signal
        if (kill_prev && (app->closeable()) && (app->blockClosing == false))
        {
            LOG_INFO("APMSwitch waiting for close confirmation from: %s", previousApplicationName.c_str());
            setState(State::WAITING_CLOSE_CONFIRMATION);
            app::Application::messageCloseApplication(this, previousApplicationName);
        }
        // if application is not closeable send lost focus message
        else
        {
            LOG_INFO("APMSwitch Waiting for lost focus from: %s", previousApplicationName.c_str());
            setState(State::WAITING_LOST_FOCUS_CONFIRMATION);
            app::Application::messageSwitchApplication(this, previousApplicationName, "", nullptr);
        }
    }
    // if there was no application to close or application can't be closed change internal state to
    // STARTING_NEW_APP and send execute lanuchers for that application
    else
    {
        startApplication(app->name());
    }

    return true;
}

//tries to switch the application
bool ApplicationManager::handleSwitchPrevApplication( APMSwitchPrevApp* msg ) {

	//if there is no previous application return false and do nothing
	if( previousApplicationName.empty() ) {
		return false;
	}

    previousApplicationName = appStack.back();
    appStack.pop_back();
    if (previousApplicationName == app::name_desktop && appStack.size())
    {
        std::string names = "";
        for (auto &el : appStack)
        {
            names += el + "<-";
        }
        LOG_ERROR("%s isn't top application! [%s]", app::name_desktop.c_str(), names.c_str());
    }

    //check if previous application is stored in the description vector
    auto app = appGet(previousApplicationName);
    if (!app)
    {
        // specified application was not found, exiting
        LOG_ERROR("Unable to find previous application: %s", previousApplicationName);
        return false;
    }

    //check if specified application is not the application that is currently running
	if( focusApplicationName == previousApplicationName ) {
		LOG_WARN("Trying to return currently active application");
		return false;
	}

#ifdef DEBUG_APPLICATION_MANAGEMENT
    LOG_DEBUG("Switch PrevApp: [%s](%s) -> [%s](%s)", focusApplicationName.c_str(), app::Application::stateStr(appGet(previousApplicationName)->getState()),
              previousApplicationName.c_str(), app::Application::stateStr(appGet(previousApplicationName)->getState()));
#endif
    //set name of the application to be executed and start closing previous application
	launchApplicationName = previousApplicationName;
	//store window and data if there is any
    app->switchData = std::move(msg->getData());
    app->switchWindow = "LastWindow";
    setState(State::CLOSING_PREV_APP);

    //check if there was previous application
	if( !focusApplicationName.empty() ) {
		previousApplicationName = focusApplicationName;
        auto app = appGet(previousApplicationName);

        //if application's launcher defines that it can be closed send message with close signal
        if (app->closeable())
        {
            LOG_INFO("Closing application: %s", previousApplicationName.c_str());
            setState(State::WAITING_CLOSE_CONFIRMATION);
            app::Application::messageCloseApplication(this, previousApplicationName);
        }
        // if application is not closeable send lost focus message
        else
        {
            setState(State::WAITING_LOST_FOCUS_CONFIRMATION);
            app::Application::messageSwitchApplication(this, previousApplicationName, "", nullptr);
        }
    }
    // if there was no application to close or application can't be closed change internal state to
    // STARTING_NEW_APP and send execute lanuchers for that application
    else
    {
        startApplication(app->name());
    }

    return true;
}

bool ApplicationManager::handleRegisterApplication( APMRegister* msg ) {
    auto app = appGet(msg->getSenderName());
    if (app == nullptr)
    {
        LOG_ERROR("can't register: %s no such app in `applicationsk`", msg->getSenderName().c_str());
        return false;
    }

    if( msg->getSenderName() == launchApplicationName ) {
		//application starts in background
		if( msg->getStartBackground()) {
            app->setState(app::Application::State::ACTIVE_BACKGROUND);
            setState(State::IDLE);
        }
		else {
            app->setState(app::Application::State::ACTIVATING);
            setState(State::WAITING_GET_FOCUS_CONFIRMATION);
            LOG_INFO("switchApplication %s %s", launchApplicationName.c_str(), app->switchData ? app->switchData->getDescription().c_str() : "");
            app::Application::messageSwitchApplication(this, launchApplicationName, app->switchWindow, std::move(app->switchData));
        }
	}
    else
    {
        app->setState(app::Application::State::ACTIVE_BACKGROUND);
    }
    return true;
}

bool ApplicationManager::handleLanguageChange( sapm::APMChangeLanguage* msg ) {

	//check if selected language is different than the one that is in the settings
	//if they are the same, return doing nothing
	SettingsLanguage requestedLanguage;
	switch( msg->getLanguage()) {
		case utils::Lang::En : requestedLanguage = SettingsLanguage::ENGLISH; break;
		case utils::Lang::Pl : requestedLanguage = SettingsLanguage::POLISH; break;
		case utils::Lang::De : requestedLanguage = SettingsLanguage::GERMAN; break;
		case utils::Lang::Sp : requestedLanguage = SettingsLanguage::SPANISH; break;
		default: requestedLanguage = SettingsLanguage::ENGLISH; break;
	};

	//if requested language is different than current update settings and i18 translations
	if( requestedLanguage != settings.language ) {
		settings = DBServiceAPI::SettingsGet(this);
		settings.language = requestedLanguage;
		DBServiceAPI::SettingsUpdate( this, settings );
		utils::localize.Switch( msg->getLanguage() );
	}
	else {
		LOG_WARN("Selected language is already set. Ignoring command.");
		return true;
	}

    // iterate over all applications in the background or foreground state and send them rebuild command
    for (auto &app : getApps())
    {
        if (app && app->launcher && app->launcher->handle &&
            (app->launcher->handle->getState() == app::Application::State::ACTIVE_BACKGROUND ||
             app->launcher->handle->getState() == app::Application::State::ACTIVE_FORGROUND))
        {
            app::Application::messageRebuildApplication(this, app->name());
        }
    }

    return true;
}

bool ApplicationManager::handleSwitchConfirmation(APMConfirmSwitch *msg)
{
    std::string app_name = getState() == State::WAITING_GET_FOCUS_CONFIRMATION ? msg->getSenderName() : focusApplicationName;
    ApplicationDescription *app = appGet(app_name);
    if (app == nullptr)
    {
        LOG_ERROR("Can't handle switch confirmation to: %s", app_name.c_str());
        return false;
    }

    /// move application with focus to front
    {
        auto app = appGet(msg->getSenderName());
        if (app && app->getState() == app::Application::State::ACTIVE_FORGROUND)
        {
            appMoveFront(app);
            debug_log_app_list();
            // notify event manager which application should receive keyboard messages
            EventManager::messageSetApplication(this, app->name());
        }
        else
        {
            LOG_DEBUG("Focus switch ingored cause: %d %s", app == nullptr, app == nullptr ? "" : app::Application::stateStr(app->getState()));
        }
    }
    // this is the case when application manager is waiting for newly started application to confim that it has
    // successfully gained focus.
    if (getState() == State::WAITING_GET_FOCUS_CONFIRMATION || getState() == State::IDLE)
    {
        LOG_INFO("APMConfirmSwitch focus confirmed by: [%s]", msg->getSenderName().c_str());
        focusApplicationName = launchApplicationName;
        launchApplicationName = "";

        app->blockClosing = false;
        app->setState(app::Application::State::ACTIVE_FORGROUND);
        setState(State::IDLE);
        return true;
    }
    // this is the case where application manager is waiting for non-closeable application
    // to confirm that app has lost focus.
    else if (getState() == State::WAITING_LOST_FOCUS_CONFIRMATION)
    {
        if (msg->getSenderName() == focusApplicationName)
        {
            LOG_INFO("APMConfirmSwitch Lost focus confirmed by: %s", msg->getSenderName().c_str());
            previousApplicationName = focusApplicationName;
            focusApplicationName = "";
            app->setState(app::Application::State::ACTIVE_BACKGROUND);
            app->switchWindow = "LastWindow";
            startApplication(launchApplicationName);
            return true;
        }
    }
    LOG_ERROR("APMConfirmSwitch %s : %s state %s", msg->getSenderName().c_str(), focusApplicationName.c_str(), stateStr(getState()));
    return false;
}

bool ApplicationManager::handleCloseConfirmation(APMConfirmClose *msg)
{
    auto app = appGet(msg->getSenderName());
    if (app == nullptr)
    {
        LOG_ERROR("can't handle: %s app: %s doesn't exist", __FUNCTION__, msg->getSenderName());
        return false;
    }

    // if application is running and it's not closeable set state to active background
    // otherwise it means that application is ready to be closed by using DestroyService api
    if (app->closeable())
    {
        // internally send close message to allow response message to be sended to application
        // that has confirmed close request.
        app->setState(app::Application::State::DEACTIVATED);
        auto msg = std::make_shared<sapm::APMDelayedClose>(this->GetName(), previousApplicationName);
        sys::Bus::SendUnicast(msg, "ApplicationManager", this);
    }
    else
    {
        app->setState(app::Application::State::ACTIVE_BACKGROUND);
    }

    return true;
}

//Static methods

bool ApplicationManager::messageSwitchApplication(sys::Service *sender, const std::string &applicationName, const std::string &windowName,
                                                  std::unique_ptr<gui::SwitchData> data)
{
    auto msg = std::make_shared<sapm::APMSwitch>(sender->GetName(), applicationName, windowName, std::move(data));
    return sys::Bus::SendUnicast(msg, "ApplicationManager", sender);
}

bool ApplicationManager::messageSwitchSpecialInput(sys::Service *sender, std::unique_ptr<gui::SwitchSpecialChar> data)
{
    auto val = data->requester;
    // forbid killing prev app, it could be done better - i.e. with state (not this disableAppClose parameter)
    data->disableAppClose = true;
    return (gui::SwitchSpecialChar::Type::Request == data->type)
               ? messageSwitchApplication(sender, app::special_input, app::char_select, std::move(data))
               : messageSwitchPreviousApplication(sender, std::make_unique<APMSwitchPrevApp>(data->requester, std::move(data)));
}

bool ApplicationManager::messageConfirmSwitch( sys::Service* sender) {

	auto msg = std::make_shared<sapm::APMConfirmSwitch>(sender->GetName() );
    sys::Bus::SendUnicast(msg, "ApplicationManager", sender);
    return true;
}
bool ApplicationManager::messageConfirmClose( sys::Service* sender) {

	auto msg = std::make_shared<sapm::APMConfirmClose>(sender->GetName() );
	sys::Bus::SendUnicast(msg, "ApplicationManager", sender);
	return true;
}
bool ApplicationManager::messageSwitchPreviousApplication(sys::Service *sender, std::unique_ptr<APMSwitchPrevApp> msg)
{

    std::shared_ptr<APMSwitchPrevApp> sendMsg;
    if (!msg)
    {
        sendMsg = std::make_shared<sapm::APMSwitchPrevApp>(sender->GetName());
    }
    else
    {
        sendMsg = std::move(msg);
    }
    sys::Bus::SendUnicast(sendMsg, "ApplicationManager", sender);
    return true;
}

bool ApplicationManager::messageRegisterApplication( sys::Service* sender, const bool& status, const bool& startBackground ) {
	auto msg = std::make_shared<sapm::APMRegister>(sender->GetName(), status, startBackground );
	sys::Bus::SendUnicast(msg, "ApplicationManager", sender);
	return true;
}

bool ApplicationManager::messageChangeLanguage( sys::Service* sender, utils::Lang language ) {
	auto msg = std::make_shared<sapm::APMChangeLanguage>( sender->GetName(), language );
	sys::Bus::SendUnicast(msg, "ApplicationManager", sender);
	return true;
}

bool ApplicationManager::messageCloseApplicationManager( sys::Service* sender ) {
	auto msg = std::make_shared<sapm::APMClose>( sender->GetName() );
	sys::Bus::SendUnicast(msg, "ApplicationManager", sender);
	return true;
}

bool ApplicationManager::messagePreventBlocking( sys::Service* sender ) {
	auto msg = std::make_shared<sapm::APMPreventBlocking>( sender->GetName() );
	sys::Bus::SendUnicast(msg, "ApplicationManager", sender);
	return true;
}

bool ApplicationManager::messageInitPowerSaveMode( sys::Service* sender ) {
	auto msg = std::make_shared<sapm::APMInitPowerSaveMode>( sender->GetName() );
	sys::Bus::SendUnicast(msg, "ApplicationManager", sender);
	return true;
}

} /* namespace sapm */
