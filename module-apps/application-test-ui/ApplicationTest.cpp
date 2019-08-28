#include "Application.hpp"

#include "MessageType.hpp"
#include "windows/TestMainWindow.hpp"

#include "ApplicationTest.hpp"

namespace app {

ApplicationTest::ApplicationTest(std::string name, bool startBackgound) :
    Application( name, startBackgound, 4096 ) {
}

ApplicationTest::~ApplicationTest() {}

// Invoked upon receiving data message
sys::Message_t ApplicationTest::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage* resp) {

    auto retMsg = Application::DataReceivedHandler(msgl);
    // if message was handled by application's template there is no need to process further.
    if (reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
        return retMsg;
    }

    // this variable defines whether message was processed.
    bool handled = true;

    if (handled)
        return std::make_shared<sys::ResponseMessage>();
    else
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

// Invoked during initialization
sys::ReturnCodes ApplicationTest::InitHandler() {

    auto ret = Application::InitHandler();
    if (ret != sys::ReturnCodes::Success)
        return ret;

    createUserInterface();

    setActiveWindow("MainWindow");

    return ret;
}

sys::ReturnCodes ApplicationTest::DeinitHandler() { return sys::ReturnCodes::Success; }

sys::ReturnCodes ApplicationTest::WakeUpHandler() { return sys::ReturnCodes::Success; }

sys::ReturnCodes ApplicationTest::SleepHandler() { return sys::ReturnCodes::Success; }

void ApplicationTest::createUserInterface() {

    gui::AppWindow *window = nullptr;

    window = new gui::TestMainWindow(this);
    windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));
}

void ApplicationTest::destroyUserInterface() {}

} /* namespace app */
