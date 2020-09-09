#include "Fota.hpp"
#include "FotaWindow.hpp"

#include <service-cellular/api/CellularServiceAPI.hpp>
#include <service-fota/api/FotaServiceAPI.hpp>
#include <service-fota/messages/FotaMessages.hpp>

#include <gui/widgets/Label.hpp>

#include <numeric>

std::string Fota::urlPrefix   = "http://mudita-pure-firmware-test.s3.eu-central-1.amazonaws.com/";
std::string Fota::versionFile = "fota.txt";

Fota::Fota(gui::FotaWindow *parent) : currentState(State::Disconnected), parent(parent)
{
    app = std::shared_ptr<app::Application>(parent->getApplication(),
                                            [](app::Application *) {}); /// with deleter that doesn't delete.
    app->busChannels.push_back(sys::BusChannels::ServiceFotaNotifications);
    registerHandlers();
    sys::Bus::Add(std::static_pointer_cast<sys::Service>(app));

    getCurrentVersion();
    parent->statusLabel->setText(getStateString());
}

Fota::~Fota()
{
    sys::Bus::Remove(std::static_pointer_cast<sys::Service>(app));
}

void Fota::next()
{
    switch (currentState) {
    case State::Disconnected:
        configureAPN();
        break;
    case State::Configuring:
        break;
    case State::Configured:
        connect();
        break;

    case State::Connecting:
        break;
    case State::Connected:
        downloadInfo();
        break;

    case State::DownloadingInfo:
        break;
    case State::DownloadedInfo:
        break;

    case State::ParsingInfo:
        break;
    case State::NeedUpdate:
        update();
        break;

    case State::Updating:
        break;
    case State::Finished:
        reboot();
        break;
    case State::Reboot:
        break;
    }
    parent->statusLabel->setText(getStateString());
}

std::string Fota::getStateString()
{
    switch (currentState) {
    case State::Disconnected:
        return std::string("Disconnected");
    case State::Configuring:
        return std::string("Configuring APN");
    case State::Configured:
        return std::string("Connect to APN");
    case State::Connecting:
        return std::string("Connecting...");
    case State::Connected:
        return std::string("Connected");
    case State::DownloadingInfo:
        return std::string("Downloading firmware info...");
    case State::DownloadedInfo:
        return std::string("Downloading info finished");
    case State::ParsingInfo:
        return std::string("Checking for firmware update...");
    case State::NeedUpdate:
        return std::string("Update");
    case State::Updating:
        return std::string("Updating...");
    case State::Finished:
        return std::string("Updating Finished! Reboot!");
    case State::Reboot:
        return std::string("Rebooting...");
    }
    return std::string("Unsupported state:") + std::to_string(static_cast<int>(currentState));
}

void Fota::configureAPN()
{
    FotaService::APN::Config apnConfig;
    apnConfig.contextId  = 1;
    apnConfig.apn        = "plus";
    apnConfig.authMethod = FotaService::APN::AuthMethod::NONE;
    FotaService::API::Configure(app.get(), apnConfig);
    parent->getBottomBar()->setActive(gui::BottomBar::Side::CENTER, false);
    currentState = State::Configuring;
}

void Fota::connect()
{
    currentState = State::Connecting;
    // do the connection
    parent->getBottomBar()->setActive(gui::BottomBar::Side::CENTER, false);
    FotaService::API::Connect(app.get());
}

void Fota::downloadInfo()
{
    currentState = State::DownloadingInfo;
    parent->getBottomBar()->setActive(gui::BottomBar::Side::CENTER, false);
    FotaService::API::HTTPGET(app.get(), urlPrefix + versionFile);
}

void Fota::parseInfo(std::string data)
{
    currentState = State::ParsingInfo;
    updateStatus();
    std::istringstream inputData(data);
    std::string line;
    /* version mapping file, each line is divided this way:
     * <currentVersion>:<newVersion>:<fileName>
     */
    const unsigned char noItemsInLine(3);
    const unsigned char currentVersionPos(0);
    const unsigned char newVersionPos(1);
    const unsigned char fileNamePos(2);
    while (std::getline(inputData, line, '\n')) {
        std::istringstream lineInput(line);
        std::string subItem;
        std::vector<std::string> items;
        items.reserve(noItemsInLine);
        while (std::getline(lineInput, subItem, ':')) {
            items.push_back(subItem);
        }
        if (items.size() == noItemsInLine) {
            versionMap[items[currentVersionPos]] = {items[newVersionPos], items[fileNamePos]};
        }
    }

    LOG_DEBUG("parsed Data:");
    for (auto item : versionMap) {
        LOG_DEBUG("%s %s->%s\n\t\t%s",
                  (item.first == currentFirmwareVersion ? "*" : " "),
                  item.first.c_str(),
                  item.second.newVersion.c_str(),
                  item.second.file.c_str());
        if (item.first == currentFirmwareVersion) {
            newFirmwareVersion = item.second.newVersion;
            break;
        }
    }
    // do the parsing
}

void Fota::checkIfUpdateRequired()
{
    if (newFirmwareVersion.empty()) {
        parent->getBottomBar()->setActive(gui::BottomBar::Side::RIGHT, true);
        currentState = State::Finished;
    }
    else {
        parent->newFirmwareLabelText->setVisible(true);
        parent->newFirmwareLabel->setText(newFirmwareVersion);
        parent->newFirmwareLabel->setVisible(true);
        parent->getBottomBar()->setActive(gui::BottomBar::Side::CENTER, true);
        currentState = State::NeedUpdate;
    }
    updateStatus();
}

void Fota::update()
{
    currentState = State::Updating;
    parent->getBottomBar()->setActive(gui::BottomBar::Side::RIGHT, false);
    parent->getBottomBar()->setActive(gui::BottomBar::Side::CENTER, false);
    parent->downloadProgress->setVisible(true);
    FotaService::API::FotaStart(app.get(), urlPrefix + versionMap[currentFirmwareVersion].file);
}

void Fota::reboot()
{
    currentState = State::Reboot;
    parent->statusLabel->setText(getStateString());
    auto msg = std::make_shared<sys::SystemManagerCmd>(sys::Code::Reboot);
    sys::Bus::SendUnicast(std::move(msg), service::name::system_manager, app.get());
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
    app->connect(FotaService::NotificationMessage(), [&](sys::DataMessage *req, sys::ResponseMessage * /*response*/) {
        LOG_DEBUG("IntrntNotificationMassage");
        if (auto msg = dynamic_cast<FotaService::NotificationMessage *>(req)) {
            LOG_DEBUG("IntrntNotificationMassage: %s", msg->c_str());
            switch (msg->type) {
            case FotaService::NotificationMessage::Type::NotReady:
                break;
            case FotaService::NotificationMessage::Type::Configured:
                currentState = State::Configured;
                parent->statusLabel->setText(getStateString());
                parent->getBottomBar()->setActive(gui::BottomBar::Side::CENTER, true);
                app->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                break;
            case FotaService::NotificationMessage::Type::Connected:
                currentState = State::Connected;
                parent->statusLabel->setText(getStateString());
                parent->getBottomBar()->setActive(gui::BottomBar::Side::CENTER, true);
                app->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                break;
            case FotaService::NotificationMessage::Type::Disconnected:
                currentState = State::Disconnected;
                break;
            case FotaService::NotificationMessage::Type::RequestProcessed:
                break;
            case FotaService::NotificationMessage::Type::ServiceReady:
                break;
            }
        }
        return std::make_shared<sys::ResponseMessage>();
    });
}

void Fota::handleHTTPResponse()
{
    LOG_DEBUG("Handling http response");
    app->connect(FotaService::HTTPResponseMessage(), [&](sys::DataMessage *req, sys::ResponseMessage * /*response*/) {
        if (auto msg = dynamic_cast<FotaService::HTTPResponseMessage *>(req)) {
            LOG_DEBUG("HTTP Response to: %s", msg->url.c_str());
            LOG_DEBUG("HTPP AT Error   : %s", FotaService::toString(msg->httpError).c_str());
            LOG_DEBUG(
                "response headers:\n\t%s",
                std::accumulate(msg->responseHeaders.begin(), msg->responseHeaders.end(), std::string("\n\t")).c_str());
            LOG_DEBUG("response data   :\n%s", msg->body.c_str());
            const std::string contentLength("Content-Length: ");
            std::string data;
            for (auto &header : msg->responseHeaders) {
                auto sizeBegin = header.find(contentLength);
                if (sizeBegin != std::string::npos) {
                    try {
                        unsigned long len = std::stoul(header.substr(sizeBegin + contentLength.size()));
                        data              = msg->body.substr(0, len);
                    }
                    catch (const std::exception &e) {
                        LOG_ERROR("Fota::handleHTTPResponse exception %s", e.what());
                        return std::make_shared<sys::ResponseMessage>();
                    }
                    break;
                }
            }

            currentState = State::DownloadedInfo;
            parseInfo(std::move(data));
            checkIfUpdateRequired();
        }
        return std::make_shared<sys::ResponseMessage>();
    });
}

void Fota::handleFotaProgres()
{
    app->connect(FotaService::FOTAProgres(), [&](sys::DataMessage *req, sys::ResponseMessage * /*response*/) {
        if (auto msg = dynamic_cast<FotaService::FOTAProgres *>(req)) {
            parent->downloadProgress->setCurrentProgress(msg->progress);
            app->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return std::make_shared<sys::ResponseMessage>();
    });
}

void Fota::handleFotaFinished()
{
    app->connect(FotaService::FOTAFinished(), [&](sys::DataMessage * /*req*/, sys::ResponseMessage * /*response*/) {
        LOG_DEBUG("Done!");
        currentState = State::Finished;
        parent->statusLabel->setText(getStateString());
        parent->currentFirmwareLabel->setText(newFirmwareVersion);
        parent->newFirmwareLabelText->setVisible(false);
        parent->newFirmwareLabel->setVisible(false);
        parent->downloadProgress->setVisible(false);
        parent->getBottomBar()->setActive(gui::BottomBar::Side::CENTER, true);
        app->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        return std::make_shared<sys::ResponseMessage>();
    });
}

void Fota::getCurrentVersion()
{
    CellularServiceAPI::GetFirmwareVersion(parent->getApplication(), currentFirmwareVersion);
}

void Fota::updateStatus()
{
    parent->statusLabel->setText(getStateString());
    app->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
}
