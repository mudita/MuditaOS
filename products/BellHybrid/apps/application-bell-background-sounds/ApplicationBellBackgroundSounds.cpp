// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellBackgroundSounds.hpp"
#include "models/BGSoundsRepository.hpp"
#include "presenter/BGSoundsMainWindowPresenter.hpp"
#include "presenter/BGSoundsTimerSelectPresenter.hpp"
#include "presenter/BGSoundsProgressPresenter.hpp"
#include "presenter/BGSoundsVolumePresenter.hpp"
#include "windows/BGSoundsMainWindow.hpp"
#include "windows/BGSoundsPausedWindow.hpp"
#include "windows/BGSoundsProgressWindow.hpp"
#include "windows/BGSoundsTimerSelectWindow.hpp"
#include "windows/BGSoundsVolumeWindow.hpp"
#include "widgets/BGSoundsPlayer.hpp"
#include <service-audio/AudioMessage.hpp>

#include <log/log.hpp>
namespace app
{
    ApplicationBellBackgroundSounds::ApplicationBellBackgroundSounds(std::string name,
                                                                     std::string parent,
                                                                     StatusIndicators statusIndicators,
                                                                     StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground),
          player{std::make_unique<bgSounds::BGSoundsPlayer>(this)}
    {
        bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);
        connect(typeid(AudioEOFNotification), [&](sys::Message *msg) -> sys::MessagePointer {
            auto notification = static_cast<AudioEOFNotification *>(msg);
            return player->handle(notification);
        });
    }
    ApplicationBellBackgroundSounds::~ApplicationBellBackgroundSounds() = default;

    sys::ReturnCodes ApplicationBellBackgroundSounds::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }
        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellBackgroundSounds::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [this](ApplicationCommon *app, const std::string &name) {
            auto tagsFetcher      = std::make_unique<bgSounds::BGSoundsTagsFetcher>(app);
            auto soundsRepository = std::make_shared<bgSounds::BGSoundsRepository>(std::move(tagsFetcher));
            auto presenter = std::make_unique<bgSounds::BGSoundsMainWindowPresenter>(std::move(soundsRepository));
            return std::make_unique<gui::BGSoundsMainWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(
            gui::window::name::bgSoundsTimerSelect, [this](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<bgSounds::BGSoundsTimerSelectPresenter>(settings.get());
                return std::make_unique<gui::BGSoundsTimerSelectWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(
            gui::window::name::bgSoundsProgress, [this](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<bgSounds::BGSoundsProgressPresenter>(settings.get(), *player);
                return std::make_unique<gui::BGSoundsProgressWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(gui::window::name::bgSoundsPaused, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::BGSoundsPausedWindow>(app);
        });

        windowsFactory.attach(gui::popup::window::volume_window, [](ApplicationCommon *app, const std::string &name) {
            auto presenter = std::make_unique<bgSounds::BGSoundsVolumePresenter>();
            return std::make_unique<gui::BGSoundsVolumeWindow>(app, std::move(presenter));
        });

        attachPopups({gui::popup::ID::AlarmActivated,
                      gui::popup::ID::AlarmDeactivated,
                      gui::popup::ID::PowerOff,
                      gui::popup::ID::Reboot});
    }

    sys::MessagePointer ApplicationBellBackgroundSounds::DataReceivedHandler(sys::DataMessage *msgl,
                                                                             sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (auto response = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
            response != nullptr && response->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        return handleAsyncResponse(resp);
    }
} // namespace app
