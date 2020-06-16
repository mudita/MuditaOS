#include "DBServiceMessage.hpp"

DBServiceMessageBackup::DBServiceMessageBackup(MessageType messageType, std::string backupPath)
    : DBMessage(messageType), backupPath(backupPath)
{}

DBServiceResponseMessage::DBServiceResponseMessage(uint32_t retCode, uint32_t count, MessageType respTo)
    : DBResponseMessage(retCode, count, respTo){};
