// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CellularUrcHandler.hpp"

#include "service-cellular/CellularMessage.hpp"
#include "messages.hpp"
#include "service-cellular/CellularServiceAPI.hpp"

#include <service-antenna/AntennaServiceAPI.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-appmgr/Constants.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-time/Constants.hpp>

using namespace at::urc;


void CellularUrcHandler::Handle(Clip &urc)
{
    LOG_TRACE("incoming call...");
    std::string phoneNumber;
    if (urc.isValid()) {
        phoneNumber = urc.getNumber();
    }
    response = std::make_unique<CellularCallerIdNotification>(phoneNumber);
    urc.setHandled(true);
}

void CellularUrcHandler::Handle(Ring &urc)
{
    response = std::make_unique<CellularRingNotification>();
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

        if (status == Store::Network::Status::RegisteredHomeNetwork ||
            status == Store::Network::Status::RegisteredRoaming) {
            CellularServiceAPI::RequestCurrentOperatorName(&cellularService);
        }
        else {
            Store::GSM::get()->setNetworkOperatorName("");
            auto notification = std::make_shared<CellularCurrentOperatorNameNotification>("");
            cellularService.bus.sendMulticast(std::move(notification), sys::BusChannel::ServiceCellularNotifications);
        }

        Store::Network network{status, accessTechnology};
        if (Store::GSM::get()->getNetwork() != network) {
            Store::GSM::get()->setNetwork(network);
            response = std::make_unique<CellularNetworkStatusUpdateNotification>();
        }
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
        response = std::make_unique<CellularNewIncomingSMSNotification>(urc.getIndex());
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
            cellularService.bus.sendUnicast(msg, service::name::appmgr);
        }
    }
    else {
        CellularServiceAPI::USSDRequest(&cellularService, CellularUSSDMessage::RequestType::abortSesion);
        cellularService.ussdState = ussd::State::sesionAborted;
        cellularService.setUSSDTimer();
        auto msg = std::make_shared<CellularMMIPushMessage>(*message);
        cellularService.bus.sendUnicast(msg, service::name::appmgr);
    }

    urc.setHandled(true);
}

void CellularUrcHandler::Handle(Ctze &urc)
{
    if (!urc.isValid()) {
        return;
    }

    auto msg = std::make_shared<CellularTimeNotificationMessage>(
        urc.getGMTTime(), urc.getTimeZoneOffset(), urc.getTimeZoneString());
    cellularService.bus.sendUnicast(msg, service::name::service_time);

    urc.setHandled(true);
}

void CellularUrcHandler::Handle(Qind &urc)
{
    if (urc.isCsq()) {
        // Received signal strength change
        auto rssi = urc.getRSSI();
        if (!rssi) {
            LOG_INFO("Invalid csq - ignore");
            AntennaServiceAPI::InvalidCSQNotification(&cellularService);
        }
        else {
            SignalStrength signalStrength(*rssi);

            Store::GSM::get()->setSignalStrength(signalStrength.data);
            response = std::make_unique<CellularSignalStrengthUpdateNotification>(urc.getUrcBody());
        }
        auto ber = urc.getBER();
        if (ber.has_value()) {
            LOG_INFO("BER value: %d", ber.value());
        }
        urc.setHandled(true);
    }
    if (urc.isAct()) {
        auto nat     = urc.getAccessTechnology();
        auto network = Store::GSM::get()->getNetwork();
        if (network.accessTechnology != nat) {
            network.accessTechnology = nat;
            Store::GSM::get()->setNetwork(network);
            response = std::make_unique<CellularNetworkStatusUpdateNotification>();
        }
        urc.setHandled(true);
    }
    else if (urc.isFota()) {
        std::string httpSuccess = "0";
        if (urc.getFotaStage() == Qind::FotaStage::HTTPEND && urc.getFotaParameter() == httpSuccess) {
            LOG_DEBUG("Fota UPDATE, switching to AT mode");
            cellularService.cmux->setMode(CellularMux::Mode::AT);
        }
        urc.setHandled(true);
    }
    else if (urc.isSmsDone()) {
        response = std::make_unique<CellularSmsDoneNotification>();
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
            response = std::make_unique<cellular::internal::msg::HandleATSimStateChange>(*state);
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
        response = std::make_unique<CellularPowerDownDeregisteringNotification>();
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
            response = std::make_unique<CellularCallAbortedNotification>();
            urc.setHandled(true);
        }
    }
}

void CellularUrcHandler::Handle(QSimstat &urc)
{
    if (urc.isValid()) {
        auto inserted = urc.getInsertedStatus();
        if (inserted.has_value()) {
            response = std::make_unique<cellular::SimInsertedNotication>(inserted.value());
            urc.setHandled(true);
        }
    }
}
