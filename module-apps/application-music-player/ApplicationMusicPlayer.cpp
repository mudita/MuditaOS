#include "ApplicationMusicPlayer.hpp"
#include "windows/MusicPlayerMainWindow.hpp"

#include <i18/i18.hpp>
#include <log/log.hpp>

namespace app
{

    ApplicationMusicPlayer::ApplicationMusicPlayer(std::string name, std::string parent, bool startBackgound)
        : Application(name, parent, startBackgound, 4096)
    {
        LOG_INFO("ApplicationMusicPlayer::create");
    }

    ApplicationMusicPlayer::~ApplicationMusicPlayer()
    {
        LOG_INFO("ApplicationMusicPlayer::destroy");
    }

    sys::Message_t ApplicationMusicPlayer::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        return Application::DataReceivedHandler(msgl);
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationMusicPlayer::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success)
            return ret;

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

        gui::AppWindow *window = nullptr;

        window = new gui::MusicPlayerMainWindow(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));
    }

    void ApplicationMusicPlayer::destroyUserInterface()
    {}

    std::vector<std::string> ApplicationMusicPlayer::getMusicFilesList() const
    {
        const char *musicFolder = USER_PATH("music");
        std::vector<std::string> musicFiles;
        LOG_INFO("Scanning music folder: %s", musicFolder);
        auto dirList = vfs.listdir(musicFolder, ".wav");

        for (vfs::DirectoryEntry ent : dirList) {
            if (ent.attributes != vfs::FileAttributes::Directory) {
                musicFiles.push_back(std::string(musicFolder) + "/" + ent.fileName);
                LOG_INFO(" - file %s found", ent.fileName.c_str());
            }
        }

        LOG_INFO("Total number of music files found: %u", static_cast<unsigned int>(musicFiles.size()));
        return musicFiles;
    }

} /* namespace app */
