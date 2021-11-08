// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellPowerNap.hpp"
#include "presenter/PowerNapMainWindowPresenter.hpp"
#include "presenter/PowerNapProgressPresenter.hpp"
#include "presenter/PowerNapSessionEndedPresenter.hpp"
#include "windows/PowerNapMainWindow.hpp"
#include "windows/PowerNapProgressWindow.hpp"
#include "windows/PowerNapSessionEndedWindow.hpp"
#include <common/models/TimeModel.hpp>
#include <AlarmSoundPaths.hpp>
#include <service-audio/AudioMessage.hpp>

namespace app
{
    ApplicationBellPowerNap::ApplicationBellPowerNap(std::string name,
                                                     std::string parent,
                                                     StatusIndicators statusIndicators,
                                                     StartInBackground startInBackground,
                                                     uint32_t stackDepth)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, stackDepth)
    {}

    ApplicationBellPowerNap::~ApplicationBellPowerNap() = default;
    sys::ReturnCodes ApplicationBellPowerNap::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }
        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellPowerNap::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [this](ApplicationCommon *app, const std::string &name) {
            auto presenter = std::make_unique<powernap::PowerNapMainWindowPresenter>(app, settings.get());
            return std::make_unique<gui::PowerNapMainWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(
            gui::window::name::powernapProgress, [this](ApplicationCommon *app, const std::string &name) {
                auto timeModel        = std::make_unique<app::TimeModel>();
                auto audioModel       = std::make_unique<AudioModel>(app);
                auto soundsRepository = std::make_unique<SoundsRepository>(alarms::paths::getAlarmDir());
                auto presenter        = std::make_unique<powernap::PowerNapProgressPresenter>(
                    app, settings.get(), std::move(soundsRepository), std::move(audioModel), std::move(timeModel));
                return std::make_unique<gui::PowerNapProgressWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(gui::window::name::powernapSessionEnded,
                              [](ApplicationCommon *app, const std::string &name) {
                                  auto presenter = std::make_unique<powernap::PowerNapSessionEndPresenter>(app);
                                  return std::make_unique<gui::PowerNapSessionEndedWindow>(app, std::move(presenter));
                              });

        attachPopups({gui::popup::ID::AlarmActivated,
                      gui::popup::ID::AlarmDeactivated,
                      gui::popup::ID::PowerOff,
                      gui::popup::ID::Reboot,
                      gui::popup::ID::BedtimeNotification});
    }

    sys::MessagePointer ApplicationBellPowerNap::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (auto response = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
            response != nullptr && response->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        return handleAsyncResponse(resp);
    }

    // Empty: do not start idleTimer on application run
    void ApplicationBellPowerNap::onStart()
    {}
} // namespace app
