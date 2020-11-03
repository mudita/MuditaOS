// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <URCFactory.hpp>

#include <URC_CREG.hpp>
#include <URC_CTZE.hpp>
#include <URC_CUSD.hpp>
#include <URC_QIND.hpp>
#include <URC_CMTI.hpp>
#include <URC_CLIP.hpp>
#include <URC_POWERED_DOWN.hpp>
#include <URC_RESPONSE.hpp>

using namespace at::urc;

std::unique_ptr<URC> URCFactory::Create(const std::string &urcMessage)
{
    if (urcMessage.empty()) {
        return std::make_unique<URC>(std::string());
    }

    const char headDelimiter = ':';
    auto it                  = std::find(urcMessage.begin(), urcMessage.end(), headDelimiter);
    std::string head         = std::string(urcMessage.begin(), it);
    std::string body         = std::string(it == urcMessage.end() ? urcMessage.begin() : it + 1, urcMessage.end());

    if (CTZE::isURC(head)) {
        return std::make_unique<CTZE>(body);
    }
    else if (CREG::isURC(head)) {
        return std::make_unique<CREG>(body);
    }
    else if (QIND::isURC(head)) {
        return std::make_unique<QIND>(body);
    }
    else if (CUSD::isURC(head)) {
        return std::make_unique<CUSD>(body);
    }
    else if (CMTI::isURC(head)) {
        return std::make_unique<CMTI>(body);
    }
    else if (CLIP::isURC(head)) {
        return std::make_unique<CLIP>(body);
    }
    else if (POWERED_DOWN::isURC(head)) {
        return std::make_unique<POWERED_DOWN>(body);
    }
    else if (auto rsp = URC_RESPONSE::Create(head)) {
        return rsp;
    }

    return std::make_unique<URC>(body, head);
}
