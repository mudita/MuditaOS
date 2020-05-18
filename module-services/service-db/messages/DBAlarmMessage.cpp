#include "DBAlarmMessage.hpp"

DBAlarmMessage ::DBAlarmMessage(MessageType messageType, const AlarmsRecord &rec) : DBMessage(messageType), record(rec)
{}

DBAlarmResponseMessage::DBAlarmResponseMessage(std::unique_ptr<std::vector<AlarmsRecord>> rec,
                                               uint32_t retCode,
                                               uint32_t count,
                                               MessageType respTo)
    : DBResponseMessage(retCode, count, respTo), records(std::move(rec))
{}
