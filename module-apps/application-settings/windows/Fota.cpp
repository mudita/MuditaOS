#include "Fota.hpp"
#include "FotaWindow.hpp"

#include <service-cellular/api/CellularServiceAPI.hpp>
#include <service-internet/api/InternetServiceAPI.hpp>
#include <service-internet/messages/InternetMessage.hpp>

#include <gui/widgets/Label.hpp>

#include <numeric>

std::string Fota::urlPrefix   = "http://www.wicik.pl/mudita/fota/";
std::string Fota::versionFile = "fota.txt";

Fota::Fota(gui::FotaWindow *parent) : currentState_m(Disconnected), parent_m(parent)
{
    app_m = std::shared_ptr<app::Application>(parent_m->getApplication(),
                                              [](app::Application *) {}); /// with deleter that doesn't delete.
    parent_m->getApplication()->busChannels.push_back(sys::BusChannels::ServiceInternetNotifications);
    registerHandlers();
    sys::Bus::Add(std::static_pointer_cast<sys::Service>(app_m));

    getCurrentVersion();
    parent_m->statusLabel->setText(getStateString());
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
    case NeedUpdate:
        update();
        break;

    case Updating:
        break;
    case Finished:
        break;
    }
    parent_m->statusLabel->setText(getStateString());
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
        return std::string("Downloading info finished");
    case ParsingInfo:
        return std::string("Checking for firmwer update...");
    case NeedUpdate:
        return std::string("Update");
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
    InternetService::API::HTTPGET(app_m.get(), urlPrefix + versionFile);
}

void Fota::parseInfo()
{
    currentState_m = ParsingInfo;
    LOG_DEBUG("!");
    // do the parsing
    currentState_m = NeedUpdate;
}

void Fota::update()
{
    currentState_m = Updating;
    parent_m->downloadProgress->setVisible(true);
    InternetService::API::FotaStart(app_m.get(), urlPrefix + versionMap_m[currentFirmwareVersion_m].file);
}

void Fota::registerHandlers()
{
    LOG_DEBUG("Registrng handles");
    handleInternetNotification();
    handleHTTPResponse();
    handleFotaProgres();
    handleFotaFinished();
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
                               parent_m->statusLabel->setText(getStateString());
                               app_m->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                               break;
                           case InternetService::NotificationMessage::Type::Connected:
                               currentState_m = Connected;
                               parent_m->statusLabel->setText(getStateString());
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

void Fota::handleHTTPResponse()
{
    LOG_DEBUG("Handling http response");
    app_m->connect(
        InternetService::HTTPResponseMessage(), [&](sys::DataMessage *req, sys::ResponseMessage * /*response*/) {
            if (auto msg = dynamic_cast<InternetService::HTTPResponseMessage *>(req)) {
                LOG_DEBUG("HTTP Response to: %s", msg->url.c_str());
                LOG_DEBUG("HTPP AT Error   : %s", InternetService::toString(msg->httpError).c_str());
                LOG_DEBUG("response headers:\n\t%s",
                          std::accumulate(msg->responseHeaders.begin(), msg->responseHeaders.end(), std::string("\n\t"))
                              .c_str());
                LOG_DEBUG("response data   :\n%s", msg->body.c_str());
                const std::string contentLength("Content-Length: ");
                std::string data;
                for (auto &header : msg->responseHeaders) {
                    auto sizeBegin = header.find(contentLength);
                    if (sizeBegin != std::string::npos) {
                        unsigned long len = std::stoul(header.substr(sizeBegin + contentLength.size()));
                        data              = msg->body.substr(0, len);
                        break;
                    }
                }
                std::istringstream inputData(data);
                std::string line;
                while (std::getline(inputData, line, '\n')) {
                    std::istringstream lineInput(line);
                    std::string subItem;
                    std::vector<std::string> items;
                    items.reserve(3);
                    while (std::getline(lineInput, subItem, ':')) {
                        items.push_back(subItem);
                    }
                    if (items.size() == 3) {
                        versionMap_m[items[0]] = {items[1], items[2]};
                    }
                }

                LOG_DEBUG("parsed Data:");
                for (auto item : versionMap_m) {
                    LOG_DEBUG("%s %s->%s\n\t\t%s",
                              (item.first == currentFirmwareVersion_m ? "*" : " "),
                              item.first.c_str(),
                              item.second.newVersion.c_str(),
                              item.second.file.c_str());
                    if (item.first == currentFirmwareVersion_m) {
                        newFirmwareVersion_m = item.second.newVersion;
                        break;
                    }
                }

                currentState_m = DownloadedInfo;
                if (newFirmwareVersion_m.empty()) {
                    currentState_m = Finished;
                }
                else {
                    parent_m->newFirmwareLabelText->setVisible(true);
                    parent_m->newFirmwareLabel->setText(newFirmwareVersion_m);
                    parent_m->newFirmwareLabel->setVisible(true);
                    currentState_m = NeedUpdate;
                }
                parent_m->statusLabel->setText(getStateString());
                app_m->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
            }
            return std::make_shared<sys::ResponseMessage>();
        });
}

void Fota::handleFotaProgres()
{
    app_m->connect(InternetService::FOTAProgres(), [&](sys::DataMessage *req, sys::ResponseMessage * /*response*/) {
        if (auto msg = dynamic_cast<InternetService::FOTAProgres *>(req)) {
            parent_m->downloadProgress->setCurrentProgress(msg->progress);
            app_m->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return std::make_shared<sys::ResponseMessage>();
    });
}

void Fota::handleFotaFinished()
{
    app_m->connect(InternetService::FOTAFinished(),
                   [&](sys::DataMessage * /*req*/, sys::ResponseMessage * /*response*/) {
                       currentState_m = State::Finished;
                       parent_m->statusLabel->setText(getStateString());
                       /// TODO: Reboot!
                       return std::make_shared<sys::ResponseMessage>();
                   });
}

void Fota::getCurrentVersion()
{
    CellularServiceAPI::GetFirmwareVersion(parent_m->getApplication(), currentFirmwareVersion_m);
}
