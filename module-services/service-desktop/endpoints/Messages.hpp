#pragma once

#include "SMSTemplateRecord.hpp"
#include "Service/Service.hpp"
#include "Service/Common.hpp"
#include "DBHelper.hpp"
#include <service-db/api/DBServiceAPI.hpp>

namespace ParserStateMachine
{

    class MessageHelper : DBHelper
    {
      public:
        MessageHelper(sys::Service *_ownerServicePtr) : DBHelper(_ownerServicePtr){};

        auto createDBEntry(json11::Json createJSON, uint32_t uuid) -> sys::ReturnCodes override;
        auto requestDataFromDB(json11::Json getJSON, uint32_t uuid) -> sys::ReturnCodes override;
        auto updateDBEntry(json11::Json updateJSON, uint32_t uuid) -> sys::ReturnCodes override;
        auto deleteDBEntry(json11::Json deleteJSON, uint32_t uuid) -> sys::ReturnCodes override;

        auto requestTemplate(json11::Json getJSON, uint32_t uuid) -> sys::ReturnCodes;
        auto requestSMS(json11::Json getJSON, uint32_t uuid) -> sys::ReturnCodes;
        auto createTemplate(json11::Json createJSON, uint32_t uuid) -> sys::ReturnCodes;
        auto createSMS(json11::Json createJSON, uint32_t uuid) -> sys::ReturnCodes;
        auto deleteTemplate(json11::Json deleteJSON, uint32_t uuid) -> sys::ReturnCodes;
        auto deleteSMS(json11::Json deleteJSON, uint32_t uuid) -> sys::ReturnCodes;
        auto updateSMS(json11::Json updateJSON, uint32_t uuid) -> sys::ReturnCodes;
        auto updateTemplate(json11::Json updateJSON, uint32_t uuid) -> sys::ReturnCodes;

        auto requestMessagesCount(json11::Json getJSON, uint32_t uuid) -> sys::ReturnCodes;
        auto processQueryResult(db::QueryResult *result) -> json11::Json override;
        auto getLastUUID() -> uint32_t;
        auto to_json(SMSRecord record) -> json11::Json;
        auto to_json(ThreadRecord record) -> json11::Json;
        auto to_json(SMSTemplateRecord record) -> json11::Json;
        auto from_json(json11::Json msgJson) -> SMSTemplateRecord;

      private:
        json11::Json receivedJson;
    };

} // namespace ParserStateMachine
