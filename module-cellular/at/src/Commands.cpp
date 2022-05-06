// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Commands.hpp>

namespace at
{

    std::vector<AT> getCommadsSet(commadsSet set)
    {
        std::vector<AT> ret;

        switch (set) {
        case commadsSet::modemInit:
            ret.push_back(AT::URC_NOTIF_CHANNEL);
            ret.push_back(AT::RI_PIN_AUTO_CALL);
            ret.push_back(AT::RI_PIN_PULSE_SMS);
            ret.push_back(AT::RI_PIN_PULSE_OTHER);
            ret.push_back(AT::URC_DELAY_ON);
            ret.push_back(AT::URC_UART1);
            ret.push_back(AT::AT_PIN_READY_LOGIC);
            ret.push_back(AT::CSQ_URC_ON);
            ret.push_back(AT::SET_DEFAULT_SCANMODE);
            ret.push_back(AT::SET_DEFAULT_SERVICEDOMAIN);
            break;
        case commadsSet::simInit:
            ret.push_back(AT::CALLER_NUMBER_PRESENTATION);
            ret.push_back(AT::SMS_TEXT_FORMAT);
            ret.push_back(AT::SMS_GSM);
            ret.push_back(AT::CRC_ON);
            break;
        case commadsSet::smsInit:
            ret.push_back(AT::SET_SMS_STORAGE);
            ret.push_back(AT::SMS_TEXT_FORMAT);
            ret.push_back(AT::SMS_GSM);
            break;
        }
        return ret;
    };

} // namespace at
