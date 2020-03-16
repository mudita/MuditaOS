/*
 * ApplicationAntenna.cpp
 *
 *  Created on: 18 lut 2020
 *      Author: kuba
 */

#include "ApplicationAntenna.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"
#include "windows/AntennaMainWindow.hpp"
#include <ticks.hpp>
namespace app
{

    void ApplicationAntenna::timerHandler(void)
    {

        if (!scanInProgress) {
            LOG_INFO("Get Network info");
            CellularServiceAPI::GetNetworkInfo(this);
        }
    }

    ApplicationAntenna::ApplicationAntenna(std::string name, std::string parent, bool startBackgound)
        : Application(name, parent, startBackgound, 4096 * 2),
          appTimer(CreateAppTimer(2000, true, [=]() { timerHandler(); }))

    {
        appTimer.restart();
    }

    ApplicationAntenna::~ApplicationAntenna()
    {}

    // Invoked upon receiving data message
    sys::Message_t ApplicationAntenna::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if ((reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success)) {
            return retMsg;
        }

        // this variable defines whether message was processed.
        bool handled = false;
        if (msgl->messageType == MessageType::CellularOperatorsScanResult) {
            auto msg = dynamic_cast<cellular::RawCommandRespAsync *>(msgl);
            if (msg != nullptr) {
                auto win = getCurrentWindow();

                if (win->getName() == "MainWindow") {
                    auto window = dynamic_cast<gui::AntennaMainWindow *>(win);
                    if (window != nullptr) {

                        window->updateOperatorsScan(msg->data);
                    }
                }
                scanInProgress = false;
            }
            handled = true;
        }
        if (msgl->messageType == MessageType::CellularNetworkInfoResult) {
            auto msg = dynamic_cast<cellular::RawCommandRespAsync *>(msgl);
            if (msg != nullptr) {
                auto win = getCurrentWindow();

                if (win->getName() == "MainWindow") {
                    auto window = dynamic_cast<gui::AntennaMainWindow *>(win);
                    if (window != nullptr) {

                        window->updateDebugInfo(msg->data);
                    }
                }
            }
            handled = true;
        }

        if (handled)
            return std::make_shared<sys::ResponseMessage>();
        else
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationAntenna::InitHandler()
    {

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success)
            return ret;

        createUserInterface();

        setActiveWindow(gui::name::window::main_window);

        return ret;
    }

    sys::ReturnCodes ApplicationAntenna::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationAntenna::createUserInterface()
    {

        gui::AppWindow *win = new gui::AntennaMainWindow(this);
        win->setSize(480, 600);

        windows.insert(std::pair<std::string, gui::AppWindow *>(gui::name::window::main_window, win));
    }

    void ApplicationAntenna::destroyUserInterface()
    {}

} /* namespace app */
