#pragma once

#include "Common/Query.hpp"
#include "EventsRecord.hpp"
#include "Service/Service.hpp"
#include "Service/Common.hpp"
#include "DBHelper.hpp"
#include <memory>
#include <service-db/api/DBServiceAPI.hpp>
#include "Endpoint.hpp"
#include "ical/ParserICS.hpp"

namespace parserFSM
{

    class EventsHelper final : public DBHelper
    {
      public:
        EventsHelper(sys::Service *_ownerServicePtr) : DBHelper(_ownerServicePtr)
        {
            parser = std::make_unique<ParserICS>();
        };

        auto requestDataFromDB(Context &context) -> sys::ReturnCodes override;
        auto createDBEntry(Context &context) -> sys::ReturnCodes override;
        auto updateDBEntry(Context &context) -> sys::ReturnCodes override;
        auto deleteDBEntry(Context &context) -> sys::ReturnCodes override;

        // auto createSimpleResponse(Context &context) -> std::string override;

      private:
        std::unique_ptr<ParserICS> parser;
    };

    namespace events
    {}
} // namespace parserFSM
