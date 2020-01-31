/*
 * @file ApplicationCallLog.cpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 19.09.2019
 * @brief Application Call Log
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "ApplicationCallLog.hpp"
#include "MessageType.hpp"
#include "data/CallLogInternals.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "service-db/messages/DBMessage.hpp"
#include "windows/CallLogCallDeleteWindow.hpp"
#include "windows/CallLogDetailsWindow.hpp"
#include "windows/CallLogMainWindow.hpp"
#include "windows/CallLogOptionsWindow.hpp"
#include <Dialog.hpp>
#include <i18/i18.hpp>
#include <log/log.hpp>

using namespace calllog;

namespace app {

ApplicationCallLog::ApplicationCallLog(std::string name, std::string parent, bool startBackgound) :
	Application( name, parent, startBackgound, 4096 ) {
}

ApplicationCallLog::~ApplicationCallLog() {
	LOG_INFO("ApplicationCallLog::destroy");
}

// Invoked upon receiving data message
sys::Message_t ApplicationCallLog::DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) {

	auto retMsg = Application::DataReceivedHandler(msgl);
	//if message was handled by application's template there is no need to process further.
	if( (reinterpret_cast<sys::ResponseMessage*>( retMsg.get() )->retCode == sys::ReturnCodes::Success ) ){
		return retMsg;
	}

	auto handled = false;

	//handle database response
	if( resp != nullptr ) {
		handled = true;
		uint32_t msgType = resp->responseTo;
		switch( msgType ) {
			case static_cast<uint32_t>(MessageType::DBCalllogGetLimitOffset): {
                if (getCurrentWindow()->onDatabaseMessage(resp))
                {
                    refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                }
                break;
            }
		}
	}

	if( handled ) {
		return std::make_shared<sys::ResponseMessage>();
	}
	else {
		return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
	}
}

// Invoked during initialization
sys::ReturnCodes ApplicationCallLog::InitHandler() {
	auto ret = Application::InitHandler();
	if(ret != sys::ReturnCodes::Success) return ret;

	createUserInterface();

	setActiveWindow(calllog::settings::MainWindowStr);

	return ret;
}

sys::ReturnCodes ApplicationCallLog::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

void ApplicationCallLog::createUserInterface() {

	gui::AppWindow* window = nullptr;

	window = new gui::CallLogMainWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));

	window = new gui::CallLogDetailsWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));

	window = new gui::CallLogOptionsWindow(this);
	windows.insert(std::pair<std::string,gui::AppWindow*>(window->getName(), window));

    window = new gui::Dialog(this, calllog::settings::CallDeleteWindowStr,
                             {
                                 .title = "",
                                 .icon = "phonebook_contact_delete_trashcan",
                                 .text = utils::localize.get("app_calllog_delete_call_confirmation"),
                                 .action = []() -> bool {
                                     LOG_INFO("!");
                                     return true;
                                 },
                             });
    windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));
}

void ApplicationCallLog::destroyUserInterface() {
}

bool ApplicationCallLog::removeCalllogEntry(const CalllogRecord &record)
{
    LOG_DEBUG("Removing CalllogRecord: %d", record.id);
    auto dialog = dynamic_cast<gui::Dialog *>(windows[calllog::settings::CallDeleteWindowStr]);
    if (dialog != nullptr)
    {
        auto meta = dialog->meta;
        meta.action = [=]() -> bool {
            if (DBServiceAPI::CalllogRemove(this, record.id) == false)
            {
                LOG_ERROR("CalllogRemove id=%u failed", record.id);
                return false;
            }
            this->switchWindow(calllog::settings::MainWindowStr);
            return true;
        };
        meta.title = record.name;
        dialog->update(meta);
        return switchWindow(calllog::settings::CallDeleteWindowStr, nullptr);
    }
    else
    {
        LOG_ERROR("Dialog bad type!");
        return false;
    }
}

} /* namespace app */
