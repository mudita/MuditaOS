#pragma once

#include "Common/Query.hpp"
#include "Context.hpp"
#include "Service/Service.hpp"
#include "Service/Common.hpp"
#include "DBHelper.hpp"
#include <application-phonebook/models/PhonebookModel.hpp>
#include <service-db/api/DBServiceAPI.hpp>
#include "ContactRecord.hpp"

namespace ParserStateMachine
{

    class ContactHelper : public DBHelper
    {
      public:
        ContactHelper(sys::Service *_ownerServicePtr) : DBHelper(_ownerServicePtr){};

        auto createDBEntry(Context &context) -> sys::ReturnCodes override;
        auto requestDataFromDB(Context &context) -> sys::ReturnCodes override;
        auto updateDBEntry(Context &context) -> sys::ReturnCodes override;
        auto deleteDBEntry(Context &context) -> sys::ReturnCodes override;

        auto requestContactByID(Context &context) -> sys::ReturnCodes;
        auto updateContact(ContactRecord contact) -> sys::ReturnCodes;
        auto processQueryResult(db::QueryResult *result) -> json11::Json;
        auto getLastUUID() -> uint32_t;
        static auto to_json(ContactRecord record) -> json11::Json;
        static auto from_json(json11::Json contactJSON) -> ContactRecord;

      private:
        json11::Json receivedJson;
    };

    namespace json::contacts
    {
        const inline std::string count           = "count";
        const inline std::string primaryName     = "priName";
        const inline std::string alternativeName = "altName";
        const inline std::string address         = "address";
        const inline std::string id              = "id";
        const inline std::string numbers         = "numbers";
        const inline std::string isBlocked       = "blocked";
        const inline std::string isFavourite     = "favourite";

    } // namespace json::contacts
} // namespace ParserStateMachine
