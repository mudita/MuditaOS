#include "DBCountryCodeMessage.hpp"

DBCountryCodeMessage::DBCountryCodeMessage(MessageType messageType, const uint32_t _mcc, const uint32_t _country_code)
    : DBMessage(messageType), mcc(_mcc), country_code(_country_code)
{}

DBCountryCodeResponseMessage::DBCountryCodeResponseMessage(const CodesTableRow _row,
                                                           bool retCode,
                                                           int count,
                                                           MessageType respTo)
    : DBResponseMessage(retCode, count, respTo), row(_row)
{}
