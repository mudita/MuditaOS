#pragma once

#include <Service/Service.hpp>
#include <Application.hpp>

#include <string>

namespace gui
{
    class Label;
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
        ParsedInfo,
        Updating,
        Finished,
    };

  public:
    Fota(app::Application *app, gui::Label *statusLabel);
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

    void getCurrentVersion();
    std::shared_ptr<app::Application> app_m;
    State currentState_m;
    std::string currentFirmwareVersion_m;
    std::string newFirmwareVersion_m;
    gui::Label *statusLable_m;
};
