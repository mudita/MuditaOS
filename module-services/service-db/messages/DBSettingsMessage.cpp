#include "DBSettingsMessage.hpp"

DBSettingsMessage::DBSettingsMessage(MessageType messageType, const SettingsRecord &rec)
    : DBMessage(messageType), record(rec)
{}

DBSettingsResponseMessage::DBSettingsResponseMessage(const SettingsRecord &rec,
                                                     uint32_t retCode,
                                                     uint32_t count,
                                                     MessageType respTo)
    : DBResponseMessage(retCode, count, respTo), record(rec){};
