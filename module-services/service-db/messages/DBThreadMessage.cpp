#include "DBThreadMessage.hpp"

DBThreadMessage::DBThreadMessage(MessageType messageType, const ThreadRecord &rec) : DBMessage(messageType), record(rec)
{}

DBThreadMessageGet::DBThreadMessageGet(MessageType messageType, uint32_t contactID)
    : DBMessage(messageType), contactID(contactID)
{}

DBThreadResponseMessage::DBThreadResponseMessage(std::unique_ptr<std::vector<ThreadRecord>> rec,
                                                 uint32_t retCode,
                                                 uint32_t limit,
                                                 uint32_t offset,
                                                 uint32_t count,
                                                 MessageType respTo)
    : DBResponseMessage(retCode, count, respTo), records(std::move(rec)), limit(limit), offset(offset)
{
    this->count = count;
}
