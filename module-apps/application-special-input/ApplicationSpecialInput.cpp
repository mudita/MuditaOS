#include "ApplicationSpecialInput.hpp"
#include "messages/AppMessage.hpp"
#include "windows/SpecialInputMainWindow.hpp"

using namespace app;

ApplicationSpecialInput::ApplicationSpecialInput(std::string name, std::string parent, bool startBackgound)
    : Application(name, parent, startBackgound)
{
    windowsFactory.attach(app::char_select, [](Application *app, const std::string &name) {
        return std::make_unique<gui::SpecialInputMainWindow>(app);
    });

    setActiveWindow(app::char_select);
}

sys::Message_t ApplicationSpecialInput::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    auto retMsg = Application::DataReceivedHandler(msgl);
    if (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
        return retMsg;
    }

    return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

sys::ReturnCodes ApplicationSpecialInput::InitHandler()
{
    auto ret = Application::InitHandler();
    if (ret != sys::ReturnCodes::Success) {
        LOG_ERROR("Can't initialize ApplicationSpecialInput");
    }
    createUserInterface();
    setActiveWindow(gui::name::window::main_window);
    return ret;
}

void ApplicationSpecialInput::createUserInterface()
{
    windowsFactory.build(this, app::char_select);
}

void ApplicationSpecialInput::destroyUserInterface()
{}

sys::ReturnCodes ApplicationSpecialInput::DeinitHandler()
{
    return sys::ReturnCodes::Success;
}
