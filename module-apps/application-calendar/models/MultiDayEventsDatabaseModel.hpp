// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DatabaseModel.hpp"
#include <module-db/Interface/EventsRecord.hpp>

namespace app
{
    class MultiDayEventsDatabaseModel : public DatabaseModel<std::pair<EventsRecord, TimePoint>>
    {

        std::vector<std::pair<EventsRecord, TimePoint>> bufferedRecords;
        std::vector<uint32_t> recordsID;
        uint32_t offset = 0;
        uint32_t limit  = 0;
        void sortAndDuplicateRecords(std::vector<std::pair<EventsRecord, TimePoint>> dbRecords);
        void sortRecords();
        void handleMultiDayEvent(const EventsRecord &record);
        void offsetAndLimitCorrection();

      public:

        explicit MultiDayEventsDatabaseModel(Application *app);
        ~MultiDayEventsDatabaseModel() override;

        bool updateRecords(std::vector<std::pair<EventsRecord, TimePoint>> dbRecords) override;
        void setOffsetAndLimit(uint32_t offset, uint32_t limit);
        void clearRecords();
        [[nodiscard]] uint32_t getBufferedRecordsSize() const;
        [[nodiscard]] uint32_t getOffsetForCurrentTime() const;
    };
} // namespace app
