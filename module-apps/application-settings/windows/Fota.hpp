#pragma once

#include <Service/Service.hpp>
#include <Application.hpp>

#include <string>

class Fota
{
    enum State
    {
        Disconnected,
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
    Fota(app::Application *app);
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
    void connect();
    void downloadInfo();
    void parseInfo();
    void update();

    void getCurrentVersion();
    app::Application *app_m;
    State currentState_m;
    std::string currentFirmwareVersion_m;
    std::string newFirmwareVersion_m;
};
