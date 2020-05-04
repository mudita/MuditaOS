#include "Fota.hpp"
#include <service-cellular/api/CellularServiceAPI.hpp>
#include <service-internet/api/InternetServiceAPI.hpp>
#include <service-internet/messages/InternetMessage.hpp>

Fota::Fota(app::Application *app) : app_m(app), currentState_m(Disconnected)
{
    getCurrentVersion();

    //    app->connect(InternetRequestMessag(),[]{});
}

void Fota::next()
{
    switch (currentState_m) {
    case Disconnected:
        connect();
        break;

    case Connectiong:
        break;
    case Connected:
        downloadInfo();
        break;

    case DownloadingInfo:
        break;
    case DownloadedInfo:
        parseInfo();
        break;

    case ParsingInfo:
        break;
    case ParsedInfo:
        update();
        break;

    case Updating:
        break;
    case Finished:
        break;
    }
}

std::string Fota::getStateString()
{
    switch (currentState_m) {
    case Disconnected:
        return std::string("Disconnected");
    case Connectiong:
        return std::string("Connecting...");
    case Connected:
        return std::string("Connected");
    case DownloadingInfo:
        return std::string("Downloading firmwer info...");
    case DownloadedInfo:
        return std::string("Downloading finished");
    case ParsingInfo:
        return std::string("Checking for firmwer update...");
    case ParsedInfo:
        return std::string("Update needed or not");
    case Updating:
        return std::string("Updating...");
    case Finished:
        return std::string("Updating Finished");
    default:
        return std::string("Unsupported state:") + std::to_string(static_cast<int>(currentState_m));
    }
}

void Fota::connect()
{
    currentState_m = Connectiong;
    // do the connection
    InternetServiceAPI::Connect(app_m);
    currentState_m = Connected;
}

void Fota::downloadInfo()
{
    currentState_m = DownloadingInfo;
    // do the dwonload
    currentState_m = DownloadedInfo;
}

void Fota::parseInfo()
{
    currentState_m = ParsingInfo;
    // do the parsing
    currentState_m = ParsedInfo;
}

void Fota::update()
{
    currentState_m = Updating;
    // do the updating
    currentState_m = Finished;
}

void Fota::getCurrentVersion()
{
    CellularServiceAPI::GetFirmwareVersion(app_m, currentFirmwareVersion_m);
}
