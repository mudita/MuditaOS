#include "Fota.hpp"
#include <service-cellular/api/CellularServiceAPI.hpp>
#include <service-internet/api/InternetServiceAPI.hpp>
#include <service-internet/messages/InternetMessage.hpp>

#include <gui/widgets/Label.hpp>

Fota::Fota(app::Application *app, gui::Label *statusLabel)
    : app_m(app), currentState_m(Disconnected), statusLable_m(statusLabel)
{
    app_m->busChannels.push_back(sys::BusChannels::ServiceInternetNotifications);
    registerHandlers();
    sys::Bus::Add(std::static_pointer_cast<sys::Service>(app_m));

    getCurrentVersion();
    statusLabel->setText(getStateString());

    //    app->connect(InternetRequestMessag(),[]{});
}

Fota::~Fota()
{
    sys::Bus::Remove(std::static_pointer_cast<sys::Service>(app_m));
}

void Fota::next()
{
    switch (currentState_m) {
    case Disconnected:
        configureAPN();
        break;
    case Configuring:
        break;
    case Confiured:
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
    statusLable_m->setText(getStateString());
}

std::string Fota::getStateString()
{
    switch (currentState_m) {
    case Disconnected:
        return std::string("Disconnected");
    case Configuring:
        return std::string("Configuring APN");
    case Confiured:
        return std::string("Connect to APN");
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

void Fota::configureAPN()
{
    InternetService::APN::Config apnConfig;
    apnConfig.contextId  = 1;
    apnConfig.apn        = "plus";
    apnConfig.authMethod = InternetService::APN::AuthMethod::NONE;
    InternetService::API::Configure(app_m.get(), apnConfig);
    currentState_m = Configuring;
}

void Fota::connect()
{
    currentState_m = Connectiong;
    // do the connection
    InternetService::API::Connect(app_m.get());
}

void Fota::downloadInfo()
{
    currentState_m = DownloadingInfo;
    LOG_DEBUG("!");
    // do the dwonload
    currentState_m = DownloadedInfo;
}

void Fota::parseInfo()
{
    currentState_m = ParsingInfo;
    LOG_DEBUG("!");
    // do the parsing
    currentState_m = ParsedInfo;
}

void Fota::update()
{
    currentState_m = Updating;
    LOG_DEBUG("!");
    // do the updating
    currentState_m = Finished;
}

void Fota::registerHandlers()
{
    LOG_DEBUG("Registrng handles");
    handleInternetNotification();
}

void Fota::handleInternetNotification()
{
    LOG_DEBUG("handle IntrntNotificationMassage");
    app_m->connect(InternetService::NotificationMessage(),
                   [&](sys::DataMessage *req, sys::ResponseMessage * /*response*/) {
                       LOG_DEBUG("IntrntNotificationMassage");
                       if (auto msg = dynamic_cast<InternetService::NotificationMessage *>(req)) {
                           LOG_DEBUG("IntrntNotificationMassage: %s", msg->c_str());
                           switch (msg->type) {
                           case InternetService::NotificationMessage::Type::NotReady:
                               break;
                           case InternetService::NotificationMessage::Type::Configured:
                               currentState_m = Confiured;
                               statusLable_m->setText(getStateString());
                               app_m->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                               break;
                           case InternetService::NotificationMessage::Type::Connected:
                               currentState_m = Connected;
                               statusLable_m->setText(getStateString());
                               app_m->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                               break;
                           case InternetService::NotificationMessage::Type::Disconnected:
                               currentState_m = Disconnected;
                               break;
                           case InternetService::NotificationMessage::Type::RequestProcessed:
                               break;
                           case InternetService::NotificationMessage::Type::ServiceReady:
                               break;
                           }
                       }
                       return std::make_shared<sys::ResponseMessage>();
                   });
}

void Fota::getCurrentVersion()
{
    CellularServiceAPI::GetFirmwareVersion(app_m.get(), currentFirmwareVersion_m);
}
