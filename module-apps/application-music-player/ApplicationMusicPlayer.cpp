// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <application-music-player/ApplicationMusicPlayer.hpp>
#include <windows/MusicPlayerAllSongsWindow.hpp>
#include <windows/MusicPlayerEmptyWindow.hpp>
#include <presenters/AudioOperations.hpp>
#include <presenters/SongsPresenter.hpp>
#include <models/SongsRepository.hpp>
#include <models/SongsModel.hpp>

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

    constexpr std::size_t applicationMusicPlayerStackSize = 4 * 1024;

    ApplicationMusicPlayer::ApplicationMusicPlayer(std::string name,
                                                   std::string parent,
                                                   sys::phone_modes::PhoneMode mode,
                                                   StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), mode, startInBackground, applicationMusicPlayerStackSize),
          priv{std::make_unique<music_player::internal::MusicPlayerPriv>()}
    {
        LOG_INFO("ApplicationMusicPlayer::create");

        auto songsRepository = std::make_unique<app::music_player::SongsRepository>(this);
        priv->songsModel     = std::make_unique<app::music_player::SongsModel>(std::move(songsRepository));
        auto audioOperations = std::make_unique<app::music_player::AudioOperations>(this);
        priv->songsPresenter =
            std::make_unique<app::music_player::SongsPresenter>(priv->songsModel, std::move(audioOperations));

        std::function<bool()> stateLockCallback = [this]() -> bool {
            auto isTrackPlaying = priv->songsModel->isSongPlaying();
            if (isTrackPlaying) {
                LOG_DEBUG("Preventing autolock while playing track.");
            }
            return isTrackPlaying;
        };
        lockPolicyHandler.setPreventsAutoLockByStateCallback(std::move(stateLockCallback));
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

        if (resp != nullptr) {
            if (auto command = callbackStorage->getCallback(resp); command->execute()) {
                refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
            }
            return sys::msgHandled();
        }

        return sys::msgNotHandled();
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
        return sys::ReturnCodes::Success;
    }

    void ApplicationMusicPlayer::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::all_songs_window, [&](Application *app, const std::string &name) {
            return std::make_unique<gui::MusicPlayerAllSongsWindow>(app, priv->songsPresenter);
        });
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::MusicPlayerEmptyWindow>(app);
        });

        attachPopups(
            {gui::popup::ID::Volume, gui::popup::ID::Tethering, gui::popup::ID::PhoneModes, gui::popup::ID::PhoneLock});
    }

    void ApplicationMusicPlayer::destroyUserInterface()
    {}
} /* namespace app */
