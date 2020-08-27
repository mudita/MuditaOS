#include "DBSettingsMessage.hpp"

DBSettingsMessage::DBSettingsMessage(MessageType messageType, const SettingsRecord &rec)
    : DBMessage(messageType), record(rec)
{}

DBSettingsMessage::DBSettingsMessage(MessageType messageType, const SettingsRecord_v2 &rec, const std::string &path)
    : DBMessage(messageType), settingsRecord(rec), path(path)
{}

DBSettingsResponseMessage::DBSettingsResponseMessage(const SettingsRecord &rec,
                                                     uint32_t retCode,
                                                     uint32_t count,
                                                     MessageType respTo)
    : DBResponseMessage(retCode, count, respTo), record(rec){};

DBSettingsResponseMessage::DBSettingsResponseMessage(const SettingsRecord_v2 &rec,
                                                     uint32_t retCode,
                                                     uint32_t count,
                                                     MessageType respTo)
    : DBResponseMessage(retCode, count, respTo), settingsRecord{rec}
{}
