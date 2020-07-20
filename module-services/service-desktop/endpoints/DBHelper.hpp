#pragma once

#include "ParserStateMachine.hpp"
#include "messages/QueryMessage.hpp"
#include "queries/sms/QuerySMSSearch.hpp"
#include <Service/Common.hpp>
#include <Service/Service.hpp>

namespace ParserStateMachine
{

    class DBHelper
    {
      public:
        virtual auto createDBEntry(json11::Json createJSON, uint32_t uuid) -> sys::ReturnCodes  = 0;
        virtual auto requestDataFromDB(json11::Json getJSON, uint32_t uuid) -> sys::ReturnCodes = 0;
        virtual auto updateDBEntry(json11::Json updateJSON, uint32_t uuid) -> sys::ReturnCodes  = 0;
        virtual auto deleteDBEntry(json11::Json deleteJSON, uint32_t uuid) -> sys::ReturnCodes  = 0;

        virtual auto processQueryResult(db::QueryResult *result) -> json11::Json = 0;
        DBHelper(sys::Service *_ownerServicePtr) : ownerServicePtr(_ownerServicePtr){};

      protected:
        sys::Service *ownerServicePtr = nullptr;
        std::queue<uint32_t> uuidQueue;
    };
} // namespace ParserStateMachine
