// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <application-music-player/ApplicationMusicPlayer.hpp>
#include <windows/MusicPlayerAllSongsWindow.hpp>
#include <windows/MusicPlayerEmptyWindow.hpp>

#include <filesystem>
#include <log.hpp>
#include <i18n/i18n.hpp>
#include <purefs/filesystem_paths.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <time/ScopedTime.hpp>

namespace app
{
    constexpr std::size_t applicationMusicPlayerStackSize = 4 * 1024;

    ApplicationMusicPlayer::ApplicationMusicPlayer(std::string name,
                                                   std::string parent,
                                                   sys::phone_modes::PhoneMode mode,
                                                   StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), mode, startInBackground, applicationMusicPlayerStackSize)
    {
        LOG_INFO("ApplicationMusicPlayer::create");
        connect(typeid(AudioStartPlaybackResponse), [&](sys::Message *msg) {
            handlePlayResponse(msg);
            return sys::MessageNone{};
        });

        connect(typeid(AudioStopResponse), [&](sys::Message *msg) {
            handleStopResponse(msg);
            return sys::MessageNone{};
        });

        std::function<bool()> stateLockCallback = [this]() -> bool {
            if (isTrackPlaying) {
                LOG_DEBUG("Preventing autolock while playing track.");
            }
            return isTrackPlaying;
        };
        lockPolicyHandler.setPreventsAutoLockByStateCallback(std::move(stateLockCallback));
    }

    void ApplicationMusicPlayer::handlePlayResponse(sys::Message *msg)
    {
        auto startResponse = static_cast<AudioStartPlaybackResponse *>(msg);
        currentFileToken   = startResponse->token;
    }

    void ApplicationMusicPlayer::handleStopResponse(sys::Message *msg)
    {
        auto stopResponse = static_cast<AudioStopResponse *>(msg);

        if (stopResponse->token == currentFileToken.value()) {
            currentFileToken.reset();
            isTrackPlaying = false;
        }
    }

    sys::MessagePointer ApplicationMusicPlayer::DataReceivedHandler(sys::DataMessage *msgl,
                                                                    [[maybe_unused]] sys::ResponseMessage *resp)
    {
        return Application::DataReceivedHandler(msgl);
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
        windowsFactory.attach(gui::name::window::all_songs_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::MusicPlayerAllSongsWindow>(app);
        });
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::MusicPlayerEmptyWindow>(app);
        });

        attachPopups(
            {gui::popup::ID::Volume, gui::popup::ID::Tethering, gui::popup::ID::PhoneModes, gui::popup::ID::PhoneLock});
    }

    void ApplicationMusicPlayer::destroyUserInterface()
    {}

    std::vector<audio::Tags> ApplicationMusicPlayer::getMusicFilesList()
    {
        const auto musicFolder = purefs::dir::getUserDiskPath() / "music";
        std::vector<audio::Tags> musicFiles;
        LOG_INFO("Scanning music folder: %s", musicFolder.c_str());
        {
            auto time = utils::time::Scoped("fetch tags time");
            for (const auto &entry : std::filesystem::directory_iterator(musicFolder)) {
                if (!std::filesystem::is_directory(entry)) {
                    const auto &filePath = entry.path();
                    const auto fileTags  = getFileTags(filePath);
                    if (fileTags) {
                        musicFiles.push_back(*fileTags);
                        LOG_DEBUG(" - file %s found", entry.path().c_str());
                    }
                    else {
                        LOG_ERROR("Not an audio file %s", entry.path().c_str());
                    }
                }
            }
        }
        LOG_INFO("Total number of music files found: %u", static_cast<unsigned int>(musicFiles.size()));
        return musicFiles;
    }

    bool ApplicationMusicPlayer::play(const std::string &fileName)
    {
        AudioServiceAPI::PlaybackStart(this, audio::PlaybackType::Multimedia, fileName);
        isTrackPlaying = true;

        return true;
    }

    bool ApplicationMusicPlayer::pause()
    {
        if (currentFileToken) {
            isTrackPlaying = false;
            return AudioServiceAPI::Pause(this, currentFileToken.value());
        }
        return false;
    }

    bool ApplicationMusicPlayer::resume()
    {
        if (currentFileToken) {
            isTrackPlaying = true;
            return AudioServiceAPI::Resume(this, currentFileToken.value());
        }
        return false;
    }

    std::optional<audio::Tags> ApplicationMusicPlayer::getFileTags(const std::string &filePath)
    {
        return AudioServiceAPI::GetFileTags(this, filePath);
    }

    bool ApplicationMusicPlayer::stop()
    {
        if (currentFileToken) {
            isTrackPlaying = false;
            return AudioServiceAPI::Stop(this, currentFileToken.value());
        }
        return false;
    }

    void ApplicationMusicPlayer::togglePlaying()
    {
        if (isTrackPlaying) {
            pause();
        }
        else {
            resume();
        }
    }

} /* namespace app */
