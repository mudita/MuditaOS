// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellBedtime.hpp"
#include "presenter/BellBedtimeWindowPresenter.hpp"
#include "windows/BellBedtimeWindow.hpp"
#include "common/models/BedtimeModel.hpp"
#include "models/BedtimeListItemProvider.hpp"

#include <common/windows/BellFinishedCallbackWindow.hpp>
namespace app
{
    ApplicationBellBedtime::ApplicationBellBedtime(std::string name,
                                                   std::string parent,
                                                   StatusIndicators statusIndicators,
                                                   StartInBackground startInBackground)
        : Application(name, parent, statusIndicators, startInBackground)
    {}

    sys::ReturnCodes ApplicationBellBedtime::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellBedtime::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [this](ApplicationCommon *app, const std::string &) {
            auto bedtimeModel = std::make_unique<bell_bedtime::BedtimeModel>(app);
            auto provider     = std::make_shared<bell_bedtime::BedtimeListItemProvider>(std::move(bedtimeModel));
            auto presenter    = std::make_unique<bell_bedtime::BellBedtimeWindowPresenter>(provider);
            return std::make_unique<gui::BellBedtimeWindow>(app, std::move(presenter));
        });

        windowsFactory.attach(gui::BellFinishedCallbackWindow::defaultName,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellFinishedCallbackWindow>(app);
                              });

        attachPopups({gui::popup::ID::AlarmActivated,
                      gui::popup::ID::AlarmDeactivated,
                      gui::popup::ID::PowerOff,
                      gui::popup::ID::Reboot});
    }

    sys::MessagePointer ApplicationBellBedtime::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }
        return handleAsyncResponse(resp);
    }
} // namespace app
