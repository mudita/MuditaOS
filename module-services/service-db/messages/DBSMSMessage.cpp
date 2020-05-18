#include "DBSMSMessage.hpp"

DBSMSMessage::DBSMSMessage(MessageType messageType, const SMSRecord &rec) : DBMessage(messageType), record(rec)
{}

DBSMSGetCount::DBSMSGetCount(EntryState state) : DBSMSMessage(MessageType::DBSMSGetCount), state(state)
{}

DBSMSResponseMessage::DBSMSResponseMessage(std::unique_ptr<std::vector<SMSRecord>> rec,
                                           uint32_t retCode,
                                           uint32_t count,
                                           MessageType respTo)
    : DBResponseMessage(retCode, count, respTo), records(std::move(rec))
{}
