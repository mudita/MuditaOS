// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMusicPlayer.hpp"
#include "windows/MusicPlayerAllSongsWindow.hpp"
#include "windows/MusicPlayerEmptyWindow.hpp"

#include <filesystem>
#include <log/log.hpp>
#include <i18n/i18n.hpp>
#include <purefs/filesystem_paths.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <time/ScopedTime.hpp>

namespace app
{
    ApplicationMusicPlayer::ApplicationMusicPlayer(std::string name,
                                                   std::string parent,
                                                   StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), startInBackground, 4096)
    {
        LOG_INFO("ApplicationMusicPlayer::create");
        connect(typeid(AudioStartPlaybackResponse), [&](sys::Message *msg) {
            handlePlayResponse(msg);
            return sys::MessageNone{};
        });
    }

    void ApplicationMusicPlayer::handlePlayResponse(sys::Message *msg)
    {
        auto startResponse = static_cast<AudioStartPlaybackResponse *>(msg);
        currentFileToken   = startResponse->token;
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

        setActiveWindow(gui::name::window::main_window);

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

        attachPopups({gui::popup::ID::Volume});
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
        return true;
    }

    bool ApplicationMusicPlayer::pause()
    {
        if (currentFileToken) {
            return AudioServiceAPI::Pause(this, currentFileToken.value());
        }
        return false;
    }

    bool ApplicationMusicPlayer::resume()
    {
        if (currentFileToken) {

            return AudioServiceAPI::Resume(this, currentFileToken.value());
        }
        return false;
    }

    std::optional<audio::Tags> ApplicationMusicPlayer::getFileTags(const std::string &filePath)
    {
        return AudioServiceAPI::GetFileTags(this, filePath);
    }

} /* namespace app */
