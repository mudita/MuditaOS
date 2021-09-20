// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * ApplicationAntenna.cpp
 *
 *  Created on: 18 lut 2020
 *      Author: kuba
 */

#include <application-antenna/ApplicationAntenna.hpp>
#include <windows/AntennaMainWindow.hpp>
#include <windows/ScanModesWindow.hpp>
#include <windows/AlgoParamsWindow.hpp>
#include <module-cellular/at/response.hpp>
#include <Timers/TimerFactory.hpp>
#include <service-cellular/CellularServiceAPI.hpp>
#include <ticks.hpp>

namespace app
{

    void ApplicationAntenna::timerHandler(void)
    {
        auto win        = getCurrentWindow();
        auto windowName = win->getName();
        if ((windowName == gui::name::window::main_window) || (windowName == gui::name::window::algo_window)) {
            if (!cellularRequestInProgress) {
                LOG_INFO("Get Network info");
                cellularRequestInProgress = true;
                CellularServiceAPI::GetNetworkInfo(this);
            }
        }
    }

    inline constexpr auto antennaApplicationStackSize = 1024 * 3;

    ApplicationAntenna::ApplicationAntenna(std::string name,
                                           std::string parent,
                                           sys::phone_modes::PhoneMode phoneMode,
                                           sys::bluetooth::BluetoothMode bluetoothMode,
                                           StartInBackground startInBackground)
        : Application(name, parent, phoneMode, bluetoothMode, startInBackground, antennaApplicationStackSize)
    {
        bus.channels.push_back(sys::BusChannel::AntennaNotifications);
        appTimer = sys::TimerFactory::createPeriodicTimer(
            this, "Antena", std::chrono::milliseconds{2000}, [this](sys::Timer &) { timerHandler(); });
        appTimer.start();
    }

    ApplicationAntenna::~ApplicationAntenna()
    {}

    // Invoked upon receiving data message
    sys::MessagePointer ApplicationAntenna::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if ((reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success)) {
            return retMsg;
        }

        // this variable defines whether message was processed.
        bool handled = false;
        if (auto msg = dynamic_cast<cellular::RawCommandRespAsync *>(msgl)) {
            switch (msg->type) {
            case CellularMessage::Type::OperatorsScanResult: {
                auto win = getCurrentWindow();
                if (win->getName() == gui::name::window::main_window) {
                    if (auto window = dynamic_cast<gui::AntennaMainWindow *>(win)) {
                        window->updateOperatorsScan(msg->data);
                    }
                }
                cellularRequestInProgress = false;
                handled                   = true;
            } break;
            case CellularMessage::Type::NetworkInfoResult: {
                handleNetworkParams(msg->data);
                auto win = getCurrentWindow();
                if (win->getName() == gui::name::window::main_window) {
                    if (auto window = dynamic_cast<gui::AntennaMainWindow *>(win)) {
                        window->updateDebugInfo(msg->data);
                    }
                }
                cellularRequestInProgress = false;
                handled                   = true;
            } break;
            case CellularMessage::Type::GetScanModeResult: {
                if (auto win = windowsStack.get(gui::name::window::scan_window)) {
                    if (auto window = dynamic_cast<gui::ScanModesWindow *>(win)) {
                        window->updateCurrentMode(msg->data[0]);
                    }
                }
                cellularRequestInProgress = false;
                handled                   = true;
            } break;
            default:
                break;
            }
        }

        if (msgl->messageType == MessageType::AntennaChanged) {

            CellularServiceAPI::GetAntenna(this, antenna);
            auto win = getCurrentWindow();

            if (win->getName() == gui::name::window::main_window) {
                auto window = dynamic_cast<gui::AntennaMainWindow *>(win);
                if (window != nullptr) {

                    window->updateAntennaButtons(antenna);
                }
            }

            bool refresh = false;
            if (win->getName() == gui::name::window::algo_window) {
                refresh = true;
            }
            auto window     = windowsStack.get(gui::name::window::algo_window);
            auto algoWindow = dynamic_cast<gui::AlgoParamsWindow *>(window);
            if (algoWindow != nullptr) {
                algoWindow->handleAntennaChanged(antenna, refresh);
            }
            handled = true;
        }
        if (msgl->messageType == MessageType::AntennaCSQChange) {
            csqUpdated = true;
            handled    = true;
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

        CellularServiceAPI::GetAntenna(this, antenna);

        return ret;
    }

    void ApplicationAntenna::createUserInterface()
    {
        using namespace gui::name::window;
        windowsFactory.attach(main_window, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::AntennaMainWindow>(app);
        });
        windowsFactory.attach(scan_window, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::ScanModesWindow>(app);
        });
        windowsFactory.attach(algo_window, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::AlgoParamsWindow>(app);
        });

        attachPopups(
            {gui::popup::ID::Volume, gui::popup::ID::Tethering, gui::popup::ID::PhoneModes, gui::popup::ID::PhoneLock});
    }

    void ApplicationAntenna::destroyUserInterface()
    {}

    void ApplicationAntenna::handleNetworkParams(std::vector<std::string> &data)
    {

        uint32_t creg;
        at::response::parseCREG(data[1], creg);

        uint32_t bandFreq = at::response::qnwinfo::parseNetworkFrequency(data[2]);

        if ((csqUpdated) || lastCreg != creg || lastFreq != bandFreq) {
            std::string param;
            at::response::parseCSQ(data[0], param);
            param += " | " + std::to_string(creg);
            param += " | " + std::to_string(bandFreq);

            csqUpdated = false;
            lastCreg   = creg;
            lastFreq   = bandFreq;

            bool refresh = false;
            auto win     = windowsStack.get(gui::name::window::algo_window);
            if (win == getCurrentWindow()) {
                refresh = true;
            }
            auto window = dynamic_cast<gui::AlgoParamsWindow *>(win);
            if (window != nullptr) {

                window->handleNewParams(param, refresh);
            }
        }
    }
} /* namespace app */
