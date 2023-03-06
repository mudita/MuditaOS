// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellFileTransfer.hpp"
#include "presenter/FileTransferPresenter.hpp"
#include "windows/FileTransferWindow.hpp"
#include <service-appmgr/Controller.hpp>
#include <service-desktop/DesktopMessages.hpp>

namespace app
{
    ApplicationBellFileTransfer::ApplicationBellFileTransfer(std::string name,
                                                             std::string parent,
                                                             StatusIndicators statusIndicators,
                                                             StartInBackground startInBackground,
                                                             uint32_t stackDepth)
        : Application(name, parent, statusIndicators, startInBackground, stackDepth)
    {
        bus.channels.push_back(sys::BusChannel::System);
        addActionReceiver(manager::actions::FileTransferFinished, [this](auto &&data) {
            LOG_ERROR("File Transfer Finished");
            app::manager::Controller::sendAction(this, app::manager::actions::Home, nullptr);
            return actionHandled();
        });
    }

    ApplicationBellFileTransfer::~ApplicationBellFileTransfer() = default;
    
    sys::ReturnCodes ApplicationBellFileTransfer::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellFileTransfer::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](ApplicationCommon *app, const std::string &) {
            auto presenter = std::make_unique<bell_file_transfer::FileTransferPresenter>();
            return std::make_unique<gui::FileTransferWindow>(app, std::move(presenter));
        });

        attachPopups({gui::popup::ID::AlarmActivated,
                      gui::popup::ID::AlarmDeactivated,
                      gui::popup::ID::PowerOff,
                      gui::popup::ID::Reboot});
    }

    sys::MessagePointer ApplicationBellFileTransfer::DataReceivedHandler(sys::DataMessage *msgl,
                                                                         sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }
        return handleAsyncResponse(resp);
    }
    sys::MessagePointer ApplicationBellFileTransfer::handleSwitchWindow(sys::Message *msgl)
    {
        stopIdleTimer();
        return ApplicationCommon::handleSwitchWindow(msgl);
    }
} // namespace app