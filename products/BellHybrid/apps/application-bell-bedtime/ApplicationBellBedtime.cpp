// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellBedtime.hpp"
#include "presenter/BellBedtimeWindowPresenter.hpp"
#include "windows/BellBedtimeWindow.hpp"
#include "models/BedtimeListItemProvider.hpp"
#include <common/models/BedtimeModel.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <common/SoundsRepository.hpp>

namespace app
{
    ApplicationBellBedtime::ApplicationBellBedtime(std::string name,
                                                   std::string parent,
                                                   StatusIndicators statusIndicators,
                                                   StartInBackground startInBackground,
                                                   std::uint32_t stackDepth)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, stackDepth)
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
        windowsFactory.attach(
            gui::name::window::main_window, [](ApplicationCommon *app, [[maybe_unused]] const std::string &name) {
                auto audioModel   = std::make_unique<AudioModel>(app);
                auto bedtimeModel = std::make_unique<bell_bedtime::BedtimeModel>(app, *audioModel);
                auto provider     = std::make_shared<bell_bedtime::BedtimeListItemProvider>(std::move(bedtimeModel));
                auto presenter    = std::make_unique<bell_bedtime::BellBedtimeWindowPresenter>(provider);
                return std::make_unique<gui::BellBedtimeWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(gui::window::bell_finished::defaultName,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellFinishedWindow>(app, name);
                              });

        attachPopups({gui::popup::ID::AlarmActivated,
                      gui::popup::ID::AlarmDeactivated,
                      gui::popup::ID::PowerOff,
                      gui::popup::ID::Reboot,
                      gui::popup::ID::ChargingNotification,
                      gui::popup::ID::ChargingDoneNotification});
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
