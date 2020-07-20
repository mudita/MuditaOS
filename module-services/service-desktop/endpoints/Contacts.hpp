#pragma once

#include "Service/Service.hpp"
#include "Service/Common.hpp"
#include "DBHelper.hpp"
#include <application-phonebook/models/PhonebookModel.hpp>
#include <service-db/api/DBServiceAPI.hpp>
#include "ContactRecord.hpp"

namespace ParserStateMachine
{

    class ContactHelper : DBHelper
    {
      public:
        ContactHelper(sys::Service *_ownerServicePtr) : DBHelper(_ownerServicePtr){};

        auto createDBEntry(json11::Json createJSON, uint32_t uuid) -> sys::ReturnCodes override;
        auto requestDataFromDB(json11::Json getJSON, uint32_t uuid) -> sys::ReturnCodes override;
        auto updateDBEntry(json11::Json updateJSON, uint32_t uuid) -> sys::ReturnCodes override;
        auto deleteDBEntry(json11::Json deleteJSON, uint32_t uuid) -> sys::ReturnCodes override;

        auto requestContactByID(json11::Json getJSON, uint32_t uuid) -> sys::ReturnCodes;
        auto updateContact(ContactRecord contact) -> sys::ReturnCodes;
        auto processQueryResult(db::QueryResult *result) -> json11::Json override;
        auto getLastUUID() -> uint32_t;
        auto to_json(ContactRecord record) -> json11::Json;
        auto from_json(json11::Json contactJSON) -> std::shared_ptr<ContactRecord>;

      private:
        json11::Json receivedJson;
    };

} // namespace ParserStateMachine
