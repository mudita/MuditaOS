// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <application-music-player/ApplicationMusicPlayer.hpp>

#include "AudioNotificationsHandler.hpp"

#include <windows/MusicPlayerAllSongsWindow.hpp>
#include <apps-common/AudioOperations.hpp>
#include <presenters/SongsPresenter.hpp>
#include <models/SongsRepository.hpp>
#include <models/SongsModel.hpp>
#include <service-appmgr/Controller.hpp>

#include <filesystem>
#include <log.hpp>
#include <i18n/i18n.hpp>
#include <purefs/filesystem_paths.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <time/ScopedTime.hpp>

namespace app
{
    namespace music_player::internal
    {
        class MusicPlayerPriv
        {
          public:
            std::shared_ptr<app::music_player::SongsModelInterface> songsModel;
            std::shared_ptr<app::music_player::SongsContract::Presenter> songsPresenter;
        };
    } // namespace music_player::internal

    constexpr std::size_t applicationMusicPlayerStackSize = 5 * 1024;

    ApplicationMusicPlayer::ApplicationMusicPlayer(std::string name,
                                                   std::string parent,
                                                   StatusIndicators statusIndicators,
                                                   StartInBackground startInBackground)
        : Application(
              std::move(name), std::move(parent), statusIndicators, startInBackground, applicationMusicPlayerStackSize),
          priv{std::make_unique<music_player::internal::MusicPlayerPriv>()}
    {
        LOG_INFO("ApplicationMusicPlayer::create");

        bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);

        auto tagsFetcher     = std::make_unique<app::music_player::ServiceAudioTagsFetcher>(this);
        auto songsRepository = std::make_unique<app::music_player::SongsRepository>(std::move(tagsFetcher));
        priv->songsModel     = std::make_unique<app::music_player::SongsModel>(std::move(songsRepository));
        auto audioOperations = std::make_unique<app::AsyncAudioOperations>(this);
        priv->songsPresenter =
            std::make_unique<app::music_player::SongsPresenter>(priv->songsModel, std::move(audioOperations));

        // callback used when playing state is changed
        using SongState                                 = app::music_player::SongState;
        std::function<void(SongState)> autolockCallback = [this](SongState isPlaying) {
            if (isPlaying == SongState::Playing) {
                LOG_DEBUG("Preventing autolock while playing track.");
                lockPolicyHandler.set(locks::AutoLockPolicy::DetermineByAppState);
            }
            else {
                LOG_DEBUG("Autolock reenabled because track is no longer playing.");
                lockPolicyHandler.set(locks::AutoLockPolicy::DetermineByWindow);
                app::manager::Controller::preventBlockingDevice(this);
            }
        };
        priv->songsPresenter->setPlayingStateCallback(std::move(autolockCallback));

        // callback used when track is not played and we are in DetermineByAppState
        std::function<bool()> stateLockCallback = []() -> bool { return true; };
        lockPolicyHandler.setPreventsAutoLockByStateCallback(std::move(stateLockCallback));

        connect(typeid(AudioStopNotification), [&](sys::Message *msg) -> sys::MessagePointer {
            auto notification = static_cast<AudioStopNotification *>(msg);
            music_player::AudioNotificationsHandler audioNotificationHandler{priv->songsPresenter};
            return audioNotificationHandler.handleAudioStopNotification(notification);
        });
        connect(typeid(AudioEOFNotification), [&](sys::Message *msg) -> sys::MessagePointer {
            auto notification = static_cast<AudioStopNotification *>(msg);
            music_player::AudioNotificationsHandler audioNotificationHandler{priv->songsPresenter};
            return audioNotificationHandler.handleAudioEofNotification(notification);
        });
        connect(typeid(AudioPausedNotification), [&](sys::Message *msg) -> sys::MessagePointer {
            auto notification = static_cast<AudioPausedNotification *>(msg);
            music_player::AudioNotificationsHandler audioNotificationHandler{priv->songsPresenter};
            return audioNotificationHandler.handleAudioPausedNotification(notification);
        });
        connect(typeid(AudioResumedNotification), [&](sys::Message *msg) -> sys::MessagePointer {
            auto notification = static_cast<AudioResumedNotification *>(msg);
            music_player::AudioNotificationsHandler audioNotificationHandler{priv->songsPresenter};
            return audioNotificationHandler.handleAudioResumedNotification(notification);
        });
    }

    ApplicationMusicPlayer::~ApplicationMusicPlayer() = default;

    sys::MessagePointer ApplicationMusicPlayer::DataReceivedHandler(sys::DataMessage *msgl,
                                                                    [[maybe_unused]] sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (static_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        return handleAsyncResponse(resp);
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationMusicPlayer::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();
        return ret;
    }

    sys::ReturnCodes ApplicationMusicPlayer::DeinitHandler()
    {
        priv->songsPresenter->getMusicPlayerItemProvider()->clearData();
        priv->songsPresenter->stop();
        return Application::DeinitHandler();
    }

    void ApplicationMusicPlayer::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [&](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::MusicPlayerAllSongsWindow>(app, priv->songsPresenter);
        });

        attachPopups({gui::popup::ID::Volume,
                      gui::popup::ID::Tethering,
                      gui::popup::ID::PhoneModes,
                      gui::popup::ID::PhoneLock,
                      gui::popup::ID::Alarm});
    }

    void ApplicationMusicPlayer::destroyUserInterface()
    {}
} /* namespace app */
