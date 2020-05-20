#include "DBCalllogMessage.hpp"

DBCalllogMessage::DBCalllogMessage(MessageType messageType, const CalllogRecord &rec)
    : DBMessage(messageType), record(rec)
{}

DBCalllogGetCount::DBCalllogGetCount(EntryState state) : DBCalllogMessage(MessageType::DBCalllogGetCount), state(state)
{}

DBCalllogResponseMessage ::DBCalllogResponseMessage(std::unique_ptr<std::vector<CalllogRecord>> rec,
                                                    uint32_t retCode,
                                                    uint32_t limit,
                                                    uint32_t offset,
                                                    uint32_t count,
                                                    MessageType respTo)
    : DBResponseMessage(retCode, count, respTo), records(std::move(rec)), limit(limit), offset(offset)
{
    this->count = count;
}
