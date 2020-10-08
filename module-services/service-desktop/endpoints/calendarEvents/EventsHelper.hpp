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

        // auto createSimpleResponse(Context &context) -> std::string override;

        static auto reminderToICS(uint32_t reminder) -> list<Alarm> *;
        static auto reminderFromICS(list<Alarm> *alarm) -> uint32_t;

        static auto repeatOptionToICS(uint32_t repeat) -> Recurrence;
        static auto repeatOptionFromICS(const Recurrence &frequency) -> uint32_t;

        static auto eventRecordsToICS(std::unique_ptr<std::vector<EventsRecord>> records) -> const char *;
        static auto eventRecordsFromICS(const char *icsFile) -> EventsRecord;

      private:
        std::string receivedICS;
    };

    namespace events
    {}
} // namespace parserFSM
