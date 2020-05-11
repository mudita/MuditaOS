/*
 * Commands.cpp
 *
 *  Created on: 20 kwi 2020
 *      Author: kuba
 */
#include "../Commands.hpp"

namespace at
{

    std::vector<AT> getCommadsSet(commadsSet set)
    {
        std::vector<AT> ret;

        switch (set) {
        case commadsSet::modemInit:
            ret.push_back(AT::URC_NOTIF_CHANNEL);
            ret.push_back(AT::RI_PIN_OFF_CALL);
            ret.push_back(AT::RI_PIN_OFF_SMS);
            ret.push_back(AT::URC_UART1);
            ret.push_back(AT::AT_PIN_READY_LOGIC);
            ret.push_back(AT::URC_NOTIF_SIGNAL);
            break;
        case commadsSet::simInit:
            ret.push_back(AT::CALLER_NUMBER_PRESENTATION);
            ret.push_back(AT::SMS_TEXT_FORMAT);
            ret.push_back(AT::SMS_UCSC2);
            ret.push_back(AT::CRC_ON);
            break;
        }
        return ret;
    };

} // namespace at
