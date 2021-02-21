// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-utils/time/TimeRangeParser.hpp>
#include <queries/calendar/QueryEventsGetAll.hpp>
#include <service-db/DBServiceAPI.hpp>
#include "MultiDayEventsDatabaseModel.hpp"

namespace app
{
    MultiDayEventsDatabaseModel::MultiDayEventsDatabaseModel(Application *app) : DatabaseModel(app)
    {}

    MultiDayEventsDatabaseModel::~MultiDayEventsDatabaseModel()
    {
        clearRecords();
        DatabaseModel::clear();
    }

    void MultiDayEventsDatabaseModel::setOffsetLimit(const uint32_t offset, const uint32_t limit)
    {
        this->offset = offset;
        this->limit = limit;
    }

    bool MultiDayEventsDatabaseModel::updateRecords(std::vector<std::pair<EventsRecord, TimePoint>> dbRecords)
    {
        if (!dbRecords.empty()) {
            sortAndDuplicateRecords(dbRecords);
        }

        modelIndex = 0;
        records.clear();
        offsetAndLimitCorrection();
        std::vector<std::pair<EventsRecord, TimePoint>> part(bufferedRecords.begin() + offset,
                                                             bufferedRecords.begin() + limit + offset);
        if (!part.empty()) {
            for (const auto &rec : part) {
                records.push_back(std::make_shared<std::pair<EventsRecord, TimePoint>>(rec));
            }
            return true;
        }
        LOG_ERROR("Null pointer received from DB");
        return false;
    }

    void MultiDayEventsDatabaseModel::sortAndDuplicateRecords(std::vector<std::pair<EventsRecord, TimePoint>> dbRecords)
    {
        bool addedNewRecord = false;
        LOG_DEBUG("Sort and duplicate records");
        for (const auto &pair : dbRecords) {
            const auto &[rec, _] = pair;
            if (const auto it = std::find(recordsID.begin(), recordsID.end(), rec.ID); it == recordsID.end()) {
                addedNewRecord = true;
                recordsID.push_back(rec.ID);
                if (utils::time::TimeRangeParser::isMultiDay(rec.date_from, rec.date_till)) {
                    handleMultiDayEvent(rec);
                }
                else {
                    bufferedRecords.push_back(pair);
                }
            }
        }
        if (addedNewRecord) {
            sortRecords();
        }
    }

    void MultiDayEventsDatabaseModel::handleMultiDayEvent(const EventsRecord &record)
    {
        bufferedRecords.emplace_back(record, record.date_from);
        auto daysCount = utils::time::TimeRangeParser::getDaysCount(record.date_from, record.date_till);
        for (uint32_t i = 1; i <= daysCount; i++) {
            auto tempRecord      = record;
            tempRecord.date_from = TimePointFromYearMonthDay(TimePointToYearMonthDay(record.date_from)) + date::days(i);
            bufferedRecords.emplace_back(tempRecord, record.date_from);
        }
    }

    void MultiDayEventsDatabaseModel::sortRecords()
    {
        std::sort(
            bufferedRecords.begin(),
            bufferedRecords.end(),
            [](const std::pair<EventsRecord, TimePoint> &left, const std::pair<EventsRecord, TimePoint> &right) {
                if(left.first.date_from == right.first.date_from)
                {
                    return left.first.ID < right.first.ID;
                }
                else
                {
                    return left.first.date_from < right.first.date_from;
                }
            });
    }

    uint32_t MultiDayEventsDatabaseModel::getBufferedRecordsSize() const
    {
        return bufferedRecords.size();
    }

    void MultiDayEventsDatabaseModel::clearRecords()
    {
        bufferedRecords.clear();
        recordsID.clear();
    }

    uint32_t MultiDayEventsDatabaseModel::getOffsetForCurrentTime() const
    {
        auto ymd         = TimePointToYearMonthDay(TimePointNow());
        uint32_t counter = std::count_if(
            bufferedRecords.begin(), bufferedRecords.end(), [&ymd](const std::pair<EventsRecord, TimePoint> &rec) {
                return TimePointToYearMonthDay(rec.first.date_from) < ymd;
            });
        return counter;
    }

    void MultiDayEventsDatabaseModel::offsetAndLimitCorrection()
    {
        if (offset > bufferedRecords.size()) {
            offset = bufferedRecords.size() - 1;
        }
        else if (offset == bufferedRecords.size()) {
            offset -= 1;
        }
        if (limit > bufferedRecords.size()) {
            limit = bufferedRecords.size() - offset;
        }
    }
} // namespace app
