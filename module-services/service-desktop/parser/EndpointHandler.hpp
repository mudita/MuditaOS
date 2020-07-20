#pragma once

#include <json/json11.hpp>
#include "Service/Service.hpp"
#include "ParserUtils.hpp"
#include "Service/Common.hpp"
#include <service-desktop/ServiceDesktop.hpp>
#include "DBHelper.hpp"
#include "Contacts.hpp"
#include "queries/phonebook/QueryContactGet.hpp"

using namespace ParserStateMachine;

class EndpointHandler
{
  public:
    static void initHelpers(sys::Service *ownerService);

    static sys::ReturnCodes handleQueryMessage(sys::DataMessage *msg, sys::ResponseMessage *resp);
    static sys::ReturnCodes handleContactsMessage(sys::DataMessage *msg, sys::ResponseMessage *resp);
    static sys::ReturnCodes decodeMessage(db::query::ContactGetResult *message);
    static std::string createSimpleResponse(sys::ReturnCodes status, int endpoint, uint32_t uuid);
    static std::string createSimpleResponse(bool status, int endpoint, uint32_t uuid);

    static std::string buildResponseStr(std::size_t responseSize, std::string responsePayloadString);
    static sys::ReturnCodes update(
        uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService);
    static sys::ReturnCodes deviceInfo(
        uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService);
    static sys::ReturnCodes backup(
        uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService);
    static sys::ReturnCodes restore(
        uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService);
    static sys::ReturnCodes factory(
        uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService);
    static sys::ReturnCodes contacts(
        uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService);

  private:
    static std::shared_ptr<ContactHelper> contactHelper;
};
