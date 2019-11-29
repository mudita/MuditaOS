#include "AppSpecialInput.hpp"
#include "windows/InputSelector.hpp"
#include "messages/AppMessage.hpp"

using namespace app;

AppSpecialInput::AppSpecialInput(std::string name, std::string parent, bool startBackgound) :
    Application(name, parent, startBackgound)
{
    auto window = new gui::UiCharSelector(this);
    windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));
    setActiveWindow(window->getName());
}

sys::Message_t AppSpecialInput::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    auto retMsg = Application::DataReceivedHandler(msgl);
    if (reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success)
    {
        return retMsg;
    }

    return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

// THIS IS IMPORTANT THIS SETS APPLICATION STATE... (not for example Application(...) constructor - this wouild be too easy
sys::ReturnCodes AppSpecialInput::InitHandler()
{
    LOG_ERROR("!!!!!");
    auto ret = Application::InitHandler();
    if(ret != sys::ReturnCodes::Success) {
        LOG_ERROR("");
    }
    setActiveWindow("MainWindow");
    return ret;
}
