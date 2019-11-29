#include "AppSpecialInput.hpp"
#include "windows/InputSelector.hpp"
#include "messages/AppMessage.hpp"

using namespace app;

AppSpecialInput::AppSpecialInput(std::string name, std::string parent, bool startBackgound) : Application(name, parent, startBackgound)
{
    auto window = new gui::UiCharSelector(this);
    windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));
    setActiveWindow(window->getName());
}

sys::Message_t AppSpecialInput::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    auto retMsg = Application::DataReceivedHandler(msgl);
    // if message was handled by application's template there is no need to process further.
    if (reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success)
    {
        return retMsg;
    }

    return sys::Message_t();
}
