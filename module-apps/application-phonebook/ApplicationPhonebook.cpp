#include "Application.hpp"

#include "ApplicationPhonebook.hpp"
#include "MessageType.hpp"
#include "windows/PhonebookBlockContact.hpp"
#include "windows/PhonebookContact.hpp"
#include "windows/PhonebookDeleteContact.hpp"
#include "windows/PhonebookErrors.hpp"
#include "windows/PhonebookMainWindow.hpp"
#include "windows/PhonebookNewContact.hpp"
#include "windows/PhonebookOptions.hpp"
#include "windows/PhonebookOptionsNamecard.hpp"
#include "windows/PhonebookSearch.hpp"
namespace app
{

ApplicationPhonebook::ApplicationPhonebook(std::string name, std::string parent, bool startBackgound)
    : Application(name, parent, startBackgound, 4096)
{
}

ApplicationPhonebook::~ApplicationPhonebook()
{
}

// Invoked upon receiving data message
sys::Message_t ApplicationPhonebook::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{

    auto retMsg = Application::DataReceivedHandler(msgl);
    // if message was handled by application's template there is no need to process further.
    if (reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success)
    {
        return retMsg;
    }

    // this variable defines whether message was processed.
    bool handled = false;

    // handle database response
    if (resp != nullptr)
    {
        handled = true;
        uint32_t msgType = resp->responseTo;
        switch (msgType)
        {
        case static_cast<uint32_t>(MessageType::DBContactGetLimitOffset): {
            if (currentWindow->onDatabaseMessage(resp))
                refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        break;
        }
    }

    if (handled)
        return std::make_shared<sys::ResponseMessage>();
    else
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

// Invoked during initialization
sys::ReturnCodes ApplicationPhonebook::InitHandler()
{

    auto ret = Application::InitHandler();
    if (ret != sys::ReturnCodes::Success)
        return ret;

    createUserInterface();

    setActiveWindow("MainWindow");

    return ret;
}

sys::ReturnCodes ApplicationPhonebook::DeinitHandler()
{
    return sys::ReturnCodes::Success;
}

void ApplicationPhonebook::createUserInterface()
{

    gui::AppWindow *window = nullptr;

    window = new PhonebookMainWindow(this);
    windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

    window = new PhonebookNewContact(this);
    windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

    window = new PhonebookContact(this);
    windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

    window = new PhonebookDeleteContact(this);
    windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

    window = new PhonebookBlockContact(this);
    windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

    window = new PhonebookSearch(this);
    windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

    window = new PhonebookNoResults(this);
    windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

    window = new PhonebookOptions(this);
    windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

    window = new PhonebookOptionsNamecard(this);
    windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));
}

void ApplicationPhonebook::destroyUserInterface()
{
}

} /* namespace app */
