// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Record.hpp"
#include "module-db/Databases/AlarmsDB.hpp"
#include "module-db/Common/Common.hpp"
#include <time/dateCommon.hpp>
#include <utf8/UTF8.hpp>
#include <stdint.h>

namespace db::query::alarms
{
    class Add;
    class AddResult;
    class Edit;
    class EditResult;
    class Get;
    class GetResult;
    class GetLimited;
    class GetLimitedResult;
    class Remove;
    class RemoveResult;
    class TurnOffAll;
    class TurnOffAllResult;
} // namespace db::query::alarms

struct AlarmsRecord : public Record
{
    TimePoint time     = TIME_POINT_INVALID;
    uint32_t snooze    = 0;
    AlarmStatus status = AlarmStatus::On;
    uint32_t repeat    = 0;
    UTF8 path;

    AlarmsRecord() = default;
    explicit AlarmsRecord(const AlarmsTableRow &tableRow);
};

enum class AlarmsRecordField
{
    Time,
    Snooze,
    Status,
    Path
};

class AlarmsRecordInterface : public RecordInterface<AlarmsRecord, AlarmsRecordField>
{
  public:
    AlarmsRecordInterface(AlarmsDB *alarmsDb);
    ~AlarmsRecordInterface();

    bool Add(const AlarmsRecord &rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(AlarmsRecordField field, const char *str) override final;
    bool Update(const AlarmsRecord &rec) override final;
    AlarmsRecord GetByID(uint32_t id) override final;

    uint32_t GetCount() override final;
    bool TurnOffAll();

    std::unique_ptr<std::vector<AlarmsRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::unique_ptr<std::vector<AlarmsRecord>> GetLimitOffsetByField(uint32_t offset,
                                                                     uint32_t limit,
                                                                     AlarmsRecordField field,
                                                                     const char *str) override final;

    std::unique_ptr<db::QueryResult> runQuery(std::shared_ptr<db::Query> query) override;

  private:
    AlarmsDB *alarmsDB;

    std::unique_ptr<db::query::alarms::AddResult> runQueryImplAdd(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::query::alarms::RemoveResult> runQueryImplRemove(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::query::alarms::EditResult> runQueryImplEdit(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::query::alarms::GetResult> runQueryImplGetResult(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::query::alarms::GetLimitedResult> runQueryImplGetLimitedResult(std::shared_ptr<db::Query> query);
    std::unique_ptr<db::query::alarms::TurnOffAllResult> runQueryImplTurnOffAll(std::shared_ptr<db::Query> query);
};
