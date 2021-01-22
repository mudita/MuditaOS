// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-cellular/CellularRequestHandler.hpp"
#include "service-cellular/RequestFactory.hpp"
#include "service-cellular/ServiceCellular.hpp"

#include "Service/Bus.hpp"
#include "Service/Message.hpp"
#include "Service/Timer.hpp"

#include "service-cellular/requests/CallRequest.hpp"
#include "service-cellular/requests/SupplementaryServicesRequest.hpp"
#include "service-cellular/requests/PasswordRegistrationRequest.hpp"
#include "service-cellular/requests/PinChangeRequest.hpp"
#include "service-cellular/requests/ImeiRequest.hpp"
#include "service-cellular/requests/UssdRequest.hpp"

#include <service-appmgr/model/ApplicationManager.hpp>
#include "service-cellular/service-cellular/requests/ClirRequest.hpp"
#include "service-cellular/service-cellular/requests/SupplementaryServicesRequest.hpp"

#include <module-cellular/at/response.hpp>

using namespace cellular;

void CellularRequestHandler::handle(ImeiRequest &request, at::Result &result)
{
    if (!request.checkModemResponse(result)) {
        request.setHandled(false);
        sendMmiResult(false);
        return;
    }
    request.setHandled(true);
    auto msg = std::make_shared<CellularMMIPushMessage>(result.response[0]);
    sys::Bus::SendUnicast(msg, app::manager::ApplicationManager::ServiceName, &cellular);
}

void CellularRequestHandler::handle(UssdRequest &request, at::Result &result)
{
    auto requestHandled = request.checkModemResponse(result);

    if (requestHandled) {
        cellular.ussdState = ussd::State::pullRequestSent;
        cellular.setUSSDTimer();
    }
    else {
        sendMmiResult(requestHandled);
    }
    request.setHandled(requestHandled);
}

void CellularRequestHandler::handle(CallRequest &request, at::Result &result)
{
    if (!request.checkModemResponse(result)) {
        request.setHandled(false);
        return;
    }
    // activate call state timer
    cellular.callStateTimer->reload();
    // Propagate "Ringing" notification into system
    sys::Bus::SendMulticast(
        std::make_shared<CellularCallMessage>(CellularCallMessage::Type::Ringing, request.getNumber()),
        sys::BusChannels::ServiceCellularNotifications,
        &cellular);
    request.setHandled(true);
}

void CellularRequestHandler::handle(SupplementaryServicesRequest &request, at::Result &result)
{
    auto requestHandled = request.checkModemResponse(result);
    request.setHandled(requestHandled);
    sendMmiResult(requestHandled);
}

void CellularRequestHandler::handle(PasswordRegistrationRequest &request, at::Result &result)
{
    auto requestHandled = request.checkModemResponse(result);
    request.setHandled(requestHandled);
    sendMmiResult(requestHandled);
}

void CellularRequestHandler::handle(PinChangeRequest &request, at::Result &result)
{
    auto requestHandled = request.checkModemResponse(result);
    request.setHandled(requestHandled);
    sendMmiResult(requestHandled);
}

void CellularRequestHandler::handle(ClirRequest &request, at::Result &result)
{
    using namespace app::manager::actions;
    using namespace cellular;
    using namespace at::response;
    auto requestHandled = request.checkModemResponse(result);

    std::shared_ptr<MMICustomResultParams> response = std::make_shared<MMIClirResult>();
    if (requestHandled) {
        auto procedureType = request.getProcedureType();
        if (procedureType == SupplementaryServicesRequest::ProcedureType::Activation) {
            response->addMessage(IMMICustomResultParams::MMIResultMessage::ClirEnabled);
        }
        else if (procedureType == SupplementaryServicesRequest::ProcedureType::Deactivation) {
            response->addMessage(IMMICustomResultParams::MMIResultMessage::ClirDisabled);
        }
        else if (procedureType == SupplementaryServicesRequest::ProcedureType::Interrogation) {
            auto clirParsed = clir::parseClir(result.response[0]);
            if (clirParsed != std::nullopt) {
                response->addMessage(clir::getStatus(clirParsed->serviceStatus));
                response->addMessage(clir::getState(clirParsed->serviceState));
            }
        }
        else {
            response->addMessage(IMMICustomResultParams::MMIResultMessage::CommonFailure);
        }
    }
    auto msg = std::make_shared<CellularMMIResultMessage>(MMIResultParams::MMIResult::Success, response);
    sys::Bus::SendUnicast(msg, app::manager::ApplicationManager::ServiceName, &cellular);
    request.setHandled(requestHandled);
}
void CellularRequestHandler::sendMmiResult(bool result)
{
    using namespace app::manager::actions;

    auto msg = std::make_shared<CellularMMIResultMessage>(result ? MMIResultParams::MMIResult::Success
                                                                 : MMIResultParams::MMIResult::Failed);
    sys::Bus::SendUnicast(msg, app::manager::ApplicationManager::ServiceName, &cellular);
}
