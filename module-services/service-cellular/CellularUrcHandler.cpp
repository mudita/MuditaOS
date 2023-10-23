﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CellularUrcHandler.hpp"
#include "messages.hpp"

#include "service-cellular/src/ServiceCellularPriv.hpp"
#include "service-cellular/src/ussd/USSDHandler.hpp"

#include <service-antenna/AntennaServiceAPI.hpp>
#include <service-appmgr/ServiceApplicationManagerName.hpp>
#include <service-time/ServiceTimeName.hpp>

using namespace at::urc;

void CellularUrcHandler::Handle(Clip &urc)
{
    LOG_TRACE("Incoming call");
    std::string phoneNumber;
    if (urc.isValid()) {
        phoneNumber = urc.getNumber();
    }
    response = std::make_unique<cellular::CallerIdNotification>(phoneNumber);
    urc.setHandled(true);
}

void CellularUrcHandler::Handle(Ring &urc)
{
    response = std::make_unique<cellular::RingNotification>();
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
            auto notification = std::make_shared<cellular::CurrentOperatorNameNotification>("");
            cellularService.bus.sendMulticast(std::move(notification), sys::BusChannel::ServiceCellularNotifications);
        }

        const Store::Network network{status, accessTechnology};
        if (Store::GSM::get()->getNetwork() != network) {
            Store::GSM::get()->setNetwork(network);
            response = std::make_unique<cellular::NetworkStatusUpdateNotification>();
        }
        urc.setHandled(true);
    }
    else {
        LOG_WARN("Network status - not valid");
    }
}

void CellularUrcHandler::Handle(Cmti &urc)
{
    LOG_TRACE("Received new SMS notification");
    if (urc.isValid()) {
        response = std::make_unique<cellular::NewIncomingSMSNotification>(urc.getIndex());
        urc.setHandled(true);
    }
    else {
        LOG_ERROR("Could not parse Cmti message");
    }
}

void CellularUrcHandler::Handle(Cusd &urc)
{
    response     = std::nullopt;
    const auto message = urc.getMessage();
    if (!message) {
        LOG_WARN("CUSD with empty message!");

        CellularServiceAPI::USSDRequest(&cellularService, cellular::USSDMessage::RequestType::abortSession);
        auto msg = std::make_shared<cellular::MMIResultMessage>(mmiactions::MMIResultParams::MMIResult::Failed);
        cellularService.bus.sendUnicast(std::move(msg), service::name::appmgr);

        return;
    }

#if LOG_SENSITIVE_DATA_ENABLED
    LOG_INFO("USSD body: %s", message->c_str());
#else
    size_t constexpr truncatedLength = 16;
    LOG_INFO("USSD body (first %zu characters): %s", truncatedLength, message->substr(0, truncatedLength).c_str());
#endif

    urc.setHandled(true);

    if (urc.isActionNeeded() && cellularService.handleUSSDURC()) {
        auto msg = std::make_shared<cellular::MMIResponseMessage>(*message);
        cellularService.bus.sendUnicast(std::move(msg), service::name::appmgr);
        return;
    }

    CellularServiceAPI::USSDRequest(&cellularService, cellular::USSDMessage::RequestType::abortSession);
    auto msg = std::make_shared<cellular::MMIPushMessage>(*message);
    cellularService.bus.sendUnicast(std::move(msg), service::name::appmgr);
}

void CellularUrcHandler::Handle(Ctze &urc)
{
    if (!urc.isValid()) {
        return;
    }

    auto msg = std::make_shared<cellular::TimeNotificationMessage>(
        urc.getGMTTime(), urc.getTimeZoneOffset(), urc.getTimeZoneString());
    cellularService.bus.sendUnicast(std::move(msg), service::name::service_time);

    urc.setHandled(true);
}

void CellularUrcHandler::Handle(Qind &urc)
{
    if (urc.isCsq()) {
        // Received signal strength change
        auto rssi = urc.getRSSI();
        if (!rssi) {
            LOG_WARN("Invalid csq - ignore");
            AntennaServiceAPI::InvalidCSQNotification(&cellularService);
        }
        else {
            const SignalStrength signalStrength(*rssi);

            Store::GSM::get()->setSignalStrength(signalStrength.data);
            response = std::make_unique<cellular::SignalStrengthUpdateNotification>(urc.getUrcBody());
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
            response = std::make_unique<cellular::NetworkStatusUpdateNotification>();
        }
        urc.setHandled(true);
    }
    else if (urc.isFota()) {
        const auto httpSuccess = "0";
        if (urc.getFotaStage() == Qind::FotaStage::HTTPEND && urc.getFotaParameter() == httpSuccess) {
            LOG_DEBUG("Fota UPDATE, switching to AT mode");
            cellularService.cmux->setMode(CellularMux::Mode::AT);
        }
        urc.setHandled(true);
    }
    else if (urc.isSmsDone()) {
        response = std::make_unique<cellular::SmsDoneNotification>();
        urc.setHandled(true);
    }
}

void CellularUrcHandler::Handle(Cpin &urc)
{
    if (urc.isValid()) {
        auto state = urc.getState();
        if (!state) {
            LOG_WARN("Invalid cpin - ignore");
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
                    response = std::make_unique<cellular::DeactivateContextResponse>(at::Result::Code::OK, ctxid);
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
        response = std::make_unique<cellular::PowerDownDeregisteringNotification>();
        urc.setHandled(true);
    }
}

void CellularUrcHandler::Handle(UrcResponse &urc)
{
    const std::vector<UrcResponse::URCResponseType> typesToHandle = {
        UrcResponse::URCResponseType::NoCarrier,
        UrcResponse::URCResponseType::Busy,
        UrcResponse::URCResponseType::NoAnswer,
    };

    for (auto &t : typesToHandle) {
        if (t == urc.getURCResponseType()) {
            LOG_INFO("Call (aborted|missed) type: %s", magic_enum::enum_name(t).data());
            response = std::make_unique<cellular::CallAbortedNotification>();
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
