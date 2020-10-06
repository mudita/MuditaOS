#pragma once

#include <Service/Service.hpp>
#include <Application.hpp>

#include <string>

namespace gui
{
    class Label;
    class FotaWindow;
} // namespace gui

class Fota
{
    enum class State
    {
        Disconnected,
        Configuring,
        Configured,
        Connecting,
        Connected,
        DownloadingInfo,
        DownloadedInfo,
        ParsingInfo,
        NeedUpdate,
        Updating,
        Finished,
        Reboot,
    };

  public:
    struct VersionInfo
    {
        std::string newVersion;
        std::string file;
    };
    using VersionMap = std::unordered_map<std::string, VersionInfo>;
    Fota(gui::FotaWindow *parent);
    virtual ~Fota();
    void next();
    std::string getCurrentFirmwareVersion()
    {
        return currentFirmwareVersion;
    }
    std::string getStateString();
    State getState()
    {
        return currentState;
    };

  private:
    void configureAPN();
    void connect();
    void downloadInfo();
    void parseInfo(std::string data);
    void checkIfUpdateRequired();
    void update();
    void reboot();

    void registerHandlers();
    void handleInternetNotification();
    void handleHTTPResponse();
    void handleFotaProgres();
    void handleFotaFinished();

    void getCurrentVersion();
    void updateStatus();
    State currentState;

    gui::FotaWindow *parent = nullptr;
    std::shared_ptr<app::Application> app;
    std::string currentFirmwareVersion;
    std::string newFirmwareVersion;
    VersionMap versionMap;
    static std::string urlPrefix;
    static std::string versionFile;
};
