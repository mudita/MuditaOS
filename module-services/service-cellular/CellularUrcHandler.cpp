// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CellularUrcHandler.hpp"

#include "service-cellular/CellularMessage.hpp"
#include "service-cellular/CellularServiceAPI.hpp"

#include <module-sys/Service/Bus.hpp>
#include <service-antenna/AntennaServiceAPI.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-appmgr/Controller.hpp>

// this static function will be replaced by Settings API
static bool isSettingsAutomaticTimeSyncEnabled()
{
    return true;
}

void CellularUrcHandler::Handle(Clip &urc)
{
    LOG_TRACE("incoming call...");
    std::string phoneNumber;
    if (urc.isValid()) {
        phoneNumber = urc.getNumber();
    }
    // continue without number
    response = std::make_unique<CellularCallMessage>(CellularCallMessage::Type::IncomingCall, phoneNumber);
    urc.setHandled(true);
}

void CellularUrcHandler::Handle(Creg &urc)
{
    if (urc.isValid()) {
        auto accessTechnology = urc.getAccessTechnology();
        auto status           = urc.getStatus();

        LOG_INFO("Network status - %s, access technology %s",
                 utils::enumToString(status).c_str(),
                 utils::enumToString(accessTechnology).c_str());

        Store::Network network{status, accessTechnology};

        Store::GSM::get()->setNetwork(network);
        response =
            std::make_unique<CellularNotificationMessage>(CellularNotificationMessage::Type::NetworkStatusUpdate);
        urc.setHandled(true);
    }
    else {
        LOG_WARN("Network status - not valid");
    }
}

void CellularUrcHandler::Handle(Cmti &urc)
{
    LOG_TRACE("received new SMS notification");
    if (urc.isValid()) {
        response = std::make_unique<CellularNotificationMessage>(CellularNotificationMessage::Type::NewIncomingSMS,
                                                                 urc.getIndex());
        urc.setHandled(true);
    }
    else {
        LOG_ERROR("Could not parse Cmti message");
    }
}

void CellularUrcHandler::Handle(Cusd &urc)
{
    response     = std::nullopt;
    auto message = urc.getMessage();
    if (!message) {
        return;
    }

    if (urc.isActionNeeded()) {
        if (cellularService.ussdState == ussd::State::pullRequestSent) {
            cellularService.ussdState = ussd::State::pullResponseReceived;
            cellularService.setUSSDTimer();
            auto msg = std::make_shared<CellularMMIResponseMessage>(*message);
            sys::Bus::SendUnicast(msg, app::manager::ApplicationManager::ServiceName, &cellularService);
        }
    }
    else {
        CellularServiceAPI::USSDRequest(&cellularService, CellularUSSDMessage::RequestType::abortSesion);
        cellularService.ussdState = ussd::State::sesionAborted;
        cellularService.setUSSDTimer();
        auto msg = std::make_shared<CellularMMIPushMessage>(*message);
        sys::Bus::SendUnicast(msg, app::manager::ApplicationManager::ServiceName, &cellularService);
    }

    urc.setHandled(true);
}

void CellularUrcHandler::Handle(Ctze &urc)
{
    if (!urc.isValid()) {
        return;
    }

    if (isSettingsAutomaticTimeSyncEnabled()) {
        auto msg = std::make_shared<CellularTimeNotificationMessage>(
            urc.getGMTTime(), urc.getTimeZoneOffset(), urc.getTimeZoneString());
        sys::Bus::SendUnicast(msg, service::name::evt_manager, &cellularService);
    }
    else {
        LOG_DEBUG("Timezone sync disabled.");
    }
    urc.setHandled(true);
}

void CellularUrcHandler::Handle(Qind &urc)
{
    if (urc.isCsq()) {
        // Received signal strength change
        AntennaServiceAPI::CSQChange(&cellularService);
        auto rssi = urc.getRSSI();
        if (!rssi) {
            LOG_INFO("Invalid csq - ignore");
        }
        else {
            SignalStrength signalStrength(*rssi);

            Store::GSM::get()->setSignalStrength(signalStrength.data);
            response = std::make_unique<CellularNotificationMessage>(
                CellularNotificationMessage::Type::SignalStrengthUpdate, urc.getUrcBody());
        }
        urc.setHandled(true);
    }
    else if (urc.isFota()) {
        std::string httpSuccess = "0";
        if (urc.getFotaStage() == Qind::FotaStage::HTTPEND && urc.getFotaParameter() == httpSuccess) {
            LOG_DEBUG("Fota UPDATE, switching to AT mode");
            cellularService.cmux->setMode(TS0710::Mode::AT);
            urc.setHandled(true);
        }
    }
    else if (urc.isSmsDone()) {
        response = std::make_unique<CellularNotificationMessage>(CellularNotificationMessage::Type::SMSDone);

        urc.setHandled(true);
    }
}

void CellularUrcHandler::Handle(Cpin &urc)
{
    if (urc.isValid()) {
        auto state = urc.getState();
        if (!state) {
            LOG_INFO("Invalid cpin - ignore");
        }
        else {
            response = std::make_unique<CellularSimStateMessage>(*state, *urc.getMessage());
            urc.setHandled(true);
        }
    }
}

void CellularUrcHandler::Handle(Qiurc &urc)
{
    auto urcType = urc.getType();
    if (urc.isValid() && urcType) {
        switch (*urcType) {
        case Qiurc::QIUrcMessages::DeactivateContext:
            if (auto urcFirstParam = urc.getFirstParam(); urcFirstParam) {
                int ctxid = 0;
                if (utils::toNumeric(*urcFirstParam, ctxid)) {
                    response = std::make_unique<CellularDeactivateContextResponse>(at::Result::Code::OK, ctxid);
                    urc.setHandled(true);
                }
            }
            break;
        }
    }
}

void CellularUrcHandler::Handle(PoweredDown &urc)
{
    if (urc.isValid()) {
        response =
            std::make_unique<CellularNotificationMessage>(CellularNotificationMessage::Type::PowerDownDeregistering);
        urc.setHandled(true);
    }
}

void CellularUrcHandler::Handle(UrcResponse &urc)
{
    std::vector<UrcResponse::URCResponseType> typesToHandle = {
        UrcResponse::URCResponseType::NoCarrier,
        UrcResponse::URCResponseType::Busy,
        UrcResponse::URCResponseType::NoAnswer,
    };

    for (auto &t : typesToHandle) {
        if (t == urc.getURCResponseType()) {
            LOG_TRACE("call aborted");
            response = std::make_unique<CellularNotificationMessage>(CellularNotificationMessage::Type::CallAborted);
            urc.setHandled(true);
        }
    }
}
