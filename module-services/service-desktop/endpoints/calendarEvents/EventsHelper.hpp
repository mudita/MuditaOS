#pragma once

#include "Common/Query.hpp"
#include "EventsRecord.hpp"
#include "Service/Service.hpp"
#include "Service/Common.hpp"
#include "DBHelper.hpp"
#include <memory>
#include <service-db/api/DBServiceAPI.hpp>
#include "Endpoint.hpp"

namespace parserFSM
{

    class EventsHelper final : public DBHelper
    {
      public:
        EventsHelper(sys::Service *_ownerServicePtr) : DBHelper(_ownerServicePtr){};

        auto createDBEntry(Context &context) -> sys::ReturnCodes override;
        auto requestDataFromDB(Context &context) -> sys::ReturnCodes override;
        auto updateDBEntry(Context &context) -> sys::ReturnCodes override;
        auto deleteDBEntry(Context &context) -> sys::ReturnCodes override;

        auto createEvent(Context &context) -> sys::ReturnCodes;
        auto requestEvent(Context &context) -> sys::ReturnCodes;
        auto updateEvent(Context &context) -> sys::ReturnCodes;
        auto deleteEvent(Context &context) -> sys::ReturnCodes;

        // auto createSimpleResponse(Context &context) -> std::string override;

        static auto toICS(EventsRecord record) -> std::string;
        static auto toICS(std::unique_ptr<std::vector<EventsRecord>> records) -> std::string;
        static auto fromICS(std::string msgJson) -> EventsRecord;

      private:
        std::string receivedICS;
    };

    namespace events
    {}
} // namespace parserFSM
