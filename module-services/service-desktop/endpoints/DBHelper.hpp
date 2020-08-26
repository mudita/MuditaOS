#pragma once

#include "Common/Query.hpp"
#include "Context.hpp"
#include "ParserFSM.hpp"
#include "messages/QueryMessage.hpp"
#include "queries/sms/QuerySMSSearch.hpp"
#include <Service/Common.hpp>
#include <Service/Service.hpp>
#include "Endpoint.hpp"

namespace parserFSM
{

    class DBHelper
    {
      public:
        virtual auto createDBEntry(Context &context) -> sys::ReturnCodes     = 0;
        virtual auto requestDataFromDB(Context &context) -> sys::ReturnCodes = 0;
        virtual auto updateDBEntry(Context &context) -> sys::ReturnCodes     = 0;
        virtual auto deleteDBEntry(Context &context) -> sys::ReturnCodes     = 0;

        DBHelper(sys::Service *_ownerServicePtr) : ownerServicePtr(_ownerServicePtr){};

      protected:
        sys::Service *ownerServicePtr = nullptr;
    };
} // namespace parserFSM
