#include "DBSMSTemplateMessage.hpp"

DBSMSTemplateMessage::DBSMSTemplateMessage(MessageType messageType, const SMSTemplateRecord &rec)
    : DBMessage(messageType), record(rec)
{}

DBSMSTemplateGetCount ::DBSMSTemplateGetCount() : DBSMSTemplateMessage(MessageType::DBSMSTemplateGetCount)
{}

DBSMSTemplateResponseMessage::DBSMSTemplateResponseMessage(std::unique_ptr<std::vector<SMSTemplateRecord>> rec,
                                                           uint32_t retCode,
                                                           uint32_t limit,
                                                           uint32_t offset,
                                                           uint32_t count,
                                                           MessageType respTo)
    : DBResponseMessage(retCode, count, respTo), records(std::move(rec)), limit(limit), offset(offset)
{}
