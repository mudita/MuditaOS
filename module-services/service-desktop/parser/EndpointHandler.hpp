#pragma once

#include <json/json11.hpp>
#include "Service/Service.hpp"
#include "ParserUtils.hpp"
#include "Service/Common.hpp"
#include <service-desktop/ServiceDesktop.hpp>
#include <string>
#include "DBHelper.hpp"
#include "queries/phonebook/QueryContactGet.hpp"

using namespace ParserStateMachine;

class EndpointHandler
{
  public:
    static void initHelpers(sys::Service *ownerService);

    static sys::ReturnCodes handleQueryMessage(sys::DataMessage *msg, sys::ResponseMessage *resp);
    static sys::ReturnCodes handleContactsMessage(sys::DataMessage *msg, sys::ResponseMessage *resp);
    static sys::ReturnCodes handleMessagesMessage(sys::DataMessage *msg, sys::ResponseMessage *resp);
    static sys::ReturnCodes handleMessageTemplatesMessage(sys::DataMessage *msg, sys::ResponseMessage *resp);

    static sys::ReturnCodes decodeMessage(db::query::ContactGetResult *message);

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
    static sys::ReturnCodes messages(
        uint8_t httpMethod, uint32_t uuid, json11::Json &body, std::string &responseStr, sys::Service *ownerService);

  private:
};
