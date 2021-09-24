// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-cellular/CellularRequestHandler.hpp"
#include "service-cellular/RequestFactory.hpp"
#include "service-cellular/ServiceCellular.hpp"

#include <Service/Message.hpp>
#include <Timers/TimerHandle.hpp>
#include <Timers/TimerFactory.hpp>

#include "service-cellular/requests/CallRequest.hpp"
#include "service-cellular/requests/SupplementaryServicesRequest.hpp"
#include "service-cellular/requests/PasswordRegistrationRequest.hpp"
#include "service-cellular/requests/PinChangeRequest.hpp"
#include "service-cellular/requests/ImeiRequest.hpp"
#include "service-cellular/requests/UssdRequest.hpp"
#include "service-cellular/requests/CallForwardingRequest.hpp"
#include "service-cellular/requests/CallBarringRequest.hpp"
#include "service-cellular/requests/ClirRequest.hpp"
#include "service-cellular/requests/ClipRequest.hpp"
#include "service-cellular/requests/CallWaitingRequest.hpp"
#include "service-cellular/requests/RejectRequest.hpp"

#include <service-appmgr/Constants.hpp>

#include <module-cellular/at/response.hpp>

void CellularRequestHandler::handle(cellular::ImeiRequest &request, at::Result &result)
{
    using namespace app::manager::actions;
    using namespace at::response;
    auto requestHandled = request.checkModemResponse(result);

    std::shared_ptr<MMICustomResultParams> response;
    if (requestHandled) {
        response = std::make_shared<MMIImeiResult>(result.response[0]);
    }
    else {
        response = std::make_shared<MMIImeiResult>(MMIImeiResult());
        response->addMessage(IMMICustomResultParams::MMIResultMessage::CommonFailure);
    }

    auto msg = std::make_shared<CellularMMIResultMessage>(MMIResultParams::MMIResult::Success, response);
    cellular.bus.sendUnicast(msg, service::name::appmgr);
}

void CellularRequestHandler::handle(cellular::UssdRequest &request, at::Result &result)
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

void CellularRequestHandler::handle(cellular::CallRequest &request, at::Result &result)
{
    if (!request.checkModemResponse(result)) {
        request.setHandled(false);
        return;
    }
    // activate call state timer
    cellular.callStateTimer.start();
    // Propagate "Ringing" notification into system
    cellular.bus.sendMulticast(std::make_shared<CellularRingingMessage>(request.getNumber()),
                               sys::BusChannel::ServiceCellularNotifications);
    request.setHandled(true);
}

void CellularRequestHandler::handle(cellular::RejectRequest &request, at::Result &result)
{
    if (request.getRejectReason() == cellular::RejectRequest::RejectReason::NoSim) {
        auto message = std::make_shared<CellularNoSimNotification>(request.getNumber());
        cellular.bus.sendUnicast(message, service::name::appmgr);
    }
    else if (request.getRejectReason() == cellular::RejectRequest::RejectReason::NotAnEmergencyNumber) {
        auto message = std::make_shared<CellularNotAnEmergencyNotification>(request.getNumber());
        cellular.bus.sendUnicast(message, service::name::appmgr);
    }
    request.setHandled(true);
}

void CellularRequestHandler::handle(cellular::SupplementaryServicesRequest &request, at::Result &result)
{
    auto requestHandled = request.checkModemResponse(result);
    request.setHandled(requestHandled);
    sendMmiResult(requestHandled);
}

void CellularRequestHandler::handle(cellular::PasswordRegistrationRequest &request, at::Result &result)
{
    auto requestHandled = request.checkModemResponse(result);
    request.setHandled(requestHandled);
    sendMmiResult(requestHandled);
}

void CellularRequestHandler::handle(cellular::PinChangeRequest &request, at::Result &result)
{
    auto requestHandled = request.checkModemResponse(result);
    request.setHandled(requestHandled);
    sendMmiResult(requestHandled);
}

void CellularRequestHandler::handle(cellular::ClirRequest &request, at::Result &result)
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
            auto clirParsed = clir::parse(result.response[0]);
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
    cellular.bus.sendUnicast(msg, ::service::name::appmgr);
    request.setHandled(requestHandled);
}

void CellularRequestHandler::handle(cellular::CallForwardingRequest &request, at::Result &result)
{
    using namespace app::manager::actions;
    using namespace cellular;
    using namespace at::response;

    auto requestHandled = request.checkModemResponse(result);
    std::shared_ptr<MMICustomResultParams> response;
    if (requestHandled) {
        auto procedureType = request.getProcedureType();
        if (procedureType == SupplementaryServicesRequest::ProcedureType::Registration) {
            response =
                std::make_shared<MMICallForwardingResult>(IMMICustomResultParams::MMIType::CallForwardingNotification);
            response->addMessage(IMMICustomResultParams::MMIResultMessage::RegistrationSuccessful);
        }
        else if (procedureType == SupplementaryServicesRequest::ProcedureType::Deactivation) {
            response =
                std::make_shared<MMICallForwardingResult>(IMMICustomResultParams::MMIType::CallForwardingNotification);
            response->addMessage(IMMICustomResultParams::MMIResultMessage::DisablingSuccessful);
        }
        else if (procedureType == SupplementaryServicesRequest::ProcedureType::Erasure) {
            response =
                std::make_shared<MMICallForwardingResult>(IMMICustomResultParams::MMIType::CallForwardingNotification);
            response->addMessage(IMMICustomResultParams::MMIResultMessage::ErasureSuccessful);
        }
        else if (procedureType == SupplementaryServicesRequest::ProcedureType::Interrogation) {
            std::vector<ccfc::ParsedCcfc> parsed;
            if (at::response::ccfc::parse(result.response, parsed)) {
                if (ccfc::isAnyActive(parsed)) {
                    auto numbers = ccfc::getNumbers(parsed);
                    response =
                        std::make_shared<MMICallForwardingResult>(IMMICustomResultParams::MMIType::CallForwardingData,
                                                                  numbers.voice,
                                                                  numbers.fax,
                                                                  numbers.sync,
                                                                  numbers.async);
                }
                else {
                    response = std::make_shared<MMICallForwardingResult>(
                        IMMICustomResultParams::MMIType::CallForwardingNotification);
                    response->addMessage(IMMICustomResultParams::MMIResultMessage::CallForwardingDisabled);
                }
            }
        }
    }

    else {
        if (result.code == at::Result::Code::ERROR &&
            std::get<at::EquipmentErrorCode>(result.errorCode) == at::EquipmentErrorCode::PhoneBusy) {
            response =
                std::make_shared<MMICallForwardingResult>(IMMICustomResultParams::MMIType::CallForwardingNotification);
            response->addMessage(IMMICustomResultParams::MMIResultMessage::CommonMMINotSupported);
        }
    }

    if (response.get() == nullptr) {
        response =
            std::make_shared<MMICallForwardingResult>(IMMICustomResultParams::MMIType::CallForwardingNotification);
        response->addMessage(IMMICustomResultParams::MMIResultMessage::CommonFailure);
    }
    auto msg = std::make_shared<CellularMMIResultMessage>(MMIResultParams::MMIResult::Success, response);
    cellular.bus.sendUnicast(msg, ::service::name::appmgr);
    request.setHandled(requestHandled);
}

void CellularRequestHandler::handle(cellular::CallBarringRequest &request, at::Result &result)
{
    using namespace app::manager::actions;
    using namespace at::response;
    using namespace cellular;
    auto requestHandled = request.checkModemResponse(result);

    std::shared_ptr<MMICustomResultParams> response;
    if (requestHandled) {
        auto procedureType = request.getProcedureType();

        if (procedureType == cellular::SupplementaryServicesRequest::ProcedureType::Activation) {
            response = std::make_shared<MMICallBarringResult>(IMMICustomResultParams::MMIType::CallBarringNotification);
            response->addMessage(IMMICustomResultParams::MMIResultMessage::CallBarringActivated);
        }
        else if (procedureType == SupplementaryServicesRequest::ProcedureType::Deactivation) {
            response = std::make_shared<MMICallBarringResult>(IMMICustomResultParams::MMIType::CallBarringNotification);
            response->addMessage(IMMICustomResultParams::MMIResultMessage::CallBarringDeactivated);
        }
        else if (procedureType == SupplementaryServicesRequest::ProcedureType::Interrogation) {
            std::vector<clck::ClckParsed> parsed;

            if (clck::parseQueryResponse(result.response, parsed)) {
                MMICallBarringResult resp = MMICallBarringResult(IMMICustomResultParams::MMIType::CallBarringData);
                std::sort(parsed.begin(), parsed.end(), clck::iterLessIter);
                for (auto el : parsed) {
                    resp.addMessages(std::make_pair<IMMICustomResultParams::MMIResultMessage,
                                                    IMMICustomResultParams::MMIResultMessage>(
                        mmi::getClass(el.serviceClass), clck::getStatus(el.status)));
                }
                response = std::make_shared<MMICallBarringResult>(resp);
            }
        }
        else {
            response = std::make_shared<MMICallBarringResult>(IMMICustomResultParams::MMIType::CallBarringNotification);
            response->addMessage(IMMICustomResultParams::MMIResultMessage::CommonMMINotSupported);
        }
    }

    if (response == nullptr) {
        response = std::make_shared<MMICallBarringResult>(IMMICustomResultParams::MMIType::CallBarringNotification);
        response->addMessage(IMMICustomResultParams::MMIResultMessage::CommonFailure);
    }

    auto msg = std::make_shared<CellularMMIResultMessage>(MMIResultParams::MMIResult::Success, response);
    cellular.bus.sendUnicast(msg, ::service::name::appmgr);
    request.setHandled(requestHandled);
}

void CellularRequestHandler::handle(cellular::ClipRequest &request, at::Result &result)
{
    using namespace app::manager::actions;
    using namespace at::response;
    using namespace cellular;
    auto requestHandled = request.checkModemResponse(result);

    std::shared_ptr<MMICustomResultParams> response = std::make_shared<MMIClipResult>();
    if (requestHandled) {
        auto procedureType = request.getProcedureType();
        if (procedureType == SupplementaryServicesRequest::ProcedureType::Activation) {
            response->addMessage(IMMICustomResultParams::MMIResultMessage::CommonEnabled);
        }
        else if (procedureType == SupplementaryServicesRequest::ProcedureType::Deactivation) {
            response->addMessage(IMMICustomResultParams::MMIResultMessage::CommonDisabled);
        }
        else if (procedureType == SupplementaryServicesRequest::ProcedureType::Interrogation) {
            auto parsed = clip::parse(result.response[0]);
            if (parsed != std::nullopt) {
                response->addMessage(clip::getClipState(parsed->clipState));
            }
            else {
                response->addMessage(IMMICustomResultParams::MMIResultMessage::CommonFailure);
            }
        }
        else {
            response->addMessage(IMMICustomResultParams::MMIResultMessage::CommonMMINotSupported);
        }
    }
    else {
        response->addMessage(IMMICustomResultParams::MMIResultMessage::CommonFailure);
    }

    auto msg = std::make_shared<CellularMMIResultMessage>(MMIResultParams::MMIResult::Success, response);
    cellular.bus.sendUnicast(msg, ::service::name::appmgr);
    request.setHandled(requestHandled);
}

void CellularRequestHandler::handle(cellular::CallWaitingRequest &request, at::Result &result)
{
    using namespace app::manager::actions;
    using namespace cellular;
    using namespace at::response;
    auto requestHandled = request.checkModemResponse(result);

    std::shared_ptr<MMICustomResultParams> response;
    if (requestHandled) {
        auto procedureType = request.getProcedureType();
        if (procedureType == SupplementaryServicesRequest::ProcedureType::Activation) {
            response = std::make_shared<MMICallWaitingResult>(IMMICustomResultParams::MMIType::CallWaitingNotification);
            response->addMessage(IMMICustomResultParams::MMIResultMessage::CallWaitingActivated);
        }
        else if (procedureType == SupplementaryServicesRequest::ProcedureType::Deactivation) {
            response = std::make_shared<MMICallWaitingResult>(IMMICustomResultParams::MMIType::CallWaitingNotification);
            response->addMessage(IMMICustomResultParams::MMIResultMessage::CallWaitingDeactivated);
        }
        else if (procedureType == SupplementaryServicesRequest::ProcedureType::Interrogation) {
            std::vector<ccwa::CcwaParsed> parsed;
            if (ccwa::parse(result.response, parsed)) {
                MMICallWaitingResult resp = MMICallWaitingResult(IMMICustomResultParams::MMIType::CallWaitingData);
                for (auto el : parsed) {
                    resp.addMessages(std::make_pair(ccwa::getClass(el.serviceClass), ccwa::getStatus(el.status)));
                }
                response = std::make_shared<MMICallWaitingResult>(resp);
            }
            else {
                response =
                    std::make_shared<MMICallWaitingResult>(IMMICustomResultParams::MMIType::CallWaitingNotification);
                response->addMessage(IMMICustomResultParams::MMIResultMessage::CommonFailure);
            }
        }
        else {
            response = std::make_shared<MMICallWaitingResult>(IMMICustomResultParams::MMIType::CallWaitingNotification);
            response->addMessage(IMMICustomResultParams::MMIResultMessage::CommonMMINotSupported);
        }
    }
    else {
        response = std::make_shared<MMICallWaitingResult>(IMMICustomResultParams::MMIType::CallWaitingNotification);
        response->addMessage(IMMICustomResultParams::MMIResultMessage::CommonFailure);
    }
    auto msg = std::make_shared<CellularMMIResultMessage>(MMIResultParams::MMIResult::Success, response);
    cellular.bus.sendUnicast(msg, ::service::name::appmgr);
    request.setHandled(requestHandled);
}

void CellularRequestHandler::sendMmiResult(bool result)
{
    using namespace app::manager::actions;

    auto msg = std::make_shared<CellularMMIResultMessage>(result ? MMIResultParams::MMIResult::Success
                                                                 : MMIResultParams::MMIResult::Failed);
    cellular.bus.sendUnicast(msg, ::service::name::appmgr);
}
