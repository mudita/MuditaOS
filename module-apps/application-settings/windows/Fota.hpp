#pragma once

#include <Service/Service.hpp>
#include <Application.hpp>

#include <string>

namespace gui
{
    class Label;
    class FotaWindow;
}

class Fota
{
    enum State
    {
        Disconnected,
        Configuring,
        Confiured,
        Connectiong,
        Connected,
        DownloadingInfo,
        DownloadedInfo,
        ParsingInfo,
        NeedUpdate,
        Updating,
        Finished,
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
    std::string currentFirmwareVersion()
    {
        return currentFirmwareVersion_m;
    }
    std::string getStateString();
    State getState()
    {
        return currentState_m;
    };

  private:
    void configureAPN();
    void connect();
    void downloadInfo();
    void parseInfo();
    void update();

    void registerHandlers();
    void handleInternetNotification();
    void handleHTTPResponse();
    void handleFotaProgres();
    void handleFotaFinished();

    void getCurrentVersion();
    State currentState_m;

    gui::FotaWindow *parent_m;
    std::shared_ptr<app::Application> app_m;
    std::string currentFirmwareVersion_m;
    std::string newFirmwareVersion_m;
    VersionMap versionMap_m;
    static std::string urlPrefix;
    static std::string versionFile;
};
