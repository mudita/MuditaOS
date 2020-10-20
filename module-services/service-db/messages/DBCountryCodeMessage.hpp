// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DBMessage.hpp"
#include "Interface/CountryCodeRecord.hpp"

class DBCountryCodeMessage : public DBMessage
{
  public:
    DBCountryCodeMessage(MessageType messageType, const uint32_t _mcc, const uint32_t _country_code);

    uint32_t mcc;
    uint32_t country_code;
};

class DBCountryCodeResponseMessage : public DBResponseMessage
{
  public:
    DBCountryCodeResponseMessage(const CodesTableRow _row,
                                 bool retCode       = true,
                                 int count          = 0,
                                 MessageType respTo = MessageType::MessageTypeUninitialized);

    CodesTableRow row;
};
